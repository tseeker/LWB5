-- --------------------------------------------------------------------------
--
-- PostgreSQL Extended Inheritance Library
--
--
-- This library contains a set of functions that extend PostgreSQL's table
-- inheritance in order to work around some of the model's limitations: it
-- propagates indexes, constraints, foreign keys and triggers, ensures
-- uniqueness for primary and unique key in the whole hierarchy, and allows
-- foreign keys referencing tables with children to reference both elements
-- of the referenced tables AND elements defined in child tables.
--
--
-- A DeepClone Development/Nocternity project
-- Copyright(C) 2007, E. Benoit <tseeker@deepclone.com>
--
-- --------------------------------------------------------------------------


--
-- Create the inheritance schema in which the various system tables
-- and related functions will reside.
--
CREATE SCHEMA inheritance;


-- --------------------------------------------------------------------------
-- PUBLIC FUNCTIONS
-- --------------------------------------------------------------------------

--
-- Function that adds a table to be ignored
--
CREATE OR REPLACE FUNCTION inheritance.ignore_table(NAME, NAME) RETURNS BOOLEAN AS $$
BEGIN
	INSERT INTO inheritance.ignore(ign_schema,ign_table) VALUES($1, $2);
	RETURN TRUE;
EXCEPTION WHEN unique_violation THEN
	RETURN FALSE;
END;
$$ LANGUAGE plpgsql;


--
-- Adds a schema to be handled to the list of schemas
--
CREATE OR REPLACE FUNCTION inheritance.add_schema(NAME) RETURNS BOOLEAN AS $$
BEGIN
	INSERT INTO inheritance.schemas (sch_name) VALUES ($1);
	RETURN TRUE;
EXCEPTION WHEN unique_violation THEN
	RETURN FALSE;
END;
$$ LANGUAGE plpgsql;


--
-- Register a foreign key that can't be handled using built-in
-- SQL because of indexes not being propagated automatically.
--
-- Usage: SELECT inheritance.foreign_key('"namespace".src_table', '"field1",field2', 'namespace."dst_table"', 'dest1,dest2', 'CASCADE', 'SET NULL');
--
CREATE OR REPLACE FUNCTION inheritance.foreign_key(on_table TEXT, src_fields TEXT, to_table TEXT, dst_fields TEXT, on_update TEXT, on_delete TEXT) RETURNS VOID AS $$
DECLARE
	f_ns	NAME;
	f_tb	NAME;
	f_fl	NAME[];
	t_ns	NAME;
	t_tb	NAME;
	t_fl	NAME[];
BEGIN
	SELECT INTO f_ns, f_tb * FROM inheritance.split_table_name(on_table);
	SELECT INTO t_ns, t_tb * FROM inheritance.split_table_name(to_table);

	SELECT INTO f_fl * FROM inheritance.split_field_list(src_fields);
	SELECT INTO t_fl * FROM inheritance.split_field_list(dst_fields);

	INSERT INTO inheritance.manual_fkeys(mfk_from_ns,mfk_from_table,mfk_from_fields,mfk_to_ns,mfk_to_table,mfk_to_fields,mfk_on_update,mfk_on_delete)
		VALUES (f_ns, f_tb, f_fl, t_ns, t_tb, t_fl, upper(on_update), upper(on_delete));
END;
$$ LANGUAGE plpgsql;


--
-- Function that handles the whole thing
--
CREATE OR REPLACE FUNCTION inheritance.init() RETURNS VOID AS $$
BEGIN
	-- Destroy any existing data
	PERFORM inheritance.kill();

	-- Update the table cache
	PERFORM inheritance.update_table_cache();

	-- Read existing constraints, indexes and triggers on tables
	PERFORM inheritance.cache_initial_triggers();
	PERFORM inheritance.cache_initial_indexes();
	PERFORM inheritance.cache_initial_constraints();

	-- Propagate constraints, indexes and triggers
	PERFORM inheritance.propagate();

	-- Handle foreign keys
	PERFORM inheritance.handle_foreign_keys();
END;
$$ LANGUAGE plpgsql;


--
-- Function that removes everything the inheritance code did previously
-- This function should be called before proceeding to any modifications
-- on a database's existing structure or before updating the inheritance
-- system.
--
CREATE OR REPLACE FUNCTION inheritance.kill() RETURNS VOID AS $$
BEGIN
	-- Flush foreign keys
	DELETE FROM inheritance.fk_cache;
	-- Flush the trigger cache
	DELETE FROM inheritance.tgr_cache;
	-- Destroy the constraint cache
	DELETE FROM inheritance.con_cache;
	-- Destroy the index cache
	DELETE FROM inheritance.idx_cache;
	-- Destroy the inheritance cache
	DELETE FROM inheritance.tbl_cache;
	-- Destroy primary key / unique key handlers
	PERFORM inheritance.destroy_unique_functions();
END;
$$ LANGUAGE plpgsql;


-- --------------------------------------------------------------------------



-- --------------------------------------------------------------------------
-- TABLES USED INTERNALLY FOR CACHING PURPOSES
-- --------------------------------------------------------------------------


--
-- Create the table that lists tables to be ignored
--
CREATE TABLE inheritance.ignore (
	ign_schema	NAME	NOT NULL,
	ign_table	NAME	NOT NULL,
	ign_oid		OID,
	PRIMARY KEY(ign_schema,ign_table)
);


--
-- Table that lists the schemas to be considered when handling inherited tables
--
CREATE TABLE inheritance.schemas (
	sch_name	NAME	PRIMARY KEY
);


--
-- Table that will contain the inheritance cache
--
CREATE TABLE inheritance.tbl_cache (
	tch_parent	OID	NOT NULL,
	tch_child	OID	NOT NULL,
	tch_direct	BOOLEAN	NOT NULL,
	PRIMARY KEY(tch_parent,tch_child,tch_direct)
);


--
-- Table that will cache constraints on tables the inheritance code handles
--
CREATE TABLE inheritance.con_cache (
	cch_table	OID NOT NULL,
	cch_constraint	OID NOT NULL,
	cch_inherited	BOOLEAN NOT NULL DEFAULT FALSE,
	PRIMARY KEY(cch_table, cch_constraint)
);


--
-- Table that will list functions created in order to handle primary keys
-- or unique keys
--
CREATE TABLE inheritance.ufn_cache (
	uch_fname	NAME NOT NULL PRIMARY KEY
);


--
-- Table that will list indexes that have been propagated from base tables;
-- it also lists indexes that are present from the beginning with the
-- ich_inherited field set to FALSE.
--
CREATE TABLE inheritance.idx_cache (
	ich_table	OID NOT NULL,
	ich_index	OID NOT NULL,
	ich_inherited	BOOLEAN NOT NULL DEFAULT FALSE,
	PRIMARY KEY(ich_table, ich_index)
);


--
-- Table that will list both initial triggers and triggers added by the
-- inheritance code.
--
CREATE TABLE inheritance.tgr_cache (
	tch_table	OID NOT NULL,
	tch_trigger	OID NOT NULL,
	tch_initial	BOOLEAN NOT NULL DEFAULT TRUE,
	PRIMARY KEY(tch_table, tch_trigger)
);


--
-- Table that stores the list of supported foreign key actions
--
CREATE TABLE inheritance.fk_actions (
	txt	VARCHAR(9)	NOT NULL UNIQUE,
	ch	CHAR		NOT NULL UNIQUE
);
COPY inheritance.fk_actions FROM STDIN;
CASCADE	c
SET NULL	n
NO ACTION	a
\.


--
-- Table to store "foreign keys" to child tables which can't be created
-- with the REFERENCES keyword due to the fact that child tables have
-- no indexes when they are created (unless these indexes are added
-- manually, which we don't want).
--
CREATE TABLE inheritance.manual_fkeys (
	mfk_from_ns	NAME NOT NULL,
	mfk_from_table	NAME NOT NULL,
	mfk_from_fields	NAME[] NOT NULL CHECK(mfk_from_fields <> '{}'),
	mfk_to_ns	NAME NOT NULL,
	mfk_to_table	NAME NOT NULL,
	mfk_to_fields	NAME[] NOT NULL CHECK(mfk_to_fields <> '{}'),
	mfk_on_update	VARCHAR(9) NOT NULL REFERENCES inheritance.fk_actions (txt),
	mfk_on_delete	VARCHAR(9) NOT NULL REFERENCES inheritance.fk_actions (txt),
	PRIMARY KEY(mfk_from_ns,mfk_from_table,mfk_from_fields),
	CHECK(array_dims(mfk_from_fields) = array_dims(mfk_to_fields))
);


--
-- Table to store foreign keys that have been removed because they were
-- referencing base tables and were replaced by sets of triggers, and
-- "manual" foreign keys.
--
CREATE TABLE inheritance.fk_cache (
	fkc_from_oid	OID	NOT NULL,
	fkc_from_attr	INT2[]	NOT NULL,
	fkc_to_oid	OID	NOT NULL,
	fkc_to_attr	INT2[]	NOT NULL,
	fkc_on_update	CHAR	NOT NULL REFERENCES inheritance.fk_actions (ch),
	fkc_on_delete	CHAR	NOT NULL REFERENCES inheritance.fk_actions (ch),
	fkc_manual	BOOLEAN	NOT NULL DEFAULT FALSE,
	fkc_constraint	OID,
	fkc_create	TEXT,
	PRIMARY KEY(fkc_from_oid,fkc_from_attr)
);


-- --------------------------------------------------------------------------



-- --------------------------------------------------------------------------
-- INTERNAL FUNCTIONS
-- --------------------------------------------------------------------------


--
-- Function that looks up a table's OID using its namespace and table name
--
CREATE OR REPLACE FUNCTION inheritance.get_table_oid(namespace NAME, tablename NAME) RETURNS OID STABLE AS $$
	SELECT c.oid FROM pg_class c, pg_namespace n
		WHERE c.relname = $2 AND n.nspname = $1 AND c.relnamespace = n.oid
$$ LANGUAGE SQL;


--
-- Function that looks up a table's name and namespace using its OID
--
CREATE OR REPLACE FUNCTION inheritance.get_table_name(tableoid OID, OUT namespace NAME, OUT tablename NAME) STABLE AS $$
	SELECT n.nspname, c.relname FROM pg_class c, pg_namespace n WHERE c.oid = $1 AND n.oid = c.relnamespace
$$ LANGUAGE SQL;


--
-- Function that looks up the names of a set of attributes
--
CREATE OR REPLACE FUNCTION inheritance.get_attr_names(tableoid OID, attrids INT2[]) RETURNS NAME[] STABLE AS $$
DECLARE
	tmp	NAME;
	attr	NAME[];
	i	INT;
BEGIN
	attr := '{}';
	FOR i IN array_lower(attrids, 1) .. array_upper(attrids, 1)
	LOOP
		SELECT INTO tmp attname FROM pg_attribute WHERE attrelid = tableoid AND attnum = attrids[i];
		IF NOT FOUND
			THEN RETURN NULL;
		END IF;
		attr[i] := tmp;
	END LOOP;
	RETURN attr;
END;
$$ LANGUAGE plpgsql;


--
-- Function that sets the OIDs in the list of tables to be ignored
--
CREATE OR REPLACE FUNCTION inheritance.update_ignore_oids() RETURNS VOID AS $$
DECLARE
	rec	RECORD;
	rd_oid	OID;
BEGIN
	FOR rec IN SELECT ign_schema, ign_table FROM inheritance.ignore
			WHERE ign_oid IS NULL
	LOOP
		rd_oid := inheritance.get_table_oid(rec.ign_schema, rec.ign_table);
		IF rd_oid IS NOT NULL THEN
			UPDATE inheritance.ignore SET ign_oid=rd_oid
				WHERE ign_schema=rec.ign_schema AND ign_table=rec.ign_table;
		END IF;
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that stores a table's inheritance data
--
CREATE OR REPLACE FUNCTION inheritance.add_to_cache(parent OID, child OID) RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	-- Generate indirect inheritance
	FOR rec IN SELECT tch_parent FROM inheritance.tbl_cache WHERE tch_child=parent
	LOOP
		INSERT INTO inheritance.tbl_cache(tch_parent,tch_child,tch_direct)
			VALUES (rec.tch_parent, child, FALSE);
	END LOOP;

	-- Generate direct inheritance
	INSERT INTO inheritance.tbl_cache(tch_parent,tch_child,tch_direct)
		VALUES (parent, child, TRUE);
END;
$$ LANGUAGE plpgsql;


--
-- Function that looks up a table's children and stores them in the cache
--
CREATE OR REPLACE FUNCTION inheritance.find_children(parent OID) RETURNS VOID AS $$
DECLARE
	child	RECORD;
BEGIN
	FOR child IN SELECT c.oid FROM pg_class c, pg_inherits i
			WHERE c.oid = i.inhrelid AND i.inhparent = parent
			  AND c.oid NOT IN (SELECT ign_oid FROM inheritance.ignore WHERE ign_oid IS NOT NULL)
	LOOP
		-- Add to cache
		PERFORM inheritance.add_to_cache(parent, child.oid);
		-- Find child tables
		PERFORM inheritance.find_children(child.oid);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that will update the inheritance cache
--
CREATE OR REPLACE FUNCTION inheritance.update_table_cache() RETURNS VOID AS $$
DECLARE
	base	RECORD;
BEGIN
	-- Make sure the ignore list is up-to-date
	PERFORM inheritance.update_ignore_oids();

	-- Find tables that do not inherit from any other table, are not set to be ignored and
	-- are in the list of schemas we are supposed to examine
	FOR base IN SELECT oid FROM pg_class
			WHERE relnamespace IN (SELECT oid FROM pg_namespace WHERE nspname IN (SELECT sch_name FROM inheritance.schemas))
			  AND oid NOT IN (SELECT ign_oid FROM inheritance.ignore WHERE ign_oid IS NOT NULL)
			  AND oid NOT IN (SELECT inhrelid FROM pg_inherits)
			  AND relhassubclass
	LOOP
		PERFORM inheritance.find_children(base.oid);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that drops a constraint using its OID
--
CREATE OR REPLACE FUNCTION inheritance.drop_constraint(cst OID) RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	SELECT INTO rec c.conname AS cn, r.relname AS tn, n.nspname AS nn
		   FROM pg_constraint c, pg_class r, pg_namespace n
		  WHERE c.oid = cst AND r.oid = c.conrelid AND n.oid = r.relnamespace;
	IF FOUND
	THEN
		EXECUTE 'ALTER TABLE "' || rec.nn || '"."' || rec.tn
			|| '" DROP CONSTRAINT "' || rec.cn || '"';
	END IF;
END;
$$ LANGUAGE plpgsql;


--
-- Function that drops a trigger using its OID
--
CREATE OR REPLACE FUNCTION inheritance.drop_trigger(tg OID) RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	SELECT INTO rec t.tgname AS n, r.relname AS tn, n.nspname AS nn
		   FROM pg_trigger t, pg_class r, pg_namespace n
		  WHERE t.oid = tg AND r.oid = t.tgrelid AND n.oid = r.relnamespace;
	IF FOUND THEN
		EXECUTE 'DROP TRIGGER "' || rec.n || '" ON "' || rec.nn || '"."' || rec.tn || '"';
--		RAISE NOTICE 'DROP TRIGGER "%" ON "%"."%"', rec.n, rec.nn, rec.tn;
	END IF;
END;
$$ LANGUAGE plpgsql;


--
-- Function that drops an index using its OID
--
CREATE OR REPLACE FUNCTION inheritance.drop_index(idx OID) RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	SELECT INTO rec * FROM inheritance.get_table_name(idx);
	IF rec.namespace IS NOT NULL THEN
		EXECUTE 'DROP INDEX "' || rec.namespace || '"."' || rec.tablename || '"';
	END IF;
END;
$$ LANGUAGE plpgsql;


--
-- Trigger function that drops constraints added by the inheritance code
--
CREATE OR REPLACE FUNCTION inheritance.drop_inherited_constraints() RETURNS TRIGGER AS $$
BEGIN
	IF OLD.cch_inherited THEN
		PERFORM inheritance.drop_constraint(OLD.cch_constraint);
	END IF;
	RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER con_cache_drop_inherited AFTER DELETE ON inheritance.con_cache
	FOR EACH ROW EXECUTE PROCEDURE inheritance.drop_inherited_constraints();


--
-- Trigger function that drops triggers added by the inheritance code
--
CREATE OR REPLACE FUNCTION inheritance.drop_inherited_triggers() RETURNS TRIGGER AS $$
BEGIN
	IF NOT OLD.tch_initial THEN
		PERFORM inheritance.drop_trigger(OLD.tch_trigger);
	END IF;
	RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tgr_cache_drop_inherited AFTER DELETE ON inheritance.tgr_cache
	FOR EACH ROW EXECUTE PROCEDURE inheritance.drop_inherited_triggers();


--
-- Trigger function that drops indexes added by the inheritance code
--
CREATE OR REPLACE FUNCTION inheritance.drop_inherited_indexes() RETURNS TRIGGER AS $$
BEGIN
	IF OLD.ich_inherited THEN
		PERFORM inheritance.drop_index(OLD.ich_index);
	END IF;
	RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER idx_cache_drop_inherited AFTER DELETE ON inheritance.idx_cache
	FOR EACH ROW EXECUTE PROCEDURE inheritance.drop_inherited_indexes();


--
-- Function that looks up a table's original constrains and stores them in
-- the constraint cache
--
CREATE OR REPLACE FUNCTION inheritance.cache_table_constraints(tbl OID) RETURNS VOID AS $$
BEGIN
	INSERT INTO inheritance.con_cache (cch_table, cch_constraint)
		SELECT tbl,c.oid FROM pg_constraint c WHERE c.conrelid = tbl;
END;
$$ LANGUAGE plpgsql;


--
-- Function that caches indexes for all existing tables handled by the inheritance code
--
CREATE OR REPLACE FUNCTION inheritance.cache_initial_indexes() RETURNS VOID AS $$
BEGIN
	INSERT INTO inheritance.idx_cache (ich_table, ich_index)
		SELECT indrelid,indexrelid FROM pg_index WHERE indrelid IN (
			SELECT DISTINCT tch_parent AS tid FROM inheritance.tbl_cache
				UNION SELECT DISTINCT tch_child AS tid FROM inheritance.tbl_cache
		);
END;
$$ LANGUAGE plpgsql;


--
-- Function that caches triggers for all existing tables handled by the inheritance code
--
CREATE OR REPLACE FUNCTION inheritance.cache_initial_triggers() RETURNS VOID AS $$
BEGIN
	INSERT INTO inheritance.tgr_cache (tch_table, tch_trigger)
		SELECT tgrelid,oid FROM pg_trigger WHERE tgrelid IN (
			SELECT DISTINCT tch_parent AS tid FROM inheritance.tbl_cache
				UNION SELECT DISTINCT tch_child AS tid FROM inheritance.tbl_cache
		) AND NOT tgisconstraint;
END;
$$ LANGUAGE plpgsql;


--
-- Function that caches constraints for all existing tables handled by the inheritance code
--
CREATE OR REPLACE FUNCTION inheritance.cache_initial_constraints() RETURNS VOID AS $$
BEGIN
	INSERT INTO inheritance.con_cache (cch_table, cch_constraint)
		SELECT conrelid,oid FROM pg_constraint WHERE conrelid IN (
			SELECT DISTINCT tch_parent AS tid FROM inheritance.tbl_cache
				UNION SELECT DISTINCT tch_child AS tid FROM inheritance.tbl_cache
		);
END;
$$ LANGUAGE plpgsql;


--
-- Function that copies foreign key constraints from a table to another
--
CREATE OR REPLACE FUNCTION inheritance.copy_fkey_constraints(src OID, dst OID) RETURNS VOID AS $$
DECLARE
	trec	RECORD;
	rec	RECORD;
	at	TEXT;
	i	INT;
BEGIN
	-- Get the destination table's name and namespace name
	SELECT INTO trec * FROM inheritance.get_table_name(dst);

	-- Get the constraints to copy
	FOR rec IN SELECT oid FROM pg_constraint WHERE contype = 'f' AND oid IN (
			SELECT cch_constraint FROM inheritance.con_cache WHERE cch_table=src AND NOT cch_inherited)
	LOOP
		EXECUTE 'ALTER TABLE "' || trec.namespace || '"."' || trec.tablename || '" ADD ' || pg_get_constraintdef(rec.oid);
	END LOOP;

	-- Update the constraint cache
	INSERT INTO inheritance.con_cache (cch_table, cch_constraint, cch_inherited)
		SELECT dst,c.oid,TRUE FROM pg_constraint c WHERE c.conrelid = dst AND c.oid NOT IN (
			SELECT cch_constraint FROM inheritance.con_cache WHERE cch_table=dst
		);
END;
$$ LANGUAGE plpgsql;


--
-- Function that generates a function (how perverse!) to check for a table's inherited
-- primary key.
--
CREATE OR REPLACE FUNCTION inheritance.make_key_function(fn NAME, tbl OID, fl SMALLINT[], kt TEXT) RETURNS VOID AS $$
DECLARE
	tdet	RECORD;
	fdet	RECORD;
	code	TEXT;
	ncode	TEXT;
	ocode	TEXT;
	i	INT;
BEGIN
	SELECT INTO tdet * FROM inheritance.get_table_name(tbl);

	ncode := '';
	ocode := '';
	FOR i IN array_lower(fl, 1) .. array_upper(fl, 1)
	LOOP
		IF ocode <> ''
		THEN
			ocode := ocode || ' AND ';
			ncode := ncode || ' AND ';
		END IF;

		SELECT INTO fdet attname FROM pg_attribute WHERE attrelid=tbl AND attnum=fl[i];
		ncode := ncode || '"' || fdet.attname || '"=NEW."' || fdet.attname || '"';
		ocode := ocode || '"' || fdet.attname || '"<>OLD."' || fdet.attname || '"';
	END LOOP;

	EXECUTE 'CREATE OR REPLACE FUNCTION inheritance.' || fn || '() RETURNS TRIGGER AS '''
		|| 'DECLARE c INT; '
		|| 'BEGIN '
		||	'IF TG_OP=''''INSERT'''' THEN '
		||		'SELECT INTO c COUNT(*) FROM "' || tdet.namespace || '"."' || tdet.tablename || '" WHERE ' || ncode || ';'
		||	'ELSE '
		||		'SELECT INTO c COUNT(*) FROM "' || tdet.namespace || '"."' || tdet.tablename || '" WHERE ' || ncode || ' AND ' || ocode || ';'
		||	'END IF;'
		||	'IF FOUND AND c>0 THEN '
		||		'RAISE EXCEPTION ''''Duplicate ' || kt || ' key value in table "%" inherited from "' || tdet.namespace
					|| '"."' || tdet.tablename || '"'''', TG_RELNAME;'
		||	'END IF;'
		||	'RETURN NEW;'
		|| 'END;'
		|| ''' LANGUAGE plpgsql';
	INSERT INTO inheritance.ufn_cache(uch_fname) VALUES (fn);
END;
$$ LANGUAGE plpgsql;


--
-- Function that propagates the base tables' primary keys
--
CREATE OR REPLACE FUNCTION inheritance.propagate_primary_keys() RETURNS VOID AS $$
DECLARE
	pkey	RECORD;
	ctbl	RECORD;
	fn	NAME;
BEGIN
	-- Loop on all primary keys found for the base tables
	FOR pkey IN SELECT DISTINCT i.tch_parent AS t,c.conkey AS k
		   FROM inheritance.tbl_cache i, pg_constraint c
		  WHERE i.tch_parent NOT IN (SELECT DISTINCT tch_child FROM inheritance.tbl_cache)
			AND c.conrelid=i.tch_parent AND c.contype='p'
	LOOP
		-- Generate the function that checks for this specific primary key
		fn := 'pkey_check_' || pkey.t;
		PERFORM inheritance.make_key_function(fn, pkey.t, pkey.k, 'primary');

		-- Create the trigger on the table itself
		SELECT INTO ctbl * FROM inheritance.get_table_name(pkey.t);
		EXECUTE 'CREATE TRIGGER ' || fn || ' BEFORE INSERT OR UPDATE '
			|| 'ON "' || ctbl.namespace || '"."' || ctbl.tablename || '" '
			|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.' || fn || '()';
		INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
			SELECT oid, pkey.t, FALSE FROM pg_trigger
				WHERE tgname = fn AND tgrelid = pkey.t;

		-- Create the trigger for all tables that inherit the base
		FOR ctbl IN SELECT r.relname AS tn, n.nspname AS nn, r.oid AS o
			   FROM	inheritance.tbl_cache i, pg_class r, pg_namespace n
			  WHERE	i.tch_parent = pkey.t AND r.oid = i.tch_child AND n.oid = r.relnamespace
		LOOP
			EXECUTE 'CREATE TRIGGER ' || fn || ' BEFORE INSERT OR UPDATE '
				|| 'ON "' || ctbl.nn || '"."' || ctbl.tn || '" '
				|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.' || fn || '()';
			INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
				SELECT t.oid, ctbl.o, FALSE FROM pg_trigger t
				WHERE t.tgname = fn AND tgrelid = ctbl.o;
		END LOOP;
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that propagates the tables' unique keys
--
CREATE OR REPLACE FUNCTION inheritance.propagate_unique_keys() RETURNS VOID AS $$
DECLARE
	ukey	RECORD;
	ctbl	RECORD;
	fn	NAME;
BEGIN
	-- Loop on all primary keys found for the base tables
	FOR ukey IN SELECT DISTINCT i.tch_parent AS t,c.oid AS c,c.conkey AS k
		   FROM	inheritance.tbl_cache i, pg_constraint c
		  WHERE c.conrelid=i.tch_parent AND c.contype='u'
	LOOP
		-- Generate the function that checks for this specific key
		fn := 'ukey_check_' || ukey.c;
		PERFORM inheritance.make_key_function(fn, ukey.t, ukey.k, 'unique');

		-- Create the trigger on the table itself
		SELECT INTO ctbl * FROM inheritance.get_table_name(ukey.t);
		EXECUTE 'CREATE TRIGGER ' || fn || ' BEFORE INSERT OR UPDATE '
			|| 'ON "' || ctbl.namespace || '"."' || ctbl.tablename || '" '
			|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.' || fn || '()';
		INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
			SELECT oid, ukey.t, FALSE FROM pg_trigger
				WHERE tgname = fn AND tgrelid = ukey.t;

		-- Create the trigger for all tables that inherit the base
		FOR ctbl IN SELECT r.relname AS tn, n.nspname AS nn, r.oid AS o
			   FROM	inheritance.tbl_cache i, pg_class r, pg_namespace n
			  WHERE	i.tch_parent = ukey.t AND r.oid = i.tch_child AND n.oid = r.relnamespace
		LOOP
			EXECUTE 'CREATE TRIGGER ' || fn || ' BEFORE INSERT OR UPDATE '
				|| 'ON "' || ctbl.nn || '"."' || ctbl.tn || '" '
				|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.' || fn || '()';
			INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
				SELECT oid, ctbl.o, FALSE FROM pg_trigger
					WHERE tgname = fn AND tgrelid = ctbl.o;
		END LOOP;
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that duplicates an index
--
CREATE OR REPLACE FUNCTION inheritance.duplicate_index(idx OID, tbl OID) RETURNS VOID AS $$
DECLARE
	irec	RECORD;
	trec	RECORD;
	attr	RECORD;
	fl	TEXT;
	i	INT;
BEGIN
	SELECT INTO irec CASE indisunique WHEN TRUE THEN ' UNIQUE' ELSE '' END AS unq, indkey::int2[] AS fls, indrelid
		FROM pg_index WHERE indexrelid = idx;
	SELECT INTO trec * FROM inheritance.get_table_name(tbl);

	fl := '';
	FOR i IN array_lower(irec.fls, 1) .. array_upper(irec.fls, 1)
	LOOP
		IF fl <> ''
			THEN fl := fl || ',';
		END IF;
		SELECT INTO attr attname FROM pg_attribute WHERE attrelid = irec.indrelid AND attnum = irec.fls[i];
		fl := fl || '"' || attr.attname || '"';
	END LOOP;

	EXECUTE 'CREATE' || irec.unq || ' INDEX inh_idx_dup_' || idx || '_' || tbl
		|| ' ON "' || trec.namespace || '"."' || trec.tablename || '" (' || fl || ')';
END;
$$ LANGUAGE plpgsql;


--
-- Function that propagates indexes from tables to their children
--
CREATE OR REPLACE FUNCTION inheritance.propagate_indexes() RETURNS VOID AS $$
DECLARE
	idx	RECORD;
BEGIN
	FOR idx IN SELECT i.ich_index, t.tch_child FROM inheritance.idx_cache i, inheritance.tbl_cache t
			WHERE NOT i.ich_inherited AND t.tch_parent = i.ich_table
	LOOP
		PERFORM inheritance.duplicate_index(idx.ich_index, idx.tch_child);
	END LOOP;

	INSERT INTO inheritance.idx_cache (ich_table, ich_index, ich_inherited)
		SELECT indrelid,indexrelid,TRUE FROM pg_index WHERE indrelid IN (
				SELECT DISTINCT tch_parent AS tid FROM inheritance.tbl_cache
					UNION SELECT DISTINCT tch_child AS tid FROM inheritance.tbl_cache
			) AND indexrelid NOT IN (SELECT ich_index FROM inheritance.idx_cache WHERE NOT ich_inherited);
END;
$$ LANGUAGE plpgsql;



--
-- Function that copies all of the base tables' constraints to their children
--
CREATE OR REPLACE FUNCTION inheritance.propagate() RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	-- Propagate indexes
	PERFORM inheritance.propagate_indexes();

	-- Propagate the base tables' primary keys
	PERFORM inheritance.propagate_primary_keys();

	-- Propagate the tables' unique keys
	PERFORM inheritance.propagate_unique_keys();

	-- Propagate triggers
	PERFORM inheritance.propagate_triggers();

	-- Copy CHECK and FOREIGN KEY constraints
	FOR rec IN SELECT tch_parent, tch_child FROM inheritance.tbl_cache
	LOOP
		PERFORM inheritance.copy_fkey_constraints(rec.tch_parent, rec.tch_child);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that deletes functions used for primary key and unique key
-- enforcement.
--
CREATE OR REPLACE FUNCTION inheritance.destroy_unique_functions() RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	FOR rec IN SELECT * FROM inheritance.ufn_cache
	LOOP
		EXECUTE 'DROP FUNCTION inheritance."' || rec.uch_fname || '"()'; -- CASCADE';
	END LOOP;

	DELETE FROM inheritance.ufn_cache;
END;
$$ LANGUAGE plpgsql;


--
-- Function that propagates triggers from base tables to their children
--
CREATE OR REPLACE FUNCTION inheritance.propagate_triggers() RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	FOR rec IN SELECT t.tch_trigger AS tg, i.tch_child AS tb
		   FROM inheritance.tgr_cache t, inheritance.tbl_cache i
		  WHERE i.tch_parent = t.tch_table AND t.tch_initial
	LOOP
		PERFORM inheritance.copy_trigger(rec.tg, rec.tb);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that copies a trigger to some table
--
CREATE OR REPLACE FUNCTION inheritance.copy_trigger(tg OID, tbl OID) RETURNS VOID AS $$
DECLARE
	tbr	RECORD;	-- Table record
	tgr	RECORD;	-- Trigger record
	prr	RECORD;	-- Procedure record
	code	TEXT;
	need_or	BOOLEAN;
	astring	BYTEA;
BEGIN
	-- Get trigger, table and procedure data
	SELECT INTO tbr * FROM inheritance.get_table_name(tbl);
	SELECT INTO tgr tgfoid AS fo, tgtype::int::bit(5) AS t, tgargs AS a FROM pg_trigger WHERE oid = tg;
	SELECT INTO prr p.proname AS pn, n.nspname AS nn FROM pg_proc p, pg_namespace n
		WHERE p.oid = tgr.fo AND n.oid = p.pronamespace;

	-- Generate the beginning of the CREATE TRIGGER instruction
	code := 'CREATE TRIGGER tgr_dup_' || tg || '_' || tbl || ' '
		|| (CASE (tgr.t << 3)::bit(1) WHEN B'1' THEN 'BEFORE' ELSE 'AFTER' END)
		|| ' ';

	-- Check the operations to which the trigger applies
	need_or := FALSE;
	IF (tgr.t << 2)::bit(1) = B'1' THEN
		code := code || 'INSERT ';
		need_or := TRUE;
	END IF;
	IF (tgr.t << 1)::bit(1) = B'1' THEN
		code := code || (CASE need_or WHEN TRUE THEN 'OR ' ELSE '' END) || 'DELETE ';
		need_or := TRUE;
	END IF;
	IF tgr.t::bit(1) = B'1' THEN
		code := code || (CASE need_or WHEN TRUE THEN 'OR ' ELSE '' END) || 'UPDATE ';
	END IF;

	-- Add table name, FOR EACH thingy and procedure name
	code := code || 'ON "' || tbr.namespace || '"."' || tbr.tablename || '" FOR EACH '
		|| (CASE (tgr.t << 4)::bit(1) WHEN B'1' THEN 'ROW' ELSE 'STATEMENT' END)
		|| ' EXECUTE PROCEDURE "' || prr.nn || '"."' || prr.pn || '"(';

	-- Generate the arguments list
	astring := tgr.a; need_or := FALSE;
	WHILE position('\\000'::bytea IN astring) > 0
	LOOP
		IF need_or
			THEN code := code || ',';
			ELSE need_or := TRUE;
		END IF;

		code := code || quote_literal(encode(substring(astring FROM 1 FOR position('\\000'::bytea IN astring) - 1), 'escape'));
		astring := substring(astring FROM position('\\000'::bytea IN astring) + 1);
	END LOOP;
	code := code || ')';

	-- Create the trigger and add it to the list
	EXECUTE code;
	INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
		SELECT oid, tbl, FALSE FROM pg_trigger
			WHERE tgname = 'tgr_dup_' || tg || '_' || tbl AND tgrelid = tbl;
END;
$$ LANGUAGE plpgsql;


--
-- This function splits a string containing a namespace and table name into
-- two strings, the namespace's name and the table's name.
--
CREATE OR REPLACE FUNCTION inheritance.split_table_name(complete TEXT, OUT nsname NAME, OUT tblname NAME) AS $$
DECLARE
	s	TEXT;
BEGIN
	s := split_part(complete, '.', 1);
	IF substr(s, 1, 1) = '"'
		THEN nsname := substr(s, 2, char_length(s) - 2);
		ELSE nsname := lower(s);
	END IF;

	s := split_part(complete, '.', 2);
	IF substr(s, 1, 1) = '"'
		THEN tblname := substr(s, 2, char_length(s) - 2);
		ELSE tblname := lower(s);
	END IF;
END;
$$ LANGUAGE plpgsql;


--
-- This function splits a string of coma-separated field names into an array
-- of names.
--
CREATE OR REPLACE FUNCTION inheritance.split_field_list(fstr TEXT, OUT lst NAME[]) AS $$
DECLARE
	s	TEXT;
	i	INT;
BEGIN
	lst := '{}';
	i := 1;
	LOOP
		s := split_part(fstr, ',', i);
		IF s = ''
			THEN EXIT;
		END IF;

		IF substr(s, 1, 1) = '"'
			THEN lst[i] := substr(s, 2, char_length(s) - 2);
			ELSE lst[i] := lower(s);
		END IF;

		i := i + 1;
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- This function handles foreign keys (be them "real" SQL foreign keys or manual
-- foreign keys created by inheritance.foreign_key()).
--
CREATE OR REPLACE FUNCTION inheritance.handle_foreign_keys() RETURNS VOID AS $$
BEGIN
	-- Cache real SQL foreign keys
	PERFORM inheritance.cache_real_fkeys();
	-- Cache manual foreign keys
	PERFORM inheritance.cache_manual_fkeys();
	-- Create the functions and triggers
	PERFORM inheritance.make_fkey_triggers();
END;
$$ LANGUAGE plpgsql;


--
-- This function reads the list of real foreign keys that refer to tables
-- handled by the inheritance code. If the foreign keys have been added by
-- inheritance, it removes them silently and stores them into the cache as
-- if they were "manual" foreign keys
--
CREATE OR REPLACE FUNCTION inheritance.cache_real_fkeys() RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	-- List the foreign keys
	INSERT INTO inheritance.fk_cache (fkc_from_oid,fkc_from_attr,fkc_to_oid,fkc_to_attr,fkc_constraint,fkc_on_update,fkc_on_delete,fkc_manual)
		SELECT c.conrelid, c.conkey, c.confrelid, c.confkey, c.oid, c.confupdtype, c.confdeltype, (
				SELECT COUNT(*)=1 FROM inheritance.con_cache
					WHERE cch_table=c.conrelid AND cch_inherited='t' AND cch_constraint=c.oid)
			FROM pg_constraint c WHERE c.contype='f' AND c.confrelid IN (
				SELECT DISTINCT tch_parent AS tid FROM inheritance.tbl_cache
					UNION SELECT DISTINCT tch_child AS tid FROM inheritance.tbl_cache);

	-- Drop constraints that were copied by the inheritance code
	DELETE FROM inheritance.con_cache WHERE cch_constraint IN (
		SELECT fkc_constraint FROM inheritance.fk_cache WHERE fkc_manual);
	DELETE FROM inheritance.fk_cache WHERE fkc_manual;

	-- Drop the other constraints
	FOR rec IN SELECT fkc_constraint FROM inheritance.fk_cache WHERE NOT fkc_manual
	LOOP
		UPDATE inheritance.fk_cache SET fkc_create = pg_get_constraintdef(rec.fkc_constraint)
			WHERE fkc_constraint = rec.fkc_constraint;
		PERFORM inheritance.drop_constraint(rec.fkc_constraint);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- This function fetches the list of manual foreign keys and adds them to
-- the cache if possible AND necessary.
--
CREATE OR REPLACE FUNCTION inheritance.cache_manual_fkeys() RETURNS VOID AS $$
DECLARE
	mfk	RECORD;
	stbl	OID;
	dtbl	OID;
	sfld	INT2[];
	dfld	INT2[];
	tmp	INT2;
	i	INT;
BEGIN
	<<cmfk_main_loop>>
	FOR mfk IN SELECT * FROM inheritance.manual_fkeys
	LOOP
		-- Get the source table's OID
		stbl := inheritance.get_table_oid(mfk.mfk_from_ns, mfk.mfk_from_table);
		CONTINUE WHEN stbl IS NULL;

		-- Get the destination table's OID
		dtbl := inheritance.get_table_oid(mfk.mfk_to_ns, mfk.mfk_to_table);
		CONTINUE WHEN dtbl IS NULL;

		-- Get the list of fields
		sfld := '{}'; dfld := '{}';
		FOR i IN array_lower(mfk.mfk_from_fields, 1) .. array_upper(mfk.mfk_from_fields, 1)
		LOOP
			SELECT INTO tmp attnum FROM pg_attribute
				WHERE attrelid = stbl AND attname = mfk.mfk_from_fields[i];
			CONTINUE cmfk_main_loop WHEN NOT FOUND;
			sfld[i] := tmp;

			SELECT INTO tmp attnum FROM pg_attribute
				WHERE attrelid = dtbl AND attname = mfk.mfk_to_fields[i];
			CONTINUE cmfk_main_loop WHEN NOT FOUND;
			dfld[i] := tmp;
		END LOOP;

		-- Insert data
		INSERT INTO inheritance.fk_cache (fkc_from_oid,fkc_from_attr,fkc_to_oid,fkc_to_attr,fkc_manual,fkc_on_update,fkc_on_delete)
			VALUES (stbl, sfld, dtbl, dfld, TRUE,
					(SELECT ch FROM inheritance.fk_actions WHERE txt=mfk.mfk_on_update),
					(SELECT ch FROM inheritance.fk_actions WHERE txt=mfk.mfk_on_delete)
				);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Trigger function that restore foreign keys that had been deleted
--
CREATE OR REPLACE FUNCTION inheritance.restore_deleted_fkeys() RETURNS TRIGGER AS $$
DECLARE
	rec	RECORD;
BEGIN
	IF NOT OLD.fkc_manual THEN
		SELECT INTO rec * FROM inheritance.get_table_name(OLD.fkc_from_oid);
		EXECUTE 'ALTER TABLE "' || rec.namespace || '"."' || rec.tablename || '" ADD ' || OLD.fkc_create;
	END IF;
	RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER fk_cache_restore_deleted AFTER DELETE ON inheritance.fk_cache
	FOR EACH ROW EXECUTE PROCEDURE inheritance.restore_deleted_fkeys();


--
-- Function that will generate trigger functions that will be applied
-- before deletion or update on referenced tables and their children.
--
CREATE OR REPLACE FUNCTION inheritance.make_referenced_functions(tbl OID) RETURNS VOID AS $$
DECLARE
	dcode	TEXT;
	ucode	TEXT;
	trec	RECORD;
	trec2	RECORD;
	rec	RECORD;
	rec2	RECORD;
	sattr	NAME[];
	dattr	NAME[];
	i	INT;
	acode	TEXT;
BEGIN
	dcode := 'CREATE FUNCTION inheritance.tgr_fk_' || tbl || '_del() RETURNS TRIGGER AS $a$DECLARE c INT;BEGIN ';
	ucode := 'CREATE FUNCTION inheritance.tgr_fk_' || tbl || '_upd_d() RETURNS TRIGGER AS $a$DECLARE c INT;BEGIN ';

	SELECT INTO trec * FROM inheritance.get_table_name(tbl);

	-- For each set of referenced keys
	FOR rec IN SELECT DISTINCT fkc_to_attr AS attr FROM inheritance.fk_cache WHERE fkc_to_oid = tbl
	LOOP
		dattr := inheritance.get_attr_names(tbl, rec.attr);

		-- Generate condition for the UPDATE trigger
		ucode := ucode || 'IF ';
		FOR i IN array_lower(dattr, 1) .. array_upper(dattr, 1)
		LOOP
			ucode := ucode || 'NEW."' || dattr[i] || '" <> OLD."' || dattr[i] || '" ';
			IF i < array_upper(dattr, 1)
				THEN ucode := ucode || 'OR ';
			END IF;
		END LOOP;
		ucode := ucode || 'THEN ';

		-- For each referencing table
		FOR rec2 IN SELECT fkc_from_oid AS oid, fkc_from_attr AS attr, fkc_on_delete AS od, fkc_on_update AS ou
			FROM inheritance.fk_cache WHERE fkc_to_oid=tbl AND fkc_to_attr=rec.attr
		LOOP
			-- Fetch table and namespace
			SELECT INTO trec2 * FROM inheritance.get_table_name(rec2.oid);

			-- Fetch attribute names
			sattr := inheritance.get_attr_names(rec2.oid, rec2.attr);
			acode := '';
			FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
			LOOP
				acode := acode || '"' || sattr[i] || '"=OLD."' || dattr[i] || '"';
				IF i < array_upper(sattr, 1)
					THEN acode := acode || ' AND ';
				END IF;
			END LOOP;

			-- Handle "ON DELETE CASCADE"
			IF rec2.od = 'c' THEN
				dcode := dcode || 'DELETE FROM "' || trec2.namespace || '"."' || trec2.tablename || '" WHERE ' || acode || ';';
			-- Handle "ON DELETE SET NULL"
			ELSIF rec2.od = 'n' THEN
				dcode := dcode || 'UPDATE "' || trec2.namespace || '"."' || trec2.tablename || '" SET ';
				FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
				LOOP
					dcode := dcode || '"' || sattr[i] || '"=NULL';
					IF i < array_upper(rec2.attr, 1)
						THEN dcode := dcode || ',';
					END IF;
				END LOOP;
				dcode := dcode || ' WHERE ' || acode || ';';
			-- Handle "ON DELETE NO ACTION"
			ELSIF rec2.od = 'a' THEN
				dcode := dcode || 'SELECT INTO c COUNT(*) FROM "' || trec2.namespace || '"."' || trec2.tablename || '" WHERE ' || acode
					|| '; IF c > 0 THEN RAISE EXCEPTION ''Foreign key on "' || trec2.namespace || '"."' || trec2.tablename
					|| '" failed while deleting from "' || trec.namespace || '"."' || trec.tablename || '"''; END IF;';
			END IF;

			-- Handle "ON UPDATE CASCADE"
			IF rec2.ou = 'c' THEN
				ucode := ucode || 'UPDATE "' || trec2.namespace || '"."' || trec2.tablename || '" SET ';
				FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
				LOOP
					ucode := ucode || '"' || sattr[i] || '"=NEW."' || dattr[i] || '"';
					IF i < array_upper(rec2.attr, 1)
						THEN ucode := ucode || ',';
					END IF;
				END LOOP;
				ucode := ucode || ' WHERE ' || acode || ';';
			-- Handle "ON UPDATE SET NULL"
			ELSIF rec2.ou = 'n' THEN
				ucode := ucode || 'UPDATE "' || trec2.namespace || '"."' || trec2.tablename || '" SET ';
				FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
				LOOP
					ucode := ucode || '"' || sattr[i] || '"=NULL';
					IF i < array_upper(rec2.attr, 1)
						THEN ucode := ucode || ',';
					END IF;
				END LOOP;
				ucode := ucode || ' WHERE ' || acode || ';';
			-- Handle "ON UPDATE NO ACTION"
			ELSIF rec2.ou = 'a' THEN
				ucode := ucode || 'SELECT INTO c COUNT(*) FROM "' || trec2.namespace || '"."' || trec2.tablename || '" WHERE ' || acode
					|| '; IF c > 0 THEN RAISE EXCEPTION ''Foreign key on "' || trec2.namespace || '"."' || trec2.tablename
					|| '" failed while updating from "' || trec.namespace || '"."' || trec.tablename || '"''; END IF;';
			END IF;
		END LOOP;
		ucode := ucode || 'END IF;';
	END LOOP;

	dcode := dcode || 'RETURN OLD;END;$a$ LANGUAGE plpgsql';
	ucode := ucode || 'RETURN NEW;END;$a$ LANGUAGE plpgsql';

	EXECUTE dcode;
	EXECUTE ucode;
END;
$$ LANGUAGE plpgsql;


--
-- Function that will generate the trigger functions to be applied
-- before insertion or update on referencing tables.
--
CREATE OR REPLACE FUNCTION inheritance.make_referencing_functions(tbl OID) RETURNS VOID AS $$
DECLARE
	icode	TEXT;
	ucode	TEXT;
	rec	RECORD;
	trec	RECORD;
	trec2	RECORD;
	sattr	NAME[];
	dattr	NAME[];
	i	INT;
BEGIN
	icode := 'CREATE FUNCTION inheritance.tgr_fk_' || tbl || '_ins() RETURNS TRIGGER AS $a$DECLARE c INT;BEGIN ';
	ucode := 'CREATE FUNCTION inheritance.tgr_fk_' || tbl || '_upd_s() RETURNS TRIGGER AS $a$DECLARE c INT;BEGIN ';

	SELECT INTO trec * FROM inheritance.get_table_name(tbl);

	-- For each set of referencing keys
	FOR rec IN SELECT fkc_from_attr AS attr, fkc_to_oid AS ttbl, fkc_to_attr AS tattr
		FROM inheritance.fk_cache WHERE fkc_from_oid = tbl
	LOOP
		sattr := inheritance.get_attr_names(tbl, rec.attr);

		-- Get referenced table and attribute
		SELECT INTO trec2 * FROM inheritance.get_table_name(rec.ttbl);
		dattr := inheritance.get_attr_names(rec.ttbl, rec.tattr);

		-- Create the code to be run on insertion
		icode := icode || 'IF';
		FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
		LOOP
			icode := icode || ' NEW."' || sattr[i] || '" IS NOT NULL';
			IF i <> array_upper(sattr, 1)
				THEN icode := icode || ' AND';
			END IF;
		END LOOP;
		icode := icode || ' THEN SELECT INTO c COUNT(*) FROM "' || trec2.namespace || '"."' || trec2.tablename || '" WHERE';
		FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
		LOOP
			icode := icode || ' NEW."' || sattr[i] || '"="' || dattr[i] || '"';
			IF i <> array_upper(sattr, 1)
				THEN icode := icode || ' AND';
			END IF;
		END LOOP;
		icode := icode || '; IF c=0 THEN RAISE EXCEPTION ''Foreign key to "' || trec2.namespace || '"."' || trec2.tablename
				|| '" failed while inserting into "' || trec.namespace || '"."' || trec.tablename
				|| '"''; END IF; END IF;';


		-- Create the code to be run on update
		ucode := ucode || 'IF (';
		FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
		LOOP
			ucode := ucode || 'NEW."' || sattr[i] || '"<>OLD."' || sattr[i] || '"';
			IF i <> array_upper(sattr, 1)
				THEN ucode := ucode || ' OR ';
			END IF;
		END LOOP;
		ucode := ucode || ') AND';
		FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
		LOOP
			ucode := ucode || ' NEW."' || sattr[i] || '" IS NOT NULL';
			IF i <> array_upper(sattr, 1)
				THEN ucode := ucode || ' AND';
			END IF;
		END LOOP;
		ucode := ucode || ' THEN SELECT INTO c COUNT(*) FROM "' || trec2.namespace || '"."' || trec2.tablename || '" WHERE';
		FOR i IN array_lower(sattr, 1) .. array_upper(sattr, 1)
		LOOP
			ucode := ucode || ' NEW."' || sattr[i] || '"="' || dattr[i] || '"';
			IF i <> array_upper(sattr, 1)
				THEN ucode := ucode || ' AND';
			END IF;
		END LOOP;
		ucode := ucode || '; IF c=0 THEN RAISE EXCEPTION ''Foreign key to "' || trec2.namespace || '"."' || trec2.tablename
				|| '" failed while updating "' || trec.namespace || '"."' || trec.tablename
				|| '"''; END IF; END IF;';
	END LOOP;

	icode := icode || 'RETURN NEW;END;$a$ LANGUAGE plpgsql';
	ucode := ucode || 'RETURN NEW;END;$a$ LANGUAGE plpgsql';

	EXECUTE icode;
	EXECUTE ucode;
END;
$$ LANGUAGE plpgsql;


--
-- Function that will generate foreign key triggers
--
CREATE OR REPLACE FUNCTION inheritance.make_fkey_triggers() RETURNS VOID AS $$
DECLARE
	rec	RECORD;
BEGIN
	-- Generate functions for all referencing tables
	FOR rec IN SELECT DISTINCT fkc_from_oid FROM inheritance.fk_cache
	LOOP
		PERFORM inheritance.make_referencing_functions(rec.fkc_from_oid);
		PERFORM inheritance.create_referencing_triggers(rec.fkc_from_oid);
	END LOOP;

	-- Generate functions and triggers for all referenced tables
	FOR rec IN SELECT DISTINCT fkc_to_oid FROM inheritance.fk_cache
	LOOP
		PERFORM inheritance.make_referenced_functions(rec.fkc_to_oid);
		PERFORM inheritance.create_referenced_triggers(rec.fkc_to_oid);
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that creates the foreign key triggers for referenced tables and their children
--
CREATE OR REPLACE FUNCTION inheritance.create_referenced_triggers(dst OID) RETURNS VOID AS $$
DECLARE
	rec	RECORD;
	trec	RECORD;
BEGIN
	-- Create triggers for the referenced table and its children
	FOR rec IN SELECT dst AS tid UNION SELECT tch_child AS tid FROM inheritance.tbl_cache WHERE tch_parent = dst
	LOOP
		SELECT INTO trec * FROM inheritance.get_table_name(rec.tid);

		EXECUTE 'CREATE TRIGGER tgr_fk_' || dst || '_del_' || rec.tid
			|| ' BEFORE DELETE ON "' || trec.namespace || '"."' || trec.tablename || '" '
			|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.tgr_fk_' || dst || '_del()';
		INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
			SELECT oid, rec.tid, FALSE FROM pg_trigger
				WHERE tgname = 'tgr_fk_' || dst || '_del_' || rec.tid
				  AND tgrelid = rec.tid;

		EXECUTE 'CREATE TRIGGER tgr_fk_' || dst || '_updd_' || rec.tid
			|| ' BEFORE UPDATE ON "' || trec.namespace || '"."' || trec.tablename || '" '
			|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.tgr_fk_' || dst || '_upd_d()';
		INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
			SELECT oid, rec.tid, FALSE FROM pg_trigger
				WHERE tgname = 'tgr_fk_' || dst || '_updd_' || rec.tid
				  AND tgrelid = rec.tid;
	END LOOP;
END;
$$ LANGUAGE plpgsql;


--
-- Function that creates the foreign key triggers for referencing tables and their children
--
CREATE OR REPLACE FUNCTION inheritance.create_referencing_triggers(src OID) RETURNS VOID AS $$
DECLARE
	rec	RECORD;
	trec	RECORD;
BEGIN
	-- Create triggers for the referencing table and its children
	FOR rec IN SELECT src AS tid UNION SELECT tch_child AS tid FROM inheritance.tbl_cache WHERE tch_parent = src
	LOOP
		SELECT INTO trec * FROM inheritance.get_table_name(rec.tid);

		EXECUTE 'CREATE TRIGGER tgr_fk_' || src || '_ins_' || rec.tid
			|| ' BEFORE INSERT ON "' || trec.namespace || '"."' || trec.tablename || '" '
			|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.tgr_fk_' || src || '_ins()';
		INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
			SELECT oid, rec.tid, FALSE FROM pg_trigger
				WHERE tgname = 'tgr_fk_' || src || '_ins_' || rec.tid
				  AND tgrelid = rec.tid;

		EXECUTE 'CREATE TRIGGER tgr_fk_' || src || '_upds_' || rec.tid
			|| ' BEFORE UPDATE ON "' || trec.namespace || '"."' || trec.tablename || '" '
			|| 'FOR EACH ROW EXECUTE PROCEDURE inheritance.tgr_fk_' || src || '_upd_s()';
		INSERT INTO inheritance.tgr_cache (tch_trigger, tch_table, tch_initial)
			SELECT oid, rec.tid, FALSE FROM pg_trigger
				WHERE tgname = 'tgr_fk_' || src || '_upds_' || rec.tid
				  AND tgrelid = rec.tid;
	END LOOP;
END;
$$ LANGUAGE plpgsql;

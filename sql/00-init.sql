-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 00-init.sql
--
-- Initialises the various roles and the database itself
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


-- Connect to the main system database
\c postgres postgres
SET search_path=public;

-- Create the LW users
CREATE ROLE legacyworlds WITH LOGIN ENCRYPTED PASSWORD 'main user password';
CREATE ROLE legacyworlds_admin WITH LOGIN ENCRYPTED PASSWORD 'administration user password' CONNECTION LIMIT 2;

-- Create the database
CREATE DATABASE legacyworlds WITH OWNER=legacyworlds_admin ENCODING='UTF8';



-- Connect to the LW database with the PostgreSQL admin user
\c legacyworlds postgres

-- Register PL/PgSQL's handler function
CREATE FUNCTION plpgsql_call_handler() RETURNS language_handler AS
	'$libdir/plpgsql' LANGUAGE C;

-- Register PL/PgSQL's validator function
CREATE FUNCTION plpgsql_validator(oid) RETURNS void AS
	'$libdir/plpgsql' LANGUAGE C;

-- Register PL/PgSQL
CREATE TRUSTED PROCEDURAL LANGUAGE plpgsql
	HANDLER plpgsql_call_handler
	VALIDATOR plpgsql_validator;



-- Connect to the LW database with the LW admin user
\c legacyworlds legacyworlds_admin


--
-- The following function returns the last inserted identifier for some table.
--
CREATE OR REPLACE FUNCTION last_inserted(tbl NAME) RETURNS BIGINT AS $$
DECLARE
	i BIGINT;
BEGIN
	SELECT INTO i currval(tbl || '_id_seq');
	RETURN i;
EXCEPTION
WHEN undefined_table OR object_not_in_prerequisite_state THEN
	RETURN NULL;
END;
$$ LANGUAGE plpgsql;

--
-- UNIX_TIMESPAMP() function for MySQL compatibility
--
CREATE OR REPLACE FUNCTION unix_timestamp(t TIMESTAMP WITH TIME ZONE) RETURNS INT AS $$
	SELECT CAST(EXTRACT(EPOCH FROM $1) AS INT);
$$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION unix_timestamp(t TIMESTAMP WITHOUT TIME ZONE) RETURNS INT AS $$
	SELECT CAST(EXTRACT(EPOCH FROM $1) AS INT);
$$ LANGUAGE SQL;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/01-planet.sql
--
-- Beta 5 games:
-- Planet data and associated tables (buildqueue, etc...)
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE planet (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	owner		BIGINT				REFERENCES player (id),
	system		INT		NOT NULL	REFERENCES system (id),
	orbit		SMALLINT	NOT NULL	CHECK(orbit BETWEEN 0 AND 5),
	name		VARCHAR(15)	NOT NULL	UNIQUE,
	status		SMALLINT	NOT NULL	DEFAULT 0 CHECK(status BETWEEN 0 AND 5),
	pop		INT		NOT NULL	DEFAULT 2000,
	max_pop		INT		NOT NULL,
	ifact		INT		NOT NULL	DEFAULT 3,
	mfact		INT		NOT NULL	DEFAULT 3,
	turrets		INT		NOT NULL	DEFAULT 0,
	renamed		INT		NOT NULL	DEFAULT 0,
	happiness	SMALLINT	NOT NULL	DEFAULT 70 CHECK(happiness BETWEEN 0 AND 100),
	beacon		SMALLINT	NOT NULL	DEFAULT 0,
	abandon		SMALLINT,
	bh_prep		SMALLINT,
	bh_unhappiness	SMALLINT	NOT NULL	DEFAULT 0,
	sale		SMALLINT,
	built_probe	BOOLEAN		NOT NULL	DEFAULT FALSE,
	probe_policy	CHAR(4),
	corruption	SMALLINT	NOT NULL	DEFAULT 0,
	vacation	CHAR(4)		NOT NULL	DEFAULT 'NO' CHECK(vacation IN ('NO','PEND','YES')),
	mod_check	BOOLEAN		NOT NULL	DEFAULT TRUE,
	force_rename	INT,
	UNIQUE (system, orbit)
);

CREATE INDEX planet_owner ON planet (owner);

GRANT SELECT,INSERT,UPDATE ON planet TO legacyworlds;
GRANT SELECT,UPDATE ON planet_id_seq TO legacyworlds;


CREATE TABLE planet_abandon_time (
	id		BIGINT				PRIMARY KEY REFERENCES planet (id),
	time_required	INT		NOT NULL	CHECK(time_required BETWEEN 6 AND 24)
);

CREATE INDEX planet_abandon_time_req ON planet_abandon_time (time_required);
GRANT SELECT,INSERT,DELETE,UPDATE ON planet_abandon_time TO legacyworlds;


CREATE TABLE planet_max_pop (
	planet		BIGINT		NOT NULL	REFERENCES planet(id),
	tech_level	SMALLINT	NOT NULL	CHECK(tech_level BETWEEN 0 AND 3),
	max_pop		INT 		NOT NULL,
	PRIMARY KEY (planet, tech_level)
);
GRANT SELECT,INSERT,DELETE ON planet_max_pop TO legacyworlds;



CREATE TABLE facthist (
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	moment		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	is_military	BOOLEAN		NOT NULL,
	change		INT		NOT NULL	CHECK(change <> 0)
);

CREATE INDEX facthist_idx ON facthist (planet, moment, is_military);
GRANT SELECT,INSERT,DELETE ON facthist TO legacyworlds;



CREATE TABLE turhist (
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	moment		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	change		INT		NOT NULL	CHECK(change <> 0)
);

CREATE INDEX turhist_idx ON turhist (planet, moment);
GRANT SELECT,INSERT,DELETE ON turhist TO legacyworlds;



CREATE TABLE buildqueue (
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	bq_order	INT		NOT NULL	CHECK(bq_order >= 0),
	item		SMALLINT	NOT NULL	CHECK(item BETWEEN 0 AND 5),
	quantity	INT		NOT NULL	CHECK(quantity > 0),
	workunits	BIGINT		NOT NULL	DEFAULT 0 CHECK(workunits >= 0),
	PRIMARY KEY (planet, bq_order)
);
GRANT SELECT,INSERT,UPDATE,DELETE ON buildqueue TO legacyworlds;



CREATE TABLE abandon_log (
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	abandon_time	INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	former_owner	BIGINT		NOT NULL	REFERENCES player (id),
	retake_time	INT,
	retake_owner	BIGINT				REFERENCES player (id),
	PRIMARY KEY (planet, abandon_time)
);

CREATE INDEX abandon_former ON abandon_log (former_owner);
CREATE INDEX abandon_retake ON abandon_log (retake_owner);

GRANT INSERT,UPDATE,SELECT ON abandon_log TO legacyworlds;



CREATE TABLE attacks (
	planet		BIGINT		NOT NULL	REFERENCES planet (id) PRIMARY KEY,
	ecm_level	SMALLINT	NOT NULL	CHECK(ecm_level BETWEEN 0 AND 4),
	eccm_level	SMALLINT	NOT NULL	CHECK(ecm_level BETWEEN 0 AND 4),
	friendly	INT		NOT NULL,
	enemy		INT		NOT NULL,
	v_players	BOOLEAN		NOT NULL,
	v_friendly	INT,
	v_enemy		INT
);
GRANT SELECT,INSERT,UPDATE,DELETE ON attacks TO legacyworlds;


--
-- Table to store planet names for new players
--

CREATE TABLE planet_reg_queue (
	account		BIGINT		NOT NULL	PRIMARY KEY REFERENCES main.reg_queue (account)
								ON DELETE CASCADE,
	p_name		VARCHAR(15)	NOT NULL	UNIQUE
);
GRANT SELECT,INSERT,DELETE ON planet_reg_queue TO legacyworlds;

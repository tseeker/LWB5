-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/10-prot-tables.sql
--
-- Beta 5 games:
-- Tables to support protection
--
-- Copyright(C) 2004-2008, DeepClone Development
-- --------------------------------------------------------


--
-- Enemies of the Peacekeepers
--
CREATE TABLE pk_enemy (
	player		BIGINT		NOT NULL	PRIMARY KEY REFERENCES player (id) ON DELETE CASCADE,
	until		INT		NOT NULL
);
CREATE INDEX pk_enemy_until ON pk_enemy (until);
GRANT SELECT,INSERT,DELETE,UPDATE ON pk_enemy TO legacyworlds;


--
-- Offenses against protected systems
--
CREATE TABLE pk_offenses (
	player		BIGINT		NOT NULL	PRIMARY KEY REFERENCES player (id) ON DELETE CASCADE,
	nb_offenses	INT		NOT NULL
);
CREATE INDEX pk_offenses_nb ON pk_offenses (nb_offenses);
GRANT SELECT,INSERT,DELETE,UPDATE ON pk_offenses TO legacyworlds;


--
-- System status
--
-- Status is one of:
--  A -> Allied player
--  W -> Warning sent
--  O -> Player is on the offensive
--  E -> Player has been declared an enemy
--
CREATE TABLE pk_sys_status (
	system		INT		NOT NULL	REFERENCES system (id),
	player		BIGINT		NOT NULL	REFERENCES player (id) ON DELETE CASCADE,
	status		CHAR(1)		NOT NULL	CHECK(status IN ('A', 'W', 'O', 'E')),
	switch_at	INT,
	PRIMARY KEY (system, player)
);
CREATE INDEX pk_sys_status_system ON pk_sys_status (system);
CREATE INDEX pk_sys_status_player ON pk_sys_status (player);
CREATE INDEX pk_sys_status_status ON pk_sys_status (status);
CREATE INDEX pk_sys_status_switch_at ON pk_sys_status (switch_at);
GRANT SELECT,INSERT,DELETE,UPDATE ON pk_sys_status TO legacyworlds;


--
-- End of protection messages
--
CREATE TABLE msg_endprotection (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	end_type	CHAR(3)		NOT NULL	CHECK (end_type IN ('BRK', 'ACT', 'EXP'))
);
GRANT INSERT,DELETE,SELECT ON msg_endprotection TO legacyworlds;

--
-- Warnings from the Peacekeepers
--
-- msg_type is one of:
-- 'W' -> warning, player must leave the planets
-- 'D' -> "you will be destroyed"
-- 'E' -> "you have been declared an enemy"
--
CREATE TABLE msg_pkwarning (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	msg_type	CHAR(1)		NOT NULL	CHECK(msg_type IN ('W', 'D', 'E')),
	delay		INT
);
GRANT SELECT,INSERT,DELETE ON msg_pkwarning TO legacyworlds;

CREATE TABLE pkwarning_planet (
	id		BIGINT		NOT NULL	REFERENCES msg_pkwarning (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	PRIMARY KEY (id, planet)
);
CREATE INDEX pkwarning_planet_id ON pkwarning_planet (planet);
GRANT SELECT,INSERT,DELETE ON pkwarning_planet TO legacyworlds;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/00-system.sql
--
-- Beta 5 games:
-- Table that stores systems
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE system (
	id		SERIAL				PRIMARY KEY,
	x		INT		NOT NULL,
	y		INT		NOT NULL,
	prot		INT		NOT NULL,
	assigned	BOOLEAN		NOT NULL	DEFAULT FALSE,
	nebula		SMALLINT	NOT NULL
);

CREATE UNIQUE INDEX system_coords ON system (x, y);
CREATE INDEX system_prot ON system (prot);

GRANT SELECT,INSERT,UPDATE ON system TO legacyworlds;
GRANT SELECT,UPDATE ON system_id_seq TO legacyworlds;

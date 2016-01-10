-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/00-gdata.sql
--
-- Beta 5 games:
-- Table that stores game data
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE gdata (
	id	VARCHAR(8)	PRIMARY KEY,
	value	VARCHAR(60)	NOT NULL
);

GRANT INSERT,SELECT,UPDATE ON gdata TO legacyworlds;

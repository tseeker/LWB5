-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 20-credits.sql
--
-- Initialises the tables that store credits
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE main.credits (
	account			BIGINT		NOT NULL	PRIMARY KEY REFERENCES main.account (id) ON DELETE CASCADE,
	resources_used		INT		NOT NULL	DEFAULT 0 CHECK(resources_used >= 0),
	credits_obtained	INT		NOT NULL	DEFAULT 9000
);

GRANT SELECT,INSERT,UPDATE ON TABLE main.credits TO legacyworlds;

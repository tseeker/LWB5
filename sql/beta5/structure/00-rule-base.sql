-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/00-rule-base.sql
--
-- Beta 5 games:
-- Base tables for the rule system
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------

CREATE TABLE rule_def (
	name	VARCHAR(32)	NOT NULL	PRIMARY KEY,
	value	INT		NOT NULL
);

GRANT SELECT ON rule_def TO legacyworlds;


CREATE TABLE rule_handler (
	rule	VARCHAR(32)	NOT NULL	PRIMARY KEY REFERENCES rule_def (name),
	handler	VARCHAR(32)	NOT NULL
);

GRANT SELECT ON rule_handler TO legacyworlds;

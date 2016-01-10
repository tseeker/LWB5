-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/01-research-base.sql
--
-- Beta 5 games:
-- Tables that store tech definitions and functions to
-- ease inserting the data
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------

-- Research definitions
CREATE TABLE research (
	id		INT		NOT NULL	PRIMARY KEY,
	points		INT		NOT NULL,
	cost		INT		NOT NULL,
	optional	SMALLINT	NOT NULL	CHECK(optional IN (0,1,2)),
	category	SMALLINT	NOT NULL	CHECK(category IN (0,1,2)),
	is_law		BOOLEAN		NOT NULL
);

GRANT SELECT ON research TO legacyworlds;


-- Dependencies
CREATE TABLE research_dep (
	research	INT		NOT NULL	REFERENCES research (id),
	depends_on	INT		NOT NULL	REFERENCES research (id),
	PRIMARY KEY (research, depends_on)
);

CREATE INDEX research_depends_on ON research_dep (depends_on);
GRANT SELECT ON research_dep TO legacyworlds;


-- Research names and descriptions
CREATE TABLE research_txt (
	research	INT		NOT NULL	REFERENCES research (id),
	lang		VARCHAR(4)	NOT NULL	REFERENCES main.lang (txt),
	name		VARCHAR(64)	NOT NULL,
	description	TEXT		NOT NULL,
	PRIMARY KEY (research, lang)
);

CREATE INDEX research_txt_lang ON research_txt (lang);
GRANT SELECT ON research_txt TO legacyworlds;


-- Research effects
CREATE TABLE research_effect (
	research	INT		NOT NULL	REFERENCES research (id),
	rule		VARCHAR(32)	NOT NULL	REFERENCES rule_def (name),
	modifier	INT		NOT NULL,
	PRIMARY KEY (research, rule)
);

CREATE INDEX research_fx_rule ON research_effect (rule);
GRANT SELECT ON research_effect TO legacyworlds;

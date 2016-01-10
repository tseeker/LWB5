-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 13-main-links.sql
--
-- Tables for the links to external sites
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin



-- 
-- Categories
--
CREATE TABLE main.lk_category (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	position	INT		NOT NULL	UNIQUE CHECK(position >= 0),
	title		VARCHAR(64)	NOT NULL	UNIQUE,
	description	TEXT
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.lk_category TO legacyworlds;
GRANT SELECT,UPDATE ON main.lk_category_id_seq TO legacyworlds;


--
-- Links
--
CREATE TABLE main.lk_link (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	category	BIGINT		NOT NULL	REFERENCES main.lk_category (id) ON DELETE CASCADE,
	title		VARCHAR(64)	NOT NULL,
	url		VARCHAR(128)	NOT NULL	UNIQUE,
	description	TEXT
);

CREATE UNIQUE INDEX lk_link_cat_title ON main.lk_link (category, title);
GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.lk_link TO legacyworlds;
GRANT SELECT,UPDATE ON main.lk_link_id_seq TO legacyworlds;


--
-- Reports of broken links
--
CREATE TABLE main.lk_broken (
	link		BIGINT		NOT NULL	REFERENCES main.lk_link (id) ON DELETE CASCADE,
	reported_by	BIGINT		NOT NULL	REFERENCES main.account (id),
	PRIMARY KEY (link, reported_by)
);
GRANT SELECT,INSERT,DELETE ON TABLE main.lk_broken TO legacyworlds;


--
-- Submitted links
--
CREATE TABLE main.lk_submitted (
	url		VARCHAR(128)	NOT NULL,
	submitted_by	BIGINT		NOT NULL	REFERENCES main.account (id),
	title		VARCHAR(64)	NOT NULL,
	description	TEXT,
	PRIMARY KEY (url, submitted_by)
);
GRANT SELECT,INSERT,DELETE ON TABLE main.lk_submitted TO legacyworlds;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 13-main-manual.sql
--
-- Tables for the manual and its index
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin



-- 
-- Manual sections
--
CREATE TABLE main.man_section (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	version		VARCHAR(16)	NOT NULL,
	lang		VARCHAR(4)	NOT NULL	REFERENCES main.lang (txt),
	in_section	BIGINT				REFERENCES main.man_section (id) ON DELETE SET NULL,
	after_section	BIGINT				REFERENCES main.man_section (id) ON DELETE SET NULL,
	link_to		BIGINT				REFERENCES main.man_section (id) ON DELETE SET NULL,
	name		VARCHAR(64)	NOT NULL,
	last_update	INT		NOT NULL,
	is_page		BOOLEAN		NOT NULL,
	in_menu		BOOLEAN		NOT NULL,
	title		VARCHAR(128)	NOT NULL,
	contents	TEXT		NOT NULL
);

CREATE UNIQUE INDEX man_section_unique ON main.man_section (version, lang, name);
CREATE INDEX man_section_in_section ON main.man_section (in_section);
CREATE INDEX man_section_after_section ON main.man_section (after_section);
CREATE INDEX man_section_link_to ON main.man_section (link_to);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.man_section TO legacyworlds;
GRANT SELECT,UPDATE ON main.man_section_id_seq TO legacyworlds;


--
-- Manual index
--
CREATE TABLE main.man_index (
	word	VARCHAR(32)	NOT NULL,
	wcount	INT		NOT NULL	CHECK(wcount > 0),
	lang	VARCHAR(4)	NOT NULL	REFERENCES main.lang (txt),
	section BIGINT		NOT NULL	REFERENCES main.man_section (id) ON DELETE CASCADE,
	PRIMARY KEY (word, lang, section)
);
GRANT SELECT,INSERT,DELETE ON TABLE main.man_index TO legacyworlds;


--
-- Banned words
--
CREATE TABLE main.man_index_ban (
	lang	VARCHAR(4)	NOT NULL	REFERENCES main.lang (txt),
	word	VARCHAR(32)	NOT NULL,
	PRIMARY KEY (lang,word)
);
GRANT SELECT ON TABLE main.man_index_ban TO legacyworlds;

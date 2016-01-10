-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/01-message-base.sql
--
-- Beta 5 games:
-- The base tables for the PM system: messages and custom
-- folders
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE custom_folder (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	name		VARCHAR(32)	NOT NULL,
	UNIQUE (player, name)
);

GRANT INSERT,SELECT,UPDATE,DELETE ON custom_folder TO legacyworlds;
GRANT SELECT,UPDATE ON custom_folder_id_seq TO legacyworlds;



CREATE TABLE message (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	sent_on		INT		NOT NULL,
	mtype		VARCHAR(16)	NOT NULL,
	ftype		CHAR(3)		NOT NULL	CHECK(ftype IN ('IN','INT','OUT','CUS')),
	fcustom		BIGINT				REFERENCES custom_folder (id) ON DELETE SET NULL,
	is_new		BOOLEAN		NOT NULL	DEFAULT TRUE,
	deleted		BOOLEAN		NOT NULL	DEFAULT FALSE,
	original	BIGINT				REFERENCES message (id),
	reply_to	BIGINT				REFERENCES message (id)
);

CREATE INDEX message_player ON message (player);
CREATE INDEX message_fcustom ON message (fcustom);
CREATE INDEX message_reply_to ON message (reply_to);
CREATE INDEX message_original ON message (original);

GRANT INSERT,SELECT,UPDATE ON message TO legacyworlds;
GRANT INSERT,SELECT,UPDATE ON message_id_seq TO legacyworlds;

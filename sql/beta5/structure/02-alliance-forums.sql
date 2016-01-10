-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/02-alliance-forums.sql
--
-- Beta 5 games:
-- Tables for alliance forums
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE af_forum (
	id		SERIAL					PRIMARY KEY,
	alliance	INT		NOT NULL		REFERENCES alliance(id) ON DELETE CASCADE,
	forder		INT		NOT NULL		CHECK(forder >= 0),
	title		VARCHAR(64)	NOT NULL,
	description	TEXT,
	topics		INT		NOT NULL DEFAULT 0	CHECK(topics >= 0),
	posts		INT		NOT NULL DEFAULT 0	CHECK(posts >= 0),
	last_post	BIGINT,
	user_post	BOOLEAN		NOT NULL DEFAULT TRUE,
	UNIQUE(alliance, forder),
	UNIQUE(alliance, title)
);

CREATE INDEX af_forum_last_post ON af_forum (last_post);

GRANT SELECT,INSERT,UPDATE,DELETE ON af_forum TO legacyworlds;
GRANT SELECT,UPDATE ON af_forum_id_seq TO legacyworlds;



CREATE TABLE af_topic (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	forum		INT		NOT NULL	REFERENCES af_forum (id) ON DELETE CASCADE,
	first_post	BIGINT		NOT NULL,
	last_post	BIGINT,
	sticky		BOOLEAN		NOT NULL	DEFAULT FALSE
);

CREATE INDEX af_topic_forum ON af_topic (forum);
CREATE INDEX af_topic_first_post ON af_topic (first_post);
CREATE INDEX af_topic_last_post ON af_topic (last_post);

GRANT SELECT,INSERT,UPDATE,DELETE ON af_topic TO legacyworlds;
GRANT SELECT,UPDATE ON af_topic_id_seq TO legacyworlds;



CREATE TABLE af_post (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	forum		INT		NOT NULL	REFERENCES af_forum (id) ON DELETE CASCADE,
	topic		BIGINT				REFERENCES af_topic (id) ON DELETE CASCADE,
	author		BIGINT		NOT NULL	REFERENCES player (id),
	reply_to	BIGINT				REFERENCES af_post (id) ON DELETE NO ACTION,
	moment		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	title		VARCHAR(100)	NOT NULL,
	contents	TEXT		NOT NULL,
	enable_code	BOOLEAN		NOT NULL	DEFAULT TRUE,
	enable_smileys	BOOLEAN		NOT NULL	DEFAULT TRUE,
	edited		INT,
	edited_by	BIGINT				REFERENCES player(id)
);

CREATE INDEX af_post_forum ON af_post (forum);
CREATE INDEX af_post_topic ON af_post (topic);
CREATE INDEX af_post_author ON af_post (author);
CREATE INDEX af_post_reply_to ON af_post (reply_to);
CREATE INDEX af_post_edited_by ON af_post (edited_by);

ALTER TABLE af_forum ADD FOREIGN KEY (last_post) REFERENCES af_post (id) ON DELETE SET NULL;
ALTER TABLE af_topic ADD FOREIGN KEY (first_post) REFERENCES af_post (id) ON DELETE CASCADE;
ALTER TABLE af_topic ADD FOREIGN KEY (last_post) REFERENCES af_post (id) ON DELETE SET NULL;

GRANT SELECT,INSERT,UPDATE,DELETE ON af_post TO legacyworlds;
GRANT SELECT,UPDATE ON af_post_id_seq TO legacyworlds;



CREATE TABLE af_read (
	reader		BIGINT		NOT NULL	REFERENCES player (id),
	topic		BIGINT		NOT NULL	REFERENCES af_topic (id) ON DELETE CASCADE,
	PRIMARY KEY (reader, topic)
);

CREATE INDEX af_read_topic ON af_read (topic);
GRANT SELECT,INSERT,DELETE ON af_read TO legacyworlds;



CREATE TABLE algr_forums (
	grade		BIGINT		NOT NULL	REFERENCES alliance_grade (id) ON DELETE CASCADE,
	forum		INT		NOT NULL	REFERENCES af_forum (id) ON DELETE CASCADE,
	is_mod		BOOLEAN		NOT NULL	DEFAULT FALSE,
	PRIMARY KEY (grade, forum)
);

CREATE INDEX algr_forums_forum ON algr_forums (forum);
GRANT SELECT,INSERT,DELETE,UPDATE ON algr_forums TO legacyworlds;

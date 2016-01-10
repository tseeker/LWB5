-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 13-main-forums.sql
--
-- Tables for the forums
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin



-- 
-- Forum categories
--
CREATE TABLE main.f_category (
	id		SERIAL		NOT NULL		PRIMARY KEY,
	corder		INT		NOT NULL		UNIQUE CHECK(corder >= 0),
	title		VARCHAR(64)	NOT NULL		UNIQUE,
	description	TEXT
);

GRANT SELECT,INSERT ON TABLE main.f_category TO legacyworlds;
GRANT SELECT,UPDATE ON main.f_category_id_seq TO legacyworlds;


--
-- Forums
--
CREATE TABLE main.f_forum (
	id		SERIAL		NOT NULL		PRIMARY KEY,
	category	INT		NOT NULL		REFERENCES main.f_category (id) ON DELETE CASCADE,
	forder		INT		NOT NULL DEFAULT 0	CHECK(forder >= 0),
	title		VARCHAR(64)	NOT NULL,
	description	TEXT,
	topics		INT		NOT NULL		CHECK(topics >= 0),
	posts		INT		NOT NULL		CHECK(posts >= 0),
	last_post	BIGINT		NULL,
	user_post	BOOLEAN		NOT NULL DEFAULT TRUE,
	admin_only	BOOLEAN		NOT NULL DEFAULT FALSE
);

CREATE UNIQUE INDEX forum_unique ON main.f_forum (category, forder);
CREATE INDEX forum_last_post ON main.f_forum (last_post);

GRANT SELECT,UPDATE,INSERT ON TABLE main.f_forum TO legacyworlds;
GRANT SELECT,UPDATE ON main.f_forum_id_seq TO legacyworlds;


--
-- Topics
--
CREATE TABLE main.f_topic (
	id		BIGSERIAL	NOT NULL		PRIMARY KEY,
	forum		INT		NOT NULL		REFERENCES main.f_forum (id) ON DELETE CASCADE,
	first_post	BIGINT		NOT NULL,
	last_post	BIGINT		NULL,
	sticky		BOOLEAN		NOT NULL DEFAULT FALSE,
	deleted		INT		NULL
);

CREATE INDEX topic_forum ON main.f_topic (forum);
CREATE INDEX topic_fpost ON main.f_topic (first_post);
CREATE INDEX topic_lpost ON main.f_topic (last_post);

GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE main.f_topic TO legacyworlds;
GRANT SELECT,UPDATE ON main.f_topic_id_seq TO legacyworlds;


--
-- Posts
--
CREATE TABLE main.f_post (
	id		BIGSERIAL			PRIMARY KEY,
	forum		INT		NOT NULL	REFERENCES main.f_forum (id) ON DELETE CASCADE,
	topic		BIGINT		NULL		REFERENCES main.f_topic (id) ON DELETE CASCADE,
	author		BIGINT		NOT NULL	REFERENCES main.account (id),
	reply_to	BIGINT		NULL		REFERENCES main.f_post (id) ON DELETE SET NULL,
	moment		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	title		VARCHAR(100)	NOT NULL,
	contents	TEXT		NOT NULL,
	enable_code	BOOLEAN		NOT NULL	DEFAULT TRUE,
	enable_smileys	BOOLEAN		NOT NULL	DEFAULT TRUE,
	edited		INT		NULL,
	edited_by	BIGINT		NULL		REFERENCES main.account (id),
	deleted		INT		NULL
);

CREATE INDEX post_forum ON main.f_post (forum);
CREATE INDEX post_topic ON main.f_post (topic);
CREATE INDEX post_author ON main.f_post (author);
CREATE INDEX post_editor ON main.f_post (edited_by);
CREATE INDEX post_reply ON main.f_post (reply_to);

ALTER TABLE main.f_forum ADD FOREIGN KEY (last_post) REFERENCES main.f_post (id) ON DELETE SET NULL;
ALTER TABLE main.f_topic ADD FOREIGN KEY (first_post) REFERENCES main.f_post (id) ON DELETE CASCADE;
ALTER TABLE main.f_topic ADD FOREIGN KEY (last_post) REFERENCES main.f_post (id) ON DELETE SET NULL;

GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE main.f_post TO legacyworlds;
GRANT SELECT,UPDATE ON main.f_post_id_seq TO legacyworlds;


-- 
-- Read topics
-- 
CREATE TABLE main.f_read (
	reader	BIGINT	NOT NULL	REFERENCES main.account (id),
	topic	BIGINT	NOT NULL	REFERENCES main.f_topic (id) ON DELETE CASCADE,
	PRIMARY KEY (reader, topic)
);

GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE main.f_read TO legacyworlds;


--
-- Smileys and forum codes
--
CREATE TABLE main.f_smiley (
	smiley	VARCHAR(20) NOT NULL PRIMARY KEY,
	file	VARCHAR(20) NOT NULL
);
CREATE TABLE main.f_code (
	p_reg_exp	VARCHAR(40) NOT NULL PRIMARY KEY,
	replacement	VARCHAR(80) NOT NULL
);
GRANT SELECT ON main.f_smiley TO legacyworlds;
GRANT SELECT ON main.f_code TO legacyworlds;


--
-- Admins, mods, losers
-- Not everything is useful in the current version so meh.
--
CREATE TABLE main.f_admin (
	"user"		BIGINT	NOT NULL	REFERENCES main.account (id) PRIMARY KEY,
	category	INT	NULL		REFERENCES main.f_category (id) ON DELETE CASCADE
);
GRANT SELECT,INSERT,UPDATE,DELETE ON main.f_admin TO legacyworlds;

CREATE TABLE main.f_moderator (
	"user"		BIGINT	NOT NULL	REFERENCES main.account (id) PRIMARY KEY,
	forum		INT	NULL		REFERENCES main.f_forum (id) ON DELETE CASCADE
);
GRANT SELECT,INSERT,UPDATE,DELETE ON main.f_moderator TO legacyworlds;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/07-message-admin.sql
--
-- Beta 5 games:
-- Tables required to send "administrative spam" to all
-- players.
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


--
-- Contents table
--

CREATE TABLE admin_spam (
	id		SERIAL				PRIMARY KEY,
	sent_by		BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	subject		VARCHAR(64)	NOT NULL,
	contents	TEXT		NOT NULL
);

CREATE INDEX admin_spam_sender ON admin_spam (sent_by);

GRANT SELECT,INSERT ON admin_spam TO legacyworlds;
GRANT SELECT,UPDATE ON admin_spam_id_seq TO legacyworlds;


--
-- Message table
--

CREATE TABLE msg_admin (
	id		BIGINT				PRIMARY KEY REFERENCES message(id) ON DELETE CASCADE,
	spam		INT		NOT NULL	REFERENCES admin_spam (id) ON DELETE CASCADE
);

CREATE INDEX msg_admin_spam ON msg_admin (spam);
GRANT INSERT,DELETE,SELECT ON msg_admin TO legacyworlds;

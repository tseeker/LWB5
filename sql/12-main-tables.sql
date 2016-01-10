-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 12-main-tables.sql
--
-- Initialises the part of the database that contains
-- the user accounts, logs ...
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin



-- 
-- Create the account table
--

CREATE TABLE main.account (
	id		BIGSERIAL				PRIMARY KEY,
	name		VARCHAR(15)	NOT NULL		UNIQUE,
	email		VARCHAR(128)	NOT NULL		UNIQUE,
	password	VARCHAR(64)	NOT NULL,
	status		VARCHAR(6)	NOT NULL DEFAULT 'NEW'	REFERENCES main.acnt_status (txt),
	conf_code	VARCHAR(16)	NULL,
	reason		TEXT		NULL,
	vac_credits	INT		NOT NULL DEFAULT 30	CHECK(vac_credits BETWEEN 0 AND 240),
	vac_start	INT		NULL			CHECK(vac_start IS NULL OR (vac_start IS NOT NULL AND vac_credits > 0)),
	quit_ts		INT		NULL			CHECK(quit_ts IS NULL OR quit_ts > 0),
	last_login	INT		NULL			CHECK(last_login IS NULL OR last_login > 0),
	last_logout	INT		NULL			CHECK(last_logout IS NULL OR last_logout > 0),
	pw_conf		VARCHAR(16)	NULL,
	admin		BOOLEAN		NOT NULL DEFAULT FALSE
);

CREATE INDEX account_status ON main.account(status);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.account TO legacyworlds;
GRANT SELECT,UPDATE ON main.account_id_seq TO legacyworlds;


--
-- The following table stores queued game registrations.
--

CREATE TABLE main.reg_queue (
	account		BIGINT		NOT NULL		PRIMARY KEY REFERENCES main.account (id)
									ON DELETE CASCADE,
	game		VARCHAR(16)	NOT NULL
);
GRANT SELECT,INSERT,DELETE ON main.reg_queue TO legacyworlds;


-- 
-- Create the table that stores kick requests
-- 

CREATE TABLE main.adm_kick (
	id		BIGSERIAL				PRIMARY KEY,
	to_kick		BIGINT		NOT NULL		REFERENCES main.account (id),
	requested_by	BIGINT		NOT NULL		REFERENCES main.account (id),
	requested_at	INT		NOT NULL		DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	reason		TEXT		NOT NULL,
	status		CHAR(1)		NOT NULL		CHECK(status IN ('P','Y','N')),
	examined_by	BIGINT					REFERENCES main.account (id)
);

CREATE INDEX adm_kick_to_kick ON main.adm_kick (to_kick);
CREATE INDEX adm_kick_req_by ON main.adm_kick (requested_by);
CREATE INDEX adm_kick_exam_by ON main.adm_kick (examined_by);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.adm_kick TO legacyworlds;
GRANT SELECT,UPDATE ON main.adm_kick_id_seq TO legacyworlds;


--
-- Create the table to handle tracking cookies
--

CREATE TABLE main.web_tracking (
	id		BIGSERIAL			PRIMARY KEY,
	cookie		VARCHAR(32)	NOT NULL	UNIQUE,
	created		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	last_used	INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	ip_addr		VARCHAR(15)	NOT NULL,
	browser		VARCHAR(255)	NOT NULL,
	stored_data	TEXT		NOT NULL	DEFAULT 'a:0:{}'
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.web_tracking TO legacyworlds;
GRANT SELECT,UPDATE ON main.web_tracking_id_seq TO legacyworlds;


--
-- Create the session storage table
--

CREATE TABLE main.web_session (
	id		BIGSERIAL			PRIMARY KEY,
	cookie		VARCHAR(32)	NOT NULL	UNIQUE,
	created		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	last_used	INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	ip_addr		VARCHAR(15)	NOT NULL,
	account		BIGINT				REFERENCES main.account(id),
	stored_data	TEXT		NOT NULL	DEFAULT 'a:0:{}',
	tracking	BIGINT		NOT NULL	REFERENCES main.web_tracking(id) ON DELETE CASCADE
);

CREATE INDEX web_session_tracking ON main.web_session (tracking);
CREATE INDEX web_session_account ON main.web_session (account);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.web_session TO legacyworlds;
GRANT SELECT,UPDATE ON main.web_session_id_seq TO legacyworlds;



--
-- Create the account log table
--
CREATE TABLE main.account_log (
	id		BIGSERIAL			PRIMARY KEY,
	account		BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	tracking	BIGINT		NULL		REFERENCES main.web_tracking (id) ON DELETE SET NULL,
	ip_addr		VARCHAR(18)	NOT NULL	DEFAULT 'AUTO',
	action		VARCHAR(6)	NOT NULL	REFERENCES main.acnt_log_entry_type (txt),
	t		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW()))
);

CREATE INDEX acnt_log_account ON main.account_log (account);
CREATE INDEX acnt_log_trk ON main.account_log (tracking);
CREATE INDEX acnt_log_act ON main.account_log (action);

GRANT SELECT,INSERT,DELETE ON TABLE main.account_log TO legacyworlds;
GRANT SELECT,UPDATE ON main.account_log_id_seq TO legacyworlds;


-- 
-- Create the table that stores cache references
-- 

CREATE TABLE main.web_cache (
	id		BIGSERIAL			PRIMARY KEY,
	rtype		VARCHAR(5)	NOT NULL,
	md5		VARCHAR(32)	NOT NULL,
	last_used	BIGINT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	UNIQUE (rtype, md5)
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.web_cache TO legacyworlds;
GRANT SELECT,UPDATE ON main.web_cache_id_seq TO legacyworlds;


-- 
-- Tables that will store rankings definitions, texts, and
-- game relations
-- 

-- Definitions
CREATE TABLE main.ranking_def (
	id	BIGSERIAL			PRIMARY KEY,
	version	VARCHAR(16)	NOT NULL,
	name	VARCHAR(16)	NOT NULL,
	more	BOOLEAN		NOT NULL	DEFAULT FALSE,
	UNIQUE (version, name)
);

GRANT SELECT ON TABLE main.ranking_def TO legacyworlds;

-- Descriptions
CREATE TABLE main.ranking_text (
	ranking		BIGINT		NOT NULL		REFERENCES main.ranking_def (id),
	lang		VARCHAR(4)	NOT NULL		REFERENCES main.lang (txt),
	name		VARCHAR(32)	NOT NULL,
	description	text		NOT NULL,
	PRIMARY KEY (ranking, lang)
);

CREATE INDEX rk_txt_lang ON main.ranking_text (lang);
GRANT SELECT ON TABLE main.ranking_text TO legacyworlds;


-- Games
CREATE TABLE main.ranking_game (
	id		BIGSERIAL	NOT NULL		PRIMARY KEY,
	ranking		BIGINT		NOT NULL		REFERENCES main.ranking_def (id),
	game		VARCHAR(16)	NOT NULL,
	UNIQUE (ranking, game)
);

GRANT SELECT ON TABLE main.ranking_game TO legacyworlds;


-- 
-- Table that will store the rankings themselves
-- 
CREATE TABLE main.ranking (
	r_type		BIGINT		NOT NULL		REFERENCES main.ranking_game (id) ON DELETE CASCADE,
	id		VARCHAR(32)	NOT NULL,
	additional	TEXT,
	points		BIGINT		NOT NULL		DEFAULT 0,
	ranking		BIGINT		NOT NULL,
	PRIMARY KEY (r_type, id)
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.ranking TO legacyworlds;


--
-- Table for user preferences
--
CREATE TABLE main.user_preferences (
	id	VARCHAR(32)		NOT NULL,
	version	VARCHAR(16)		NOT NULL,
	account BIGINT			NULL			REFERENCES main.account (id) ON DELETE CASCADE,
	value	VARCHAR(255)		NOT NULL,
	UNIQUE (id, version, account)
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.user_preferences TO legacyworlds;

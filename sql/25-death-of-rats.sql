-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 25-death-of-rats.sql
--
-- Tables for the AMS
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



--
-- Execution logs
--
CREATE TABLE main.dor_exec (
	ts		INT		NOT NULL	PRIMARY KEY,
	entries		INT		NOT NULL	CHECK(entries >= 0),
	events		INT		NOT NULL	CHECK(events >= 0)
);
GRANT SELECT,INSERT,DELETE ON main.dor_exec TO legacyworlds;


--
-- Single player log
--
CREATE TABLE main.dor_single(
	message		VARCHAR(10)	NOT NULL,
	account		BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	ts		INT		NOT NULL
);

CREATE INDEX dor_single_message ON main.dor_single (message);
CREATE INDEX dor_single_account ON main.dor_single (account);
CREATE INDEX dor_single_ts ON main.dor_single (ts);

GRANT SELECT, INSERT, DELETE ON main.dor_single TO legacyworlds;


--
-- Multiplayer log
--
CREATE TABLE main.dor_multi(
	message		VARCHAR(10)	NOT NULL,
	account1	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	account2	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	ts		INT		NOT NULL
);

CREATE INDEX dor_multi_message ON main.dor_multi (message);
CREATE INDEX dor_multi_account1 ON main.dor_multi (account1);
CREATE INDEX dor_multi_account2 ON main.dor_multi (account2);
CREATE INDEX dor_multi_ts ON main.dor_multi (ts);

GRANT SELECT, INSERT, DELETE ON main.dor_multi TO legacyworlds;


--
-- Log of people who try to connect with banned accounts
--
CREATE TABLE main.banned_attempt (
	ip_addr		VARCHAR(15)	NOT NULL,
	ts		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW())
);
CREATE INDEX banned_attempt_ip_addr ON main.banned_attempt (ip_addr);
CREATE INDEX banned_attempt_ts ON main.banned_attempt (ts);
GRANT SELECT, INSERT, DELETE ON main.banned_attempt TO legacyworlds;


--
-- Log of password changes
--
CREATE TABLE main.pass_change (
	account		BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	ts		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	old_pass	VARCHAR(64)	NOT NULL,
	new_pass	VARCHAR(64)	NOT NULL
);
CREATE INDEX pass_change_account ON main.pass_change (account);
CREATE INDEX pass_change_ts ON main.pass_change (ts);
GRANT SELECT, INSERT, DELETE ON main.pass_change TO legacyworlds;


--
-- Single player "badness points"
--
CREATE TABLE main.dor_single_points (
	account		BIGINT		NOT NULL	PRIMARY KEY REFERENCES main.account (id) ON DELETE CASCADE,
	points		INT		NOT NULL	CHECK(points > 0)
);
GRANT SELECT, INSERT, DELETE ON main.dor_single_points TO legacyworlds;

--
-- Multiplayer "badness points"
--
CREATE TABLE main.dor_multi_points (
	account1	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	account2	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	points		INT		NOT NULL	CHECK(points > 0),
	PRIMARY KEY (account1, account2)
);
GRANT SELECT, INSERT, DELETE ON main.dor_multi_points TO legacyworlds;


--
-- Punishments
--
CREATE TABLE main.dor_punishment (
	account		BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	other_account	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	ts		INT		NOT NULL,
	PRIMARY KEY(account,ts)
);
CREATE INDEX dor_punishment_ts ON main.dor_punishment (ts);
CREATE INDEX dor_punishment_oaccount ON main.dor_punishment (other_account);
GRANT SELECT, INSERT, DELETE ON main.dor_punishment TO legacyworlds;


--
-- Warnings
--
CREATE TABLE main.dor_warning (
	account1	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	account2	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	ts		INT		NOT NULL,
	PRIMARY KEY(account1, account2)
);
CREATE INDEX dor_warning_ts ON main.dor_warning (ts);
GRANT SELECT, INSERT, DELETE ON main.dor_warning TO legacyworlds;


--
-- In-game checks
--
CREATE TABLE main.dor_ingame_check (
	account1	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	account2	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	message		VARCHAR(16)	NOT NULL,
	ts		INT		NOT NULL,
	game		VARCHAR(10)	NOT NULL
);

CREATE INDEX dor_ingame_check_ac1 ON main.dor_ingame_check (account1);
CREATE INDEX dor_ingame_check_ac2 ON main.dor_ingame_check (account2);
CREATE INDEX dor_ingame_check_game ON main.dor_ingame_check (game);
CREATE INDEX dor_ingame_check_message ON main.dor_ingame_check (message);
CREATE INDEX dor_ingame_check_ts ON main.dor_ingame_check (ts);

GRANT SELECT, INSERT, DELETE ON main.dor_ingame_check TO legacyworlds;


--
-- Final badness points
--
CREATE TABLE main.dor_final_points (
	account1	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	account2	BIGINT		NOT NULL	REFERENCES main.account (id) ON DELETE CASCADE,
	points		INT		NOT NULL	CHECK(points > 0),
	PRIMARY KEY(account1, account2)
);
GRANT SELECT, INSERT, DELETE ON main.dor_final_points TO legacyworlds;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 11-main-enums.sql
--
-- Initialises the tables to be used as enumerations for
-- the main schema
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin


--
-- Create the account status table
--

CREATE TABLE main.acnt_status (
	txt	VARCHAR(6)	PRIMARY KEY
);
GRANT SELECT ON TABLE main.acnt_status TO legacyworlds;

-- Fill the account status table
COPY main.acnt_status FROM STDIN;
NEW
STD
KICKED
QUIT
INAC
VAC
\.


--
-- Create the account log entry types list
--

CREATE TABLE main.acnt_log_entry_type (
	txt	VARCHAR(6)	PRIMARY KEY
);
GRANT SELECT ON TABLE main.acnt_log_entry_type TO legacyworlds;

-- Fill the account log entry type table
COPY main.acnt_log_entry_type FROM STDIN;
IN
OUT
CREATE
CONF
QUIT
VSTART
VEND
\.


--
-- Create the supported language list
--

CREATE TABLE main.lang (
	txt	VARCHAR(4)	PRIMARY KEY
);
GRANT SELECT ON TABLE main.lang TO legacyworlds;

-- Fill the supported language table
COPY main.lang FROM STDIN;
en
\.

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 13-main-proxy.sql
--
-- Table for open proxy detector's cache
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin


--
-- Cache table
--
CREATE TABLE main.proxy_detector (
	host		VARCHAR(15)			PRIMARY KEY,
	last_check	INT		NOT NULL,
	is_proxy	BOOLEAN		NOT NULL
);
GRANT SELECT,INSERT,DELETE ON main.proxy_detector TO legacyworlds;

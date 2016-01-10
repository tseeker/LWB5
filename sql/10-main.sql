-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 10-main.sql
--
-- Initialises the main schema
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin

-- Create the main schema
CREATE SCHEMA main;
GRANT USAGE ON SCHEMA main TO legacyworlds;


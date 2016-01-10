-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/finalise.sql
--
-- Finalise a Beta 5 game database by revoking extra
-- privileges from the user role and dropping helper
-- functions
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------

-- Revoke user privileges
REVOKE INSERT ON gdata FROM legacyworlds;


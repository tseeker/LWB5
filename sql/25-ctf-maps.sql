-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 25-ctf-maps.sql
--
-- Create the tables for predefined maps.
--
-- Copyright(C) 2004-2008, DeepClone Development
-- --------------------------------------------------------


-- Connect to the database in ADMIN mode
\c legacyworlds legacyworlds_admin


--
-- Create the definition table
--

CREATE TABLE main.ctf_map_def (
	id		SERIAL					PRIMARY KEY,
	name		VARCHAR(32)	NOT NULL		UNIQUE,
	description	TEXT,
	alliances	INT		NOT NULL		CHECK(alliances > 1),
	width		INT		NOT NULL		CHECK(width > 1),
	height		INT		NOT NULL		CHECK(height > 1)
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.ctf_map_def TO legacyworlds;
GRANT SELECT,UPDATE ON main.ctf_map_def_id_seq TO legacyworlds;


--
-- Create the map table
--
-- sys_type is either 'S' for normal systems or '1' .. '4' for nebulae
-- alloc_for is either 0 (target) or a number that corresponds to an alliance
--

CREATE TABLE main.ctf_map_layout (
	map		INT		NOT NULL		REFERENCES main.ctf_map_def (id) ON DELETE CASCADE,
	sys_x		INT		NOT NULL,
	sys_y		INT		NOT NULL,
	sys_type	CHAR(1)		NOT NULL		CHECK(sys_type IN ('S', '1', '2', '3', '4')),
	alloc_for	INT,
	spawn_here	BOOLEAN,

	CHECK( (sys_type = 'S' AND alloc_for IS NOT NULL)
	    OR (sys_type <> 'S' AND alloc_for IS NULL) ),

	CHECK( ((alloc_for IS NULL OR alloc_for = 0) AND spawn_here IS NULL)
	    OR (alloc_for > 0 AND spawn_here IS NOT NULL) ),

	PRIMARY KEY(map, sys_x, sys_y)
);

GRANT SELECT,INSERT,UPDATE,DELETE ON TABLE main.ctf_map_layout TO legacyworlds;

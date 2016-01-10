-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 50-beta6-planet-pictures.sql
--
-- Tables that store votes about Beta 6's planet pictures
--
-- Copyright(C) 2004-2008, DeepClone Development
-- --------------------------------------------------------


-- Connect to the database in ADMIN mode
\c legacyworlds legacyworlds_admin


CREATE TABLE main.b6_planet_pics (
	id		SERIAL					PRIMARY KEY,
	p_size		INT		NOT NULL		CHECK(p_size BETWEEN 1 AND 10),
	p_type		INT		NOT NULL		CHECK(p_type BETWEEN 0 AND 4)
);

GRANT SELECT,INSERT ON TABLE main.b6_planet_pics TO legacyworlds;
GRANT SELECT,UPDATE ON main.b6_planet_pics_id_seq TO legacyworlds;


CREATE TABLE main.b6_planet_votes (
	account		BIGINT		NOT NULL		REFERENCES main.account (id),
	picture		INT		NOT NULL		REFERENCES main.b6_planet_pics (id),
	vote		INT		NOT NULL		CHECK(vote BETWEEN 1 AND 5),
	PRIMARY KEY(account, picture)
);

GRANT SELECT,INSERT ON TABLE main.b6_planet_votes TO legacyworlds;

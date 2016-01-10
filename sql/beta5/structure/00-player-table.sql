-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/00-player-table.sql
--
-- Beta 5 games:
-- Table that stores player data, minus its foreign keys
-- which will be defined in 99-player-fk.sql
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE player (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	userid		BIGINT		NOT NULL	REFERENCES main.account (id),
	name		VARCHAR(15),
	quit		INT,
	cash		INT		NOT NULL	DEFAULT 20000,
	alliance	INT,
	a_status	CHAR(3)		NULL		CHECK(a_status IS NULL OR a_status IN ('REQ', 'IN')),
	a_grade		BIGINT,
	a_vote		BIGINT,
	first_planet	BIGINT,
	restrain	SMALLINT	NOT NULL	DEFAULT 10,
	research	VARCHAR(8)	NOT NULL	DEFAULT '20!40!40',
	res_assistance	BIGINT				REFERENCES player (id),
	bh_unhappiness	INT		NOT NULL	DEFAULT 0,
	probe_policy	CHAR(4)		NOT NULL	DEFAULT '2110',
	hidden		BOOLEAN		NOT NULL	DEFAULT FALSE
);

CREATE INDEX player_alliance ON player (alliance);
CREATE INDEX player_name ON player (name);
CREATE INDEX player_userid ON player (userid);
CREATE INDEX player_a_vote ON player (a_vote);
CREATE INDEX player_a_grade ON player (a_grade);
CREATE INDEX player_res_assistance ON player (res_assistance);
CREATE INDEX player_first_planet ON player (first_planet);

GRANT SELECT,INSERT,UPDATE ON player TO legacyworlds;
GRANT SELECT,UPDATE ON player_id_seq TO legacyworlds;

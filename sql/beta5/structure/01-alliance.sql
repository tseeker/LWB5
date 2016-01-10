-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/01-alliance.sql
--
-- Beta 5 games:
-- Main alliance tables including definitions for an
-- alliance itself as well as candidates for presidency
-- and ranks
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE alliance (
	id		SERIAL		NOT NULL	PRIMARY KEY,
	tag		VARCHAR(5)	NOT NULL	UNIQUE,
	name		VARCHAR(64)	NOT NULL,
	leader		BIGINT				REFERENCES player (id),
	successor	BIGINT				REFERENCES player (id),
	democracy	BOOLEAN		NOT NULL	DEFAULT FALSE,
	default_grade	BIGINT		NOT NULL,
	enable_tt	CHAR(1)		NOT NULL	DEFAULT 'N' CHECK(enable_tt IN ('N', 'S', 'R'))
);

CREATE INDEX alliance_leader ON alliance (leader);
CREATE INDEX alliance_successor ON alliance (successor);
CREATE INDEX alliance_def_grade ON alliance (default_grade);

GRANT SELECT,INSERT,UPDATE,DELETE ON alliance TO legacyworlds;
GRANT SELECT,UPDATE ON alliance_id_seq TO legacyworlds;



--
-- Table for alliance ranks
--
-- list_access -> level of access to the alliance's listings
--  NO  - no access
--  PY  - member list
--  PL  - planet list
--  PLD - detailed planet list
--
-- tech_trade -> level of access to the alliance's tech trading system
--  NO - no access
--  SL - submit tech list / view orders
--  SR - submit tech list and requests / view orders
--  VL - submit tech list and requests / view orders / view list
--  MR - submit tech list and requests / view orders / view list / manage recommandations
--


CREATE TABLE alliance_grade (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	alliance	INT				REFERENCES alliance (id) ON DELETE CASCADE,
	name		VARCHAR(32),
	list_access	CHAR(3)		NOT NULL	DEFAULT 'PLD' CHECK(list_access IN ('NO','PY','PL','PLD')),
	attacks		BOOLEAN		NOT NULL 	DEFAULT TRUE,
	can_set_grades	BOOLEAN		NOT NULL 	DEFAULT FALSE,
	can_kick	BOOLEAN		NOT NULL 	DEFAULT FALSE,
	can_accept	BOOLEAN		NOT NULL 	DEFAULT FALSE,
	can_be_kicked	BOOLEAN		NOT NULL 	DEFAULT TRUE,
	forum_admin	BOOLEAN		NOT NULL 	DEFAULT FALSE,
	dipl_contact	BOOLEAN		NOT NULL 	DEFAULT FALSE,
	can_vote	BOOLEAN		NOT NULL 	DEFAULT TRUE,
	can_be_cand	BOOLEAN		NOT NULL 	DEFAULT TRUE,
	tech_trade	CHAR(2)		NOT NULL	DEFAULT 'NO' CHECK(tech_trade IN ('NO','SL','SR','VL','MR'))
);

CREATE INDEX algr_alliance ON alliance_grade (alliance);
ALTER TABLE alliance ADD FOREIGN KEY (default_grade) REFERENCES alliance_grade (id);

GRANT SELECT,INSERT,UPDATE,DELETE ON alliance_grade TO legacyworlds;
GRANT SELECT,UPDATE ON alliance_grade_id_seq TO legacyworlds;



CREATE TABLE algr_chgrade (
	grade		BIGINT		NOT NULL	REFERENCES alliance_grade (id) ON DELETE CASCADE,
	can_change	BIGINT		NOT NULL	REFERENCES alliance_grade (id) ON DELETE CASCADE,
	PRIMARY KEY (grade, can_change)
);

CREATE INDEX algr_chgrade_target ON algr_chgrade (can_change);
GRANT SELECT,INSERT,UPDATE,DELETE ON algr_chgrade TO legacyworlds;



CREATE TABLE algr_kick (
	grade		BIGINT		NOT NULL	REFERENCES alliance_grade (id) ON DELETE CASCADE,
	kick		BIGINT		NOT NULL	REFERENCES alliance_grade (id) ON DELETE CASCADE,
	PRIMARY KEY (grade, kick)
);

CREATE INDEX algr_kick_target ON algr_kick (kick);
GRANT SELECT,INSERT,UPDATE,DELETE ON algr_kick TO legacyworlds;



CREATE TABLE alliance_candidate (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	alliance	INT		NOT NULL	REFERENCES alliance (id) ON DELETE CASCADE,
	candidate	BIGINT		NOT NULL	REFERENCES player (id),
	UNIQUE (alliance, candidate)
);

CREATE INDEX alcand_candidate ON alliance_candidate (candidate);
GRANT SELECT,INSERT,UPDATE,DELETE ON alliance_candidate TO legacyworlds;
GRANT SELECT,UPDATE ON alliance_candidate_id_seq TO legacyworlds;


--
-- Alliance status regarding the victory conditions
-- in test match 1.
--
CREATE TABLE alliance_victory (
	alliance	INT		NOT NULL	PRIMARY KEY REFERENCES alliance(id) ON DELETE CASCADE,
	time_of_victory	INT		NULL
);

GRANT SELECT,INSERT,UPDATE,DELETE ON alliance_victory TO legacyworlds;

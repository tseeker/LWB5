-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/03-fleets.sql
--
-- Beta 5 games:
-- The table that containst fleet-related data
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE fleet (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	owner		BIGINT		NOT NULL	REFERENCES player (id),
	name		VARCHAR(64)	NOT NULL	DEFAULT 'No Name',
	location	BIGINT				REFERENCES planet (id),
	gaships		INT		NOT NULL	DEFAULT 0 CHECK (gaships >= 0),
	fighters	INT		NOT NULL	DEFAULT 0 CHECK (fighters >= 0),
	cruisers	INT		NOT NULL	DEFAULT 0 CHECK (cruisers >= 0),
	bcruisers	INT		NOT NULL	DEFAULT 0 CHECK (bcruisers >= 0),
	attacking	BOOLEAN		NOT NULL	DEFAULT FALSE,
	can_move	CHAR(1)		NOT NULL	DEFAULT 'Y' CHECK(can_move IN ('Y','H','B')),
	moving		BIGINT				REFERENCES moving_object (id) ON DELETE SET NULL,
	waiting		BIGINT				REFERENCES hs_wait (id) ON DELETE SET NULL,
	time_spent	INT		NOT NULL	DEFAULT 0,
	sale		SMALLINT,
	CHECK(gaships + fighters + cruisers + bcruisers > 0)
);

CREATE INDEX fleet_owner ON fleet (owner);
CREATE INDEX fleet_location ON fleet (location);
CREATE INDEX fleet_moving ON fleet (moving);
CREATE INDEX fleet_waiting ON fleet (waiting);

GRANT INSERT,SELECT,DELETE,UPDATE ON fleet TO legacyworlds;
GRANT SELECT,UPDATE ON fleet_id_seq TO legacyworlds;


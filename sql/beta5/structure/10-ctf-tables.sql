-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/10-ctf-tables.sql
--
-- Beta 5 games:
-- Tables specific to CTF games
--
-- Copyright(C) 2004-2008, DeepClone Development
-- --------------------------------------------------------


--
-- Target status
--
-- Targets are always listed in this table, and the status
-- of the targets is set according to who holds the system.
--

CREATE TABLE ctf_target (
	system		INT			PRIMARY KEY REFERENCES system (id),
	held_by		INT			REFERENCES alliance (id),
	held_since	INT,
	grace_expires	INT,

	CHECK( (held_by IS NULL AND held_since IS NULL)
	    OR (held_by IS NOT NULL AND held_since IS NOT NULL) ),

	CHECK( (held_by IS NULL AND grace_expires IS NULL) OR held_by IS NOT NULL )
);

CREATE INDEX ctf_target_alliance ON ctf_target (held_by);

GRANT SELECT,INSERT,UPDATE ON ctf_target TO legacyworlds;


--
-- Allocated system status
--
-- This is used in order to clear players out of other
-- teams' zones and to know where each player was spawned.
--

CREATE TABLE ctf_alloc (
	system		INT			PRIMARY KEY REFERENCES system (id),
	alliance	INT	NOT NULL	CHECK( alliance > 0 AND alliance < 9 ),
	spawn_point	BOOLEAN	NOT NULL,
	player		BIGINT			REFERENCES player (id),

	CHECK( spawn_point OR (NOT spawn_point AND player IS NULL) )
);

GRANT SELECT,INSERT,UPDATE ON ctf_alloc TO legacyworlds;


--
-- Team points
--
-- This table stores the points for each team
--

CREATE TABLE ctf_points (
	team		INT	NOT NULL	PRIMARY KEY REFERENCES alliance(id),
	points		INT	NOT NULL	DEFAULT 0 CHECK (points >= 0 AND points <= 100)
);
GRANT SELECT,INSERT,UPDATE ON ctf_points TO legacyworlds;


--
-- Game messages
--
-- The message type is one of the following:
--   0 => Player joined the game, inform him [team]
--   1 => Player joined a team, inform the rest of the team [team]
--   2 => A player's team is now holding all the targets [team,time_stamp]
--   3 => Another team is now holding all the targets [team,time_stamp]
--   4 => A player's team is no longer holding all the targets, but there is a grace period [team,time_stamp]
--   5 => A player's team is no longer holding all the targets, and there is no grace period [team]
--   6 => A player's team is no longer holding all the targets, and the grace period has expired [team]
--   7 => Another team is no longer holding all the targets, but there is a grace period [team,time_stamp]
--   8 => Another team is no longer holding all the targets, and there is no grace period [team]
--   9 => Another team is no longer holding all the targets, and the grace period has expired [team]
--  10 => A player's team is still holding the targets after half the required time [team,time_stamp]
--  11 => Another team is still holding the targets after half the required time [team,time_stamp]
--  12 => A player's team has held the targets long enough and the game has been reset [team]
--  13 => Another team has held the targets long enough and the game has been reset [team]
--  14 => A player's team has won the match [team]
--  15 => A player's team has lost the match [team]

CREATE TABLE msg_ctf (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	msg_type	INT		NOT NULL	CHECK(msg_type >= 0 AND msg_type < 16),
	team		INT		NOT NULL	REFERENCES alliance (id),
	time_stamp	INT
);

CREATE INDEX msg_ctf_team ON msg_ctf (team);

GRANT SELECT,INSERT,UPDATE,DELETE ON msg_ctf TO legacyworlds;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/01-player-dipl.sql
--
-- Beta 5 games:
-- Data structures that store everything related to a
-- player's diplomatic relations
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE research_assistance (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	price		INT		NOT NULL	CHECK(price >= 0),
	offer_to	BIGINT		NOT NULL	REFERENCES player (id),
	technology	INT				REFERENCES research (id),
	moment		INT		NOT NULL	DEFAULT INT4(EXTRACT(EPOCH FROM NOW())),
	accepted	BOOLEAN
);

CREATE INDEX res_as_player ON research_assistance (player);
CREATE INDEX res_as_offer_to ON research_assistance (offer_to);
CREATE INDEX res_as_technology ON research_assistance (technology);
CREATE INDEX res_as_moment ON research_assistance (moment);

GRANT SELECT,INSERT,UPDATE ON research_assistance TO legacyworlds;
GRANT SELECT,UPDATE ON research_assistance_id_seq TO legacyworlds;



CREATE TABLE enemy_alliance (
	player		BIGINT		NOT NULL	REFERENCES player (id),
	alliance	INT		NOT NULL	REFERENCES alliance (id) ON DELETE CASCADE,
	PRIMARY KEY (player, alliance)
);

CREATE INDEX enemy_alliance_idx ON enemy_alliance (alliance);
GRANT SELECT,INSERT,DELETE ON enemy_alliance TO legacyworlds;



CREATE TABLE enemy_player (
	player		BIGINT		NOT NULL	REFERENCES player (id),
	enemy		BIGINT		NOT NULL	REFERENCES player (id),
	PRIMARY KEY (player, enemy)
);

CREATE INDEX enemy_player_idx ON enemy_player (enemy);
GRANT SELECT,INSERT,DELETE ON enemy_player TO legacyworlds;



CREATE TABLE trusted (
	player		BIGINT		NOT NULL	REFERENCES player (id),
	level		SMALLINT	NOT NULL	CHECK(level >= 0),
	friend		BIGINT		NOT NULL	REFERENCES player (id),
	PRIMARY KEY (player, level),
	UNIQUE (player, level)
);
GRANT SELECT,INSERT,DELETE,UPDATE ON trusted TO legacyworlds;



CREATE TABLE trusted_ban (
	player		BIGINT		NOT NULL	REFERENCES player (id),
	ban_player	BIGINT		NOT NULL	REFERENCES player (id),
	PRIMARY KEY (player, ban_player)
);

CREATE INDEX trusted_ban_idx ON trusted_ban (ban_player);
GRANT SELECT,INSERT,DELETE ON trusted_ban TO legacyworlds;



CREATE TABLE donation_log (
	time		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	source		BIGINT		NOT NULL	REFERENCES player (id),
	target		BIGINT		NOT NULL	REFERENCES player (id),
	amount		INT		NOT NULL	CHECK(amount > 0),
	PRIMARY KEY (time, source)
);

CREATE INDEX donation_source ON donation_log (source);
CREATE INDEX donation_target ON donation_log (target);

GRANT INSERT,SELECT ON donation_log TO legacyworlds;

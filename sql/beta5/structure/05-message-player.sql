-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/05-message-player.sql
--
-- Beta 5 games:
-- Tables for the player-controlled messages
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



CREATE TABLE msg_std (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	sender		BIGINT		NOT NULL	REFERENCES player (id),
	recipient	BIGINT		NOT NULL	REFERENCES player (id),
	subject		VARCHAR(64)	NOT NULL,
	message		text		NOT NULL
);

CREATE INDEX msg_std_sender ON msg_std (sender);
CREATE INDEX msg_std_recipient ON msg_std (recipient);
GRANT INSERT,DELETE,SELECT ON msg_std TO legacyworlds;



CREATE TABLE msg_planet (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	pname		VARCHAR(15)	NOT NULL,
	sender		BIGINT		NOT NULL	REFERENCES player (id),
	subject		VARCHAR(64)	NOT NULL,
	message		text		NOT NULL
);

CREATE INDEX msg_planet_sender ON msg_planet (sender);
CREATE INDEX msg_planet_planet ON msg_planet (planet);
GRANT INSERT,DELETE,SELECT ON msg_planet TO legacyworlds;



CREATE TABLE msg_alliance (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	sender		BIGINT		NOT NULL	REFERENCES player (id),
	alliance	INT				REFERENCES alliance (id) ON DELETE SET NULL,
	tag		VARCHAR(5)	NOT NULL,
	subject		VARCHAR(64)	NOT NULL,
	message		TEXT		NOT NULL
);

CREATE INDEX msg_alliance_sender ON msg_alliance (sender);
CREATE INDEX msg_alliance_alliance ON msg_alliance (alliance);
GRANT INSERT,DELETE,SELECT ON msg_alliance TO legacyworlds;



CREATE TABLE msg_diplchan (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	sender		BIGINT		NOT NULL	REFERENCES player (id),
	recipient	BIGINT		NOT NULL	REFERENCES player (id),
	alliance	INT				REFERENCES alliance (id) ON DELETE SET NULL,
	tag		VARCHAR(5)	NOT NULL,
	subject		VARCHAR(64)	NOT NULL,
	message		TEXT		NOT NULL
);

CREATE INDEX msg_diplchan_sender ON msg_diplchan (sender);
CREATE INDEX msg_diplchan_recipient ON msg_diplchan (recipient);
CREATE INDEX msg_diplchan_alliance ON msg_diplchan (alliance);
GRANT INSERT,DELETE,SELECT ON msg_diplchan TO legacyworlds;

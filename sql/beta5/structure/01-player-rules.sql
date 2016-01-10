-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/01-player-rules.sql
--
-- Beta 5 games:
-- Data structures that store the rules that apply for
-- each player, as well as the list of implemented techs
-- for each player
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE research_player (
	player		BIGINT		NOT NULL	REFERENCES player (id),
	research	INT		NOT NULL	REFERENCES research (id),
	possible	BOOLEAN		NOT NULL,
	in_effect	SMALLINT	NOT NULL	DEFAULT 0,
	points		INT		NOT NULL	DEFAULT 0 CHECK(points >= 0),
	given_by	BIGINT				REFERENCES player (id),
	PRIMARY KEY (player, research)
);

CREATE INDEX resplayer_research ON research_player (research);
CREATE INDEX resplayer_giver ON research_player (given_by);

GRANT SELECT,INSERT,UPDATE,DELETE ON research_player TO legacyworlds;



CREATE TABLE rule (
	name		VARCHAR(32)	NOT NULL	REFERENCES rule_def (name),
	player		BIGINT		NOT NULL	REFERENCES player (id),
	value		INT		NOT NULL,
	PRIMARY KEY (name, player)
);

CREATE INDEX rule_player ON rule (player);
GRANT SELECT,INSERT,UPDATE,DELETE ON rule TO legacyworlds;

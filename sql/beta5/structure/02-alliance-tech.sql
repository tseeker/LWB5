-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/02-alliance-tech.sql
--
-- Beta 5 games:
-- Tables for the alliance technology trading tool.
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


--
-- Table tech_trade_list
--
-- Contains the lists for all players in an alliance
--
-- status: indicates the player's status w/r to a tech
--  N - New technology
--  L - Law
--  I - Implemented technology
--  F - Foreseen breakthrough
--

CREATE TABLE tech_trade_list (
	alliance	INT		NOT NULL	REFERENCES alliance (id) ON DELETE CASCADE,
	member		BIGINT		NOT NULL	REFERENCES player (id) ON DELETE CASCADE,
	tech		INT		NOT NULL	REFERENCES research (id),
	submitted	INT		NOT NULL	DEFAULT UNIX_TIMESTAMP( NOW() ),
	status		CHAR(1)		NOT NULL	CHECK(status IN ('N', 'L', 'I', 'F')),
	PRIMARY KEY(member, tech)
);

CREATE INDEX tech_trade_list_alliance ON tech_trade_list (alliance);
CREATE INDEX tech_trade_list_tech ON tech_trade_list (tech);

GRANT INSERT,SELECT,DELETE ON tech_trade_list TO legacyworlds;


--
-- Table tech_trade_request
--
-- Contains the list of requests made by alliance members
--

CREATE TABLE tech_trade_request (
	alliance	INT		NOT NULL	REFERENCES alliance (id) ON DELETE CASCADE,
	player		BIGINT		NOT NULL	REFERENCES player (id) ON DELETE CASCADE,
	priority	INT		NOT NULL	CHECK(priority BETWEEN 0 AND 6),
	tech		INT		NOT NULL	REFERENCES research (id),

	PRIMARY KEY(player, priority),
	UNIQUE(player, tech)
);

CREATE INDEX tech_trade_req_alliance ON tech_trade_request (alliance);
CREATE INDEX tech_trade_req_tech ON tech_trade_request (tech);

GRANT INSERT,SELECT,DELETE,UPDATE ON tech_trade_request TO legacyworlds;


--
-- Table tech_trade_order
--
-- Contains the orders issued for tech trades
--

CREATE TABLE tech_trade_order (
	alliance	INT		NOT NULL	REFERENCES alliance (id) ON DELETE CASCADE,
	player		BIGINT		NOT NULL	REFERENCES player (id) ON DELETE CASCADE,
	send_to		BIGINT		NOT NULL	UNIQUE REFERENCES player (id) ON DELETE CASCADE,
	tech		INT		NOT NULL	REFERENCES research (id),
	submitted	INT		NOT NULL	DEFAULT UNIX_TIMESTAMP( NOW() ),
	obeyed		INT,
	PRIMARY KEY(player)
);

CREATE INDEX tech_trade_order_tech ON tech_trade_order (tech);
CREATE INDEX tech_trade_order_alliance ON tech_trade_order (alliance);

GRANT SELECT,INSERT,UPDATE,DELETE ON tech_trade_order TO legacyworlds;

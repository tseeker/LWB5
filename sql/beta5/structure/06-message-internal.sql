-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/06-message-internal.sql
--
-- Beta 5 games:
-- Tables for all types of internal messages except for
-- battle reports
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



--
-- Planets being abandoned
--
CREATE TABLE msg_abandon (
	id		BIGINT		NOT NULL	REFERENCES message (id) ON DELETE CASCADE,
	p_id		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	PRIMARY KEY (id, p_id)
);
CREATE INDEX msg_abandon_planet ON msg_abandon (p_id);
GRANT INSERT,DELETE,SELECT ON msg_abandon TO legacyworlds;



--
-- Alliance internal messages
--
CREATE TABLE msg_alint (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	alliance	INT				REFERENCES alliance (id) ON DELETE SET NULL,
	tag		VARCHAR(5)	NOT NULL,
	player		BIGINT				REFERENCES player (id),
	msg_type	SMALLINT	NOT NULL	CHECK(msg_type >= 0)
);
CREATE INDEX msg_alint_alliance ON msg_alint (alliance);
CREATE INDEX msg_alint_player ON msg_alint (player);
GRANT INSERT,DELETE,SELECT ON msg_alint TO legacyworlds;



--
-- Bids on auction sales
--
CREATE TABLE msg_bid (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	offer		BIGINT				REFERENCES public_offer (offer) ON DELETE SET NULL,
	is_planet	BOOLEAN		NOT NULL,
	pname		VARCHAR(15)	NOT NULL,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	f_gas		INT		NOT NULL	DEFAULT 0,
	f_fighters	INT		NOT NULL	DEFAULT 0,
	f_cruisers	INT		NOT NULL	DEFAULT 0,
	f_bcruisers	INT		NOT NULL	DEFAULT 0,
	new_price	INT		NOT NULL	DEFAULT 0,
	last_bidder	BIGINT				REFERENCES player (id)
);
CREATE INDEX msg_bid_offer ON msg_bid (offer);
CREATE INDEX msg_bid_planet ON msg_bid (planet);
CREATE INDEX msg_bid_last_bidder ON msg_bid (last_bidder);
GRANT INSERT,DELETE,SELECT ON msg_bid TO legacyworlds;



--
-- Cash donations
--
CREATE TABLE msg_cash (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	p_id		BIGINT		NOT NULL	REFERENCES player (id),
	amount		INT		NOT NULL	CHECK(amount > 0)
);
CREATE INDEX msg_cash_player ON msg_cash (p_id);
GRANT INSERT,DELETE,SELECT ON msg_cash TO legacyworlds;


--
-- Fleet movements
--
CREATE TABLE msg_flmove (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	p_id		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL
);
CREATE INDEX msg_flmove_planet ON msg_flmove (p_id);
GRANT INSERT,DELETE,SELECT ON msg_flmove TO legacyworlds;

CREATE TABLE flmove_data (
	id		BIGINT		NOT NULL	REFERENCES msg_flmove (id) ON DELETE CASCADE,
	f_name		VARCHAR(64)	NOT NULL,
	f_owner		BIGINT		NOT NULL	REFERENCES player (id),
	f_gaships	INT		NOT NULL	DEFAULT 0,
	f_fighters	INT		NOT NULL	DEFAULT 0,
	f_cruisers	INT		NOT NULL	DEFAULT 0,
	f_bcruisers	INT		NOT NULL	DEFAULT 0,
	f_power		INT		NOT NULL,
	hostile		BOOLEAN		NOT NULL	DEFAULT FALSE,
	arrived		BOOLEAN		NOT NULL	DEFAULT FALSE,
	from_id		BIGINT				REFERENCES planet (id),
	from_name	VARCHAR(15)
);
CREATE INDEX flmove_id ON flmove_data (id);
CREATE INDEX flmove_owner ON flmove_data (f_owner);
CREATE INDEX flmove_origin ON flmove_data (from_id);
GRANT INSERT,DELETE,SELECT ON flmove_data TO legacyworlds;



--
-- Fleets that get switched to attack due to enemy lists
--
CREATE TABLE msg_flswitch (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	pname		VARCHAR(15)	NOT NULL,
	n_fleets	INT		NOT NULL	CHECK(n_fleets > 0),
	fpower		INT		NOT NULL	CHECK(fpower > 0)
);
CREATE INDEX msg_flswitch_planet ON msg_flswitch (planet);
GRANT INSERT,DELETE,SELECT ON msg_flswitch TO legacyworlds;



--
-- Hyperspace standby losses
--
CREATE TABLE msg_hsloss (
	id		BIGINT		NOT NULL	REFERENCES message (id) ON DELETE CASCADE,
	p_id		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	f_name		VARCHAR(64)	NOT NULL,
	gaships		INT		NOT NULL	DEFAULT 0,
	fighters	INT		NOT NULL	DEFAULT 0,
	cruisers	INT		NOT NULL	DEFAULT 0,
	bcruisers	INT		NOT NULL	DEFAULT 0,
	power		INT		NOT NULL	CHECK(power > 0)
);
CREATE INDEX msg_hsloss_idx ON msg_hsloss (id,p_id);
CREATE INDEX msg_hsloss_planet ON msg_hsloss (p_id);
GRANT INSERT,DELETE,SELECT ON msg_hsloss TO legacyworlds;



--
-- Fleet losses due to insufficient income
--
CREATE TABLE msg_kfleet (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	gaships		INT		NOT NULL	DEFAULT 0,
	fighters	INT		NOT NULL	DEFAULT 0,
	cruisers	INT		NOT NULL	DEFAULT 0,
	bcruisers	INT		NOT NULL	DEFAULT 0
);
GRANT INSERT,DELETE,SELECT ON msg_kfleet TO legacyworlds;



--
-- Planetary improvents losses due to insufficient income
--
CREATE TABLE msg_kimpr (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	turrets		INT		NOT NULL	DEFAULT 0,
	factories	INT		NOT NULL	DEFAULT 0
);
GRANT INSERT,DELETE,SELECT ON msg_kimpr TO legacyworlds;



--
-- Players leaving the game
--
CREATE TABLE msg_leave (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	reason		CHAR(4)		NOT NULL	DEFAULT 'QUIT' CHECK(reason IN ('QUIT', 'INAC', 'KICK')),
	tag		VARCHAR(5),
	ally		BOOLEAN		NOT NULL	DEFAULT FALSE,
	sales_to	INT		NOT NULL	DEFAULT 0,
	sales_from	INT		NOT NULL	DEFAULT 0
);
CREATE INDEX msg_leave_player ON msg_leave (player);
GRANT INSERT,DELETE,SELECT ON msg_leave TO legacyworlds;



--
-- Planet owner changes
--
CREATE TABLE msg_ownerch (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	owner		BIGINT		NULL		REFERENCES player (id),
	p_id		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	status		CHAR(4)		NOT NULL	CHECK(status IN ('LOSE', 'TAKE', 'VIEW'))
);
CREATE INDEX msg_ownerch_owner ON msg_ownerch (owner);
CREATE INDEX msg_ownerch_planet ON msg_ownerch (p_id);
GRANT INSERT,DELETE,SELECT ON msg_ownerch TO legacyworlds;



--
-- Planet sales cancelled due to owner changes
--
CREATE TABLE msg_plsc (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	seller		BIGINT		NOT NULL	REFERENCES player (id),
	taker		BIGINT		NOT NULL	REFERENCES player (id),
	p_id		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL
);
CREATE INDEX msg_plsc_seller ON msg_plsc (seller);
CREATE INDEX msg_plsc_taker ON msg_plsc (taker);
CREATE INDEX msg_plsc_planet ON msg_plsc (p_id);
GRANT INSERT,DELETE,SELECT ON msg_plsc TO legacyworlds;



--
-- Planets being renamed
--
CREATE TABLE msg_rename (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	status		CHAR(5)		NOT NULL	DEFAULT 'ORBIT' CHECK(status IN ('ORBIT','MOVE','PROBE')),
	old_name	VARCHAR(15)	NOT NULL,
	new_name	VARCHAR(15)	NOT NULL
);
CREATE INDEX msg_rename_planet ON msg_rename (planet);
GRANT INSERT,DELETE,SELECT ON msg_rename TO legacyworlds;



--
-- Research diplomacy messages
--
CREATE TABLE msg_resdipl (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	offer		BIGINT		NOT NULL	REFERENCES research_assistance (id),
	msg_id		SMALLINT	NOT NULL	CHECK(msg_id >= 0)
);
CREATE INDEX msg_resdipl_offer ON msg_resdipl (offer);
GRANT INSERT,DELETE,SELECT ON msg_resdipl TO legacyworlds;



--
-- Damage to infrastructure because of revolts
--
CREATE TABLE msg_revdmg (
	id		BIGINT		NOT NULL	REFERENCES message (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	pname		VARCHAR(15)	NOT NULL,
	ifact		INT		NOT NULL	DEFAULT 0,
	mfact		INT		NOT NULL	DEFAULT 0,
	turrets		INT		NOT NULL	DEFAULT 0,
	PRIMARY KEY (id, planet)
);
CREATE INDEX msg_revdmg_planet ON msg_revdmg (planet);
GRANT INSERT,DELETE,SELECT ON msg_revdmg TO legacyworlds;



--
-- Beginning and end of revolts
--
CREATE TABLE msg_revolt (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	pname		VARCHAR(15)	NOT NULL,
	started		BOOLEAN		NOT NULL
);
CREATE INDEX msg_revolt_planet ON msg_revolt (planet);
GRANT INSERT,DELETE,SELECT ON msg_revolt TO legacyworlds;



--
-- Sales
--
CREATE TABLE msg_sale (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	p_id		BIGINT				REFERENCES planet (id),
	p_name		VARCHAR(15),
	f_name		VARCHAR(64),
	f_power		INT,
	pl_id		BIGINT		NOT NULL	REFERENCES player (id),
	pl_name		VARCHAR(15)	NOT NULL,
	is_sale		BOOLEAN		NOT NULL	DEFAULT FALSE
);
CREATE INDEX msg_sale_planet ON msg_sale (p_id);
CREATE INDEX msg_sale_player ON msg_sale (pl_id);
GRANT INSERT,DELETE,SELECT ON msg_sale TO legacyworlds;



--
-- Warnings for incorrect planet names
--
CREATE TABLE msg_warnname (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	moderator	BIGINT		NOT NULL	REFERENCES player (id),
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	event		CHAR(5)		NOT NULL	DEFAULT 'WARN' CHECK(event IN ('WARN', 'NEUT', 'ANEUT'))
);
CREATE INDEX msg_warnname_moderator ON msg_warnname (moderator);
CREATE INDEX msg_warnname_planet ON msg_warnname (planet);
GRANT INSERT,DELETE,SELECT ON msg_warnname TO legacyworlds;



--
-- Planets that get blown up by the WHSN tech
--
CREATE TABLE msg_whsn (
	id		BIGINT		NOT NULL	PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	p_id		BIGINT		NOT NULL	REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	was_owner	BOOLEAN		NOT NULL	DEFAULT FALSE,
	f_power		INT		NOT NULL	DEFAULT 0,
	e_power		INT		NOT NULL	DEFAULT 0
);
CREATE INDEX msg_whsn_planet ON msg_whsn (p_id);
GRANT INSERT,DELETE,SELECT ON msg_whsn TO legacyworlds;

CREATE TABLE whsn_fleet (
	id		BIGINT		NOT NULL	REFERENCES msg_whsn (id) ON DELETE CASCADE,
	name		VARCHAR(64)	NOT NULL,
	gaships		INT		NOT NULL	DEFAULT 0,
	fighters	INT		NOT NULL	DEFAULT 0,
	cruisers	INT		NOT NULL	DEFAULT 0,
	bcruisers	INT		NOT NULL	DEFAULT 0,
	power		INT		NOT NULL	CHECK(power > 0)
);
CREATE INDEX whsn_fleet_msg ON whsn_fleet (id);
GRANT INSERT,DELETE,SELECT ON whsn_fleet TO legacyworlds;

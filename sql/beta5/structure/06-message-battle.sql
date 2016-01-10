-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/06-message-battle.sql
--
-- Beta 5 games:
-- Table to store battle reports
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



CREATE TABLE msg_battle (
	id		BIGINT		NOT NULL	REFERENCES message (id) ON DELETE CASCADE,
	planet_id	BIGINT		NOT NULL	REFERENCES planet (id),
	planet		VARCHAR(15)	NOT NULL,
	o_gaships	INT		NOT NULL	DEFAULT 0,
	o_fighters	INT		NOT NULL	DEFAULT 0,
	o_cruisers	INT		NOT NULL	DEFAULT 0,
	o_bcruisers	INT		NOT NULL	DEFAULT 0,
	o_power		INT		NOT NULL	DEFAULT 0,
	ol_gaships	INT		NOT NULL	DEFAULT 0,
	ol_fighters	INT		NOT NULL	DEFAULT 0,
	ol_cruisers	INT		NOT NULL	DEFAULT 0,
	ol_bcruisers	INT		NOT NULL	DEFAULT 0,
	ol_power	INT		NOT NULL	DEFAULT 0,
	a_gaships	INT		NOT NULL	DEFAULT 0,
	a_fighters	INT		NOT NULL	DEFAULT 0,
	a_cruisers	INT		NOT NULL	DEFAULT 0,
	a_bcruisers	INT		NOT NULL	DEFAULT 0,
	a_power		INT		NOT NULL	DEFAULT 0,
	al_gaships	INT		NOT NULL	DEFAULT 0,
	al_fighters	INT		NOT NULL	DEFAULT 0,
	al_cruisers	INT		NOT NULL	DEFAULT 0,
	al_bcruisers	INT		NOT NULL	DEFAULT 0,
	al_power	INT		NOT NULL	DEFAULT 0,
	e_gaships	INT		NOT NULL	DEFAULT 0,
	e_fighters	INT		NOT NULL	DEFAULT 0,
	e_cruisers	INT		NOT NULL	DEFAULT 0,
	e_bcruisers	INT		NOT NULL	DEFAULT 0,
	e_power		INT		NOT NULL	DEFAULT 0,
	el_gaships	INT		NOT NULL	DEFAULT 0,
	el_fighters	INT		NOT NULL	DEFAULT 0,
	el_cruisers	INT		NOT NULL	DEFAULT 0,
	el_bcruisers	INT		NOT NULL	DEFAULT 0,
	el_power	INT		NOT NULL	DEFAULT 0,
	turrets		INT		NOT NULL	DEFAULT 0,
	tpower		INT		NOT NULL	DEFAULT 0,
	l_turrets	INT		NOT NULL	DEFAULT 0,
	l_tpower	INT		NOT NULL	DEFAULT 0,
	tmode		SMALLINT	NOT NULL	DEFAULT 0,
	heroic_def	INT		NOT NULL	CHECK(heroic_def >= -1 AND heroic_def <= 1)
);
CREATE INDEX msg_battle_planet ON msg_battle (planet_id);
GRANT INSERT,DELETE,SELECT ON msg_battle TO legacyworlds;

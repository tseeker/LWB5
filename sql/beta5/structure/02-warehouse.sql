-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/02-warehouse.sql
--
-- Beta 5 games:
-- The tables in which the day ticks store the game's
-- history
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------

CREATE TABLE bt_debug (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	tick_time	INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	txt		TEXT		NOT NULL
);

CREATE INDEX bt_time ON bt_debug (tick_time);
CREATE RULE bt_debug_cleaner AS
	ON INSERT TO bt_debug DO ALSO
		DELETE FROM bt_debug WHERE UNIX_TIMESTAMP(NOW()) - tick_time > 3 * 86400;
GRANT INSERT,DELETE ON bt_debug TO legacyworlds;
GRANT SELECT,UPDATE ON bt_debug_id_seq TO legacyworlds;



CREATE TABLE warehouse (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	tick_at		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	cash		BIGINT		NOT NULL,
	a_tag		VARCHAR(5),
	g_rank		BIGINT		NOT NULL,
	c_rank		BIGINT		NOT NULL,
	m_rank		BIGINT		NOT NULL,
	f_rank		BIGINT		NOT NULL,
	d_rank		BIGINT		NOT NULL,
	o_rank		BIGINT,
	gaships		INT		NOT NULL,
	fighters	INT		NOT NULL,
	cruisers	INT		NOT NULL,
	bcruisers	INT		NOT NULL,
	fleet		INT		NOT NULL,
	tech_list	TEXT		NOT NULL,
	tech_points	BIGINT		NOT NULL,
	UNIQUE (player, tick_at)
);

GRANT INSERT,SELECT,DELETE ON warehouse TO legacyworlds;
GRANT SELECT,UPDATE ON warehouse_id_seq TO legacyworlds;



CREATE TABLE wh_planet (
	id		BIGINT		NOT NULL	REFERENCES warehouse (id) ON DELETE CASCADE,
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	name		VARCHAR(15)	NOT NULL,
	pop		INT		NOT NULL,
	max_pop		INT		NOT NULL,
	ifact		INT		NOT NULL,
	mfact		INT		NOT NULL,
	turrets		INT		NOT NULL,
	happiness	SMALLINT	NOT NULL,
	beacon		SMALLINT	NOT NULL,
	abandon		SMALLINT,
	corruption	INT,
	PRIMARY KEY (id, planet)
);
GRANT INSERT,SELECT,DELETE ON wh_planet TO legacyworlds;

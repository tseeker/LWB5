-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/07-beacons.sql
--
-- Beta 5 games:
-- Tables that handle detection of fleets in HSSB by
-- beacons.
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


--
-- Detection status
--
-- Fleets should be listed in this table when they get
-- detected.
--

CREATE TABLE beacon_detection (
	planet		BIGINT		NOT NULL	REFERENCES planet (id),
	fleet		BIGINT		NOT NULL	REFERENCES fleet (id) ON DELETE CASCADE,
	i_level		INT		NOT NULL	CHECK( i_level >= 0 AND i_level <= 4 ),
	fl_size		INT,
	fl_owner	BIGINT				REFERENCES player (id) ON DELETE CASCADE,
	PRIMARY KEY (planet, fleet),

	CHECK( i_level = 0 AND fl_size IS NULL OR i_level > 0 AND fl_size IS NOT NULL ),
	CHECK( i_level < 4 AND fl_owner IS NULL OR i_level = 4 AND fl_owner IS NOT NULL )
);

CREATE INDEX beacon_detection_fleet ON beacon_detection (fleet);
CREATE INDEX beacon_detection_owner ON beacon_detection (fl_owner);
GRANT SELECT,INSERT,DELETE ON beacon_detection TO legacyworlds;


--
-- Message table
--

CREATE TABLE msg_detect (
	id		BIGINT				PRIMARY KEY REFERENCES message (id) ON DELETE CASCADE,
	planet		BIGINT				REFERENCES planet (id) ON DELETE CASCADE,
	p_name		VARCHAR(15)	NOT NULL,
	is_owner	BOOLEAN		NOT NULL,
	i_level		INT		NOT NULL	CHECK( i_level >= 0 AND i_level <= 4 ),
	fl_size		INT,
	flo_id		BIGINT				REFERENCES player (id) ON DELETE SET NULL,
	flo_name	VARCHAR(15),

	CHECK( is_owner OR NOT is_owner AND (i_level = 0 AND fl_size IS NULL OR i_level > 0 AND fl_size IS NOT NULL) ),
	CHECK( is_owner OR NOT is_owner AND (i_level < 4 AND flo_name IS NULL OR i_level = 4 AND flo_name IS NOT NULL) )
);

CREATE INDEX msg_detect_planet ON msg_detect (planet);
CREATE INDEX msg_detect_owner ON msg_detect (flo_id);
GRANT SELECT,INSERT,DELETE ON msg_detect TO legacyworlds;

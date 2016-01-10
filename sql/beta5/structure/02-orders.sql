-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/02-orders.sql
--
-- Beta 5 games:
-- Data structures for moving objects and hyperspace
-- stand-by
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE hs_wait (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	time_left	INT		NOT NULL	CHECK(time_left BETWEEN 0 AND 48),
	time_spent	INT		NOT NULL	DEFAULT 0 CHECK(time_spent >= 0),
	origin		BIGINT				REFERENCES planet (id),
	drop_point	BIGINT		NOT NULL	REFERENCES planet (id)
);

CREATE INDEX wait_origin ON hs_wait (origin);
CREATE INDEX wait_drop_point ON hs_wait (drop_point);

GRANT SELECT,INSERT,UPDATE,DELETE ON hs_wait TO legacyworlds;
GRANT SELECT,UPDATE ON hs_wait_id_seq TO legacyworlds;



CREATE TABLE moving_object (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	m_from		BIGINT		NOT NULL	REFERENCES planet (id),
	m_to		BIGINT		NOT NULL	REFERENCES planet (id),
	changed		SMALLINT	NOT NULL	DEFAULT 0 CHECK(changed >= 0),
	time_left	INT		NOT NULL	CHECK(time_left >= 0),
	hyperspace	BOOLEAN		NOT NULL,
	wait_order	BIGINT				REFERENCES hs_wait (id) ON DELETE SET NULL
);

CREATE INDEX move_from ON moving_object (m_from);
CREATE INDEX move_to ON moving_object (m_to);
CREATE INDEX move_hssb ON moving_object (wait_order);

GRANT SELECT,INSERT,UPDATE,DELETE ON moving_object TO legacyworlds;
GRANT SELECT,UPDATE ON moving_object_id_seq TO legacyworlds;



CREATE TABLE waypoint (
	move_id		BIGINT		NOT NULL	REFERENCES moving_object (id) ON DELETE CASCADE,
	location	BIGINT		NOT NULL	REFERENCES planet (id),
	until_eta	INT		NOT NULL	CHECK(until_eta >= 0),
	PRIMARY KEY (move_id, until_eta)
);

CREATE INDEX waypoint_location ON waypoint (location);
GRANT SELECT,INSERT,UPDATE,DELETE ON waypoint TO legacyworlds;

-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/00-ecm-eccm.sql
--
-- Beta 5 games:
-- Tables that store ECM and ECCM settings
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE eccm (
	eccm_level	SMALLINT	NOT NULL,
	gain		SMALLINT	NOT NULL,
	probability	SMALLINT	NOT NULL,
	PRIMARY KEY (eccm_level, gain)
);

CREATE TABLE ecm (
	ecm_level	SMALLINT	NOT NULL,
	info_level	SMALLINT	NOT NULL,
	probability	SMALLINT	NOT NULL,
	PRIMARY KEY (ecm_level, info_level)
);

GRANT SELECT ON eccm TO legacyworlds;
GRANT SELECT ON ecm TO legacyworlds;

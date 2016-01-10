-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/structure/04-sales.sql
--
-- Beta 5 games:
-- The various tables that store information associated
-- with fleet and planet sales
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


CREATE TABLE sale (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	started		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	expires		INT,
	fleet		BIGINT				REFERENCES fleet (id),
	planet		BIGINT				REFERENCES planet (id),
	finalized	INT,
	sold_to		BIGINT				REFERENCES player (id),
	CHECK(expires IS NULL OR expires > started),
	CHECK(fleet IS NOT NULL OR planet IS NOT NULL),
	CHECK(finalized IS NULL AND sold_to IS NULL OR finalized IS NOT NULL AND sold_to IS NOT NULL)
);

CREATE INDEX sale_player ON sale (player);
CREATE INDEX sale_fleet ON sale (fleet);
CREATE INDEX sale_planet ON sale (planet);
CREATE INDEX sale_sold_to ON sale (sold_to);

GRANT INSERT,SELECT,DELETE,UPDATE ON sale TO legacyworlds;
GRANT SELECT,UPDATE ON sale_id_seq TO legacyworlds;



CREATE TABLE private_offer (
	offer		BIGINT		NOT NULL	REFERENCES sale (id) ON DELETE CASCADE PRIMARY KEY,
	to_player	BIGINT		NOT NULL	REFERENCES player (id),
	price		INT		NOT NULL	DEFAULT 0 CHECK(price >= 0)
);

CREATE INDEX p_offer_to ON private_offer (to_player);
GRANT INSERT,SELECT,DELETE,UPDATE ON private_offer TO legacyworlds;



CREATE TABLE public_offer (
	offer		BIGINT		NOT NULL	REFERENCES sale (id) ON DELETE CASCADE PRIMARY KEY,
	price		INT		NOT NULL	DEFAULT 0 CHECK(price > 0),
	auction		BOOLEAN		NOT NULL	DEFAULT FALSE
);
GRANT INSERT,SELECT,DELETE,UPDATE ON public_offer TO legacyworlds;



CREATE TABLE auction (
	offer		BIGINT		NOT NULL	REFERENCES sale (id) ON DELETE CASCADE,
	player		BIGINT		NOT NULL	REFERENCES player (id),
	moment		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	price		INT		NOT NULL	CHECK(price > 0),
	PRIMARY KEY(offer, player, moment)
);

CREATE INDEX auction_player ON auction (player);
GRANT SELECT,INSERT,DELETE ON auction TO legacyworlds;



CREATE TABLE sale_history (
	id		BIGSERIAL	NOT NULL	PRIMARY KEY,
	offer		BIGINT				REFERENCES sale (id) ON DELETE SET NULL,
	from_player	BIGINT		NOT NULL	REFERENCES player (id),
	to_player	BIGINT				REFERENCES player (id),
	started		INT		NOT NULL	DEFAULT UNIX_TIMESTAMP(NOW()),
	ended		INT,
	mode		SMALLINT	NOT NULL,
	end_mode	SMALLINT,
	price		INT		NOT NULL	CHECK(price >= 0),
	sell_price	INT				CHECK(sell_price IS NULL OR sell_price >= 0),
	p_id		BIGINT				REFERENCES planet (id),
	p_name		VARCHAR(15)	NOT NULL,
	is_planet	BOOLEAN		NOT NULL,
	p_pop		INT,
	p_turrets	INT,
	p_factories	INT,
	f_gaships	INT		NOT NULL	DEFAULT 0,
	f_fighters	INT		NOT NULL	DEFAULT 0,
	f_cruisers	INT		NOT NULL	DEFAULT 0,
	f_bcruisers	INT		NOT NULL	DEFAULT 0,
	CHECK(ended IS NULL OR ended > started)
);

CREATE INDEX shist_offer ON sale_history (offer);
CREATE INDEX shist_from_player ON sale_history (from_player);
CREATE INDEX shist_to_player ON sale_history (to_player);
CREATE INDEX shist_planet ON sale_history (p_id);

GRANT INSERT,UPDATE,SELECT,DELETE ON sale_history TO legacyworlds;
GRANT SELECT,UPDATE ON sale_history_id_seq TO legacyworlds;

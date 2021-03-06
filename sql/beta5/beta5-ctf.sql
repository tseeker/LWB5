-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/beta5-match.sql
--
-- Generic installation script for matches, to be parsed
-- by administrative scripts
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


-- Connect as administrator
\c legacyworlds legacyworlds_admin

-- Create the game's schema
CREATE SCHEMA "b5mX";
GRANT USAGE ON SCHEMA "b5mX" TO legacyworlds;

-- Include the files defining the game's database structure
SET search_path="b5mX",main,public;
\i beta5/structure/00-gdata.sql
\i beta5/structure/00-system.sql
\i beta5/structure/00-ecm-eccm.sql
\i beta5/structure/00-rule-base.sql
\i beta5/structure/00-player-table.sql
\i beta5/structure/01-research-base.sql
\i beta5/structure/01-alliance.sql
\i beta5/structure/01-planet.sql
\i beta5/structure/01-player-dipl.sql
\i beta5/structure/01-player-rules.sql
\i beta5/structure/01-message-base.sql
\i beta5/structure/02-alliance-forums.sql
\i beta5/structure/02-alliance-tech.sql
\i beta5/structure/02-orders.sql
\i beta5/structure/02-warehouse.sql
\i beta5/structure/03-fleets.sql
\i beta5/structure/04-sales.sql
\i beta5/structure/05-message-player.sql
\i beta5/structure/06-message-internal.sql
\i beta5/structure/06-message-battle.sql
\i beta5/structure/07-message-admin.sql
\i beta5/structure/07-beacons.sql
\i beta5/structure/10-ctf-tables.sql
\i beta5/structure/99-player-fk.sql

-- Include the file containing the game's read-only data
\i beta5/data/match.sql

-- Include the file containing initial game data in USER mode
\c - legacyworlds
SET search_path="b5mX",main,public;
\i beta5/data/game.sql

-- Finalise the structure
\c - legacyworlds_admin
SET search_path="b5mX",main,public;
\i beta5/structure/finalise.sql

-- Register the game itself
SELECT register_game ('beta5', 'b5mX');

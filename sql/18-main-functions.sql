-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- 18-main-functions.sql
--
-- Creates SQL functions to be used when registering new
-- games
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------



-- Connect to the database
\c legacyworlds legacyworlds_admin


--
-- Function that adds a ranking description for some ranking type in some language
--
CREATE OR REPLACE FUNCTION main.add_ranking_description (version TEXT, name TEXT, lang TEXT, title TEXT, description TEXT) RETURNS VOID AS $$
	INSERT INTO main.ranking_text (ranking, lang, name, description) VALUES (
		(SELECT id FROM main.ranking_def WHERE version=$1 AND name=$2), $3, $4, $5)
$$ LANGUAGE SQL;


--
-- Function that registers a game
--
CREATE OR REPLACE FUNCTION main.register_game (version TEXT, game_name TEXT) RETURNS VOID AS $$
	INSERT INTO main.ranking_game (ranking, game)
		SELECT id, $2 FROM main.ranking_def
			WHERE version = $1;
$$ LANGUAGE SQL;

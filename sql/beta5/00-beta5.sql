-- LegacyWorlds Beta 5
-- PostgreSQL database scripts
--
-- beta5/00-beta5.sql
--
-- Install the Beta 5-specific data into the main database
--
-- Copyright(C) 2004-2007, DeepClone Development
-- --------------------------------------------------------


\c legacyworlds legacyworlds_admin

SET search_path=main;

COPY ranking_def (version, name, more) FROM STDIN;
beta5	p_general	FALSE
beta5	a_general	TRUE
beta5	p_financial	FALSE
beta5	p_military	FALSE
beta5	p_civ	FALSE
beta5	p_round	FALSE
beta5	p_idr	FALSE
\.


SELECT add_ranking_description('beta5', 'p_general', 'en', 'General Ranking',
	'This ranking corresponds to a combination of civilisation, military and financial rankings. It represents a ' ||
	'player''s current overall advancement and strength in the game.');
SELECT add_ranking_description('beta5', 'a_general', 'en', 'Alliance Ranking',
	'An alliance''s ranking indicates its overall strength; the points for an alliance are the sum of the general ' ||
	'ranking points for all of its members.');
SELECT add_ranking_description('beta5', 'p_financial', 'en', 'Financial Ranking',
	'This ranking corresponds to the economic health of a player''s empire. It takes into account the player''s ' ||
	'banked cash, his income and the number of industrial factories he owns.');
SELECT add_ranking_description('beta5', 'p_military', 'en', 'Military Ranking',
	'This ranking allows to assess the military strength of a player''s empire. Its calculation is based on the ' ||
	'number of turrets and military factories the player owns along with his fleet fire power.');
SELECT add_ranking_description('beta5', 'p_civ', 'en', 'Civilization Ranking',
	'This ranking represents the advancement level of the society in a player''s empire. It takes into account ' ||
	'technology level, population and happiness.');
SELECT add_ranking_description('beta5', 'p_round', 'en', 'Overall Round Ranking',
	'This ranking is calculated based on players'' previous general rankings for the top 15 players. It allows ' ||
	'for a long term estimate of the bets players'' accomplishments.');
SELECT add_ranking_description('beta5', 'p_idr', 'en', 'Inflicted Damage Ranking',
	'This ranking represents the amount of damage a player has inflicted on other players'' fleets.');

<?php

/* Load the engine's code */
$__loader = array(
	'log', 'classloader',
	'version', 'game', 'tick', 'config',
	'db_connection', 'db_accessor', 'db',
	'library', 'actions', 'data_tree',
	'input', 'ajax', 'handler', 'engine',
	'resource', 'tracking', 'session',
	'account', 'prefs', 'output'
);
require_once("../scripts/loader.inc");

/* Identify game version, requested page and engine type, then read request
 * data */
list($sitePath, $page, $eType) = input::identify();
$input = input::read();

/* Load page handler */
handler::load();

/* Load the engine */
engine::load();

/* Connect to the DB */
dbConnect();

/* Set up tracking cookie */
tracking::init();

/* Handle requests */
handleInput();

?>

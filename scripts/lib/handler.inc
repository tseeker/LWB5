<?php

/****************************************************************************
 * PAGE HANDLER FUNCTIONS
 ****************************************************************************/

class handler {

	static $h = null;

	/** This function looks for a handler for the requested combination of game
	 * version and page. If such a handler is found, the file is loaded and the
	 * handler is initialized.
	 */
	function load() {
		$path = input::$path;
		$page = input::$page;
		$game = input::$game;

		if (!file_exists("{$game->siteDir}/handlers") || !is_dir($game->siteDir."/handlers")) {
			l::fatal(8, array("Handlers directory for path '$path' not found (page '$page')"));
		}

		$ifile = "{$game->siteDir}/handlers/$page.inc";
		if (!(file_exists($ifile) && is_readable($ifile) && is_file($ifile))) {
			l::notice("Requested path '" . input::$path . "' doesn't match any handler");
			l::debug("Referer was '{$_SERVER['HTTP_REFERER']}' ; requested page was '" . input::$page . "'");
			$path = input::$path = 'main';
			$page = input::$page = 'notfound';
			input::$eType = null;
			$game = input::$game = config::getGame('main');
			$ifile = "{$game->siteDir}/handlers/$page.inc";
		}

		if (!include_once($ifile)) {
			l::fatal(10, "File inclusion failed: '$ifile'");
		}
		if (!class_exists('page_handler')) {
			l::fatal(11, "File '$ifile' did not define a page_handler class");
		}

		handler::$h = $handler = new page_handler();
		$handler->game = $game;

		/* The handler is loaded, check the engine type vs. the handler's
		 * supported and default engines.
		 */

		// Get the list of supported engines
		if (is_array($handler->engines)) {
			$engines = $handler->engines;
		} else {
			$engines = array('page', 'css', 'js', 'rpc');
		}
		// Get the default engine
		if (is_null($handler->defaultEngine)) {
			$dEngine = $engines[0];
		} else {
			$dEngine = $handler->defaultEngine;
		}

		// Set the engine type to default if it isn't set
		if (is_null(input::$eType)) {
			input::$eType = $dEngine;
		}

		// Check the engine's type against the list of supported engines
		if (!in_array(input::$eType, $engines)) {
			l::fatal(31, "$path/$page: trying to use unsupported engine type '" . input::$eType . "'");
		}

		return $handler;
	}
}


?>

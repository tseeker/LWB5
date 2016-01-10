<?php

/****************************************************************************
 * DISPLAY ENGINE MANAGEMENT
 ****************************************************************************/


class engine {

	static $e = null;

	/** This function loads a display engine. It then checks the configuration for
	 * the maintenance mode data; if such data is found, the display engine's
	 * maintenance() method is called.
	 * A fatal error is displayed if the engine isn't found in the engines/
	 * subdirectory of the script directory.
	 */
	function load() {
		$eType = input::$eType;

		// Check for a valid type
		if (preg_match('/[^A-Za-z0-9]/', $eType)) {
			l::fatal(26);
		}
		$eType = strtolower($eType);

		// Get the path
		$ePath = config::$main['scriptdir'] . "/lib/engines/$eType.inc";

		// Load the engine
		if (!file_exists($ePath)) {
			l::fatal(27, "Unhandled engine type: $eType");
		} elseif (!include($ePath)) {
			l::fatal(28, "Could not open engine file '$ePath'");
		} elseif (!class_exists('display_engine')) {
			l::fatal(29, "Invalid engine file for $eType");
		} else {
			$engine = new display_engine();
		}

		// Check for maintenance mode
		if (is_array(config::$main['maintenance'])) {
			$engine->maintenance(config::$main['maintenance']);
			exit(0);
		}

		engine::$e = $engine;
		l::setFatalHandler('engine::fatalError');
	}

	static function fatalError($errno, $errorText, $information) {
		die(engine::$e->displayFatalError($errno, $errorText, $information));
	}
}


?>

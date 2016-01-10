<?php

/********************************
 * LEGACY WORLDS - TICK MANAGER *
 ********************************/

/* Checks for fork() */
if (!(function_exists('pcntl_fork') || $__runFromAdmin)) {
	die("This script may not be called from the web server.\n");
}

/* If we're running from an administration script, go to the appropriate directory */
if ($__runFromAdmin) {
	$oldDir = getcwd();
	chdir(dirname(__FILE__));
}

/* Load the required libraries */
$__logPrefix = "lwTicks";
$__loader = array(
	'log', 'classloader',
	'version', 'game', 'tick', 'config',
	'db_connection', 'db_accessor', 'db_copy', 'db',
	'pcheck', 'library', 'tick_manager',
);
require_once("loader.inc");


if ($__runFromAdmin) {
	/* If we're being executed from an administration script,
	 * we need to manually run a tick on a game.
	 */
	list($gName, $tName) = $__adminParams;
	l::setSyslogPrefix("lwControl");

	function __adminFatalError($errno, $errorText, $information) {
		foreach ($information as $it) {
			$errorText .= "<br/>$it";
		}

		$bt = debug_backtrace();
		array_shift($bt);		// Remove this function
		array_shift($bt);		// Remove eval in lib/log.inc

		$errorText .= "<br/><u>Backtrace to the error:</u><pre>";
		$base = dirname(config::$main['scriptdir']);
		foreach ($bt as $data) {
			$str = "... " . str_repeat(' ', strlen($data['class']) > 30 ? 1 : (31 - strlen($data['class'])))
				. $data['class'] . " :: " . $data['function'];
			if (!is_null($data['file'])) {
				$str .= str_repeat(' ', strlen($data['function']) > 25 ? 1 : (26 - strlen($data['function'])));
				$fn = preg_replace("#^$base/#", "", $data['file']);
				$str .= "  (line {$data['line']}, file '$fn')";
			}
			$errorText .= "$str\n";
		}
		$errorText .= "</pre>";

		throw new Exception($errorText);
	}

	l::setFatalHandler('__adminFatalError');
	try {
		dbConnect();

		$game = config::getGame($gName);
		if (is_null($game)) {
			throw new Exception("Game '$gName' not found");
		}

		l::notice("administration script executing tick {$gName}::{$tName}");
		$game->getDBAccess();
		$game->runTick($tName, true);
		l::notice("tick {$gName}::{$tName} executed");

		dbClose();
	} catch (Exception $e) {
		$argh = $e->getMessage();
	}
	chdir($oldDir);
	return;
} else {
	l::setSyslogPrefix("lwTicks");
	if (count($argv) > 1) {
		/* Checks for command line arguments */
		if ($argv[1] == "-r" && count($argv) == 4) {
			$game = $argv[2];
			$tick = $argv[3];

			dbConnect();

			$game = config::getGame($argv[2]);
			if (is_null($game)) {
				die("Error: game {$argv[2]} not found");
			}
			l::notice("manually executing {$argv[2]}::{$argv[3]}");
			$game->getDBAccess();
			$game->runTick($argv[3], true);
			l::notice("{$argv[2]}::{$argv[3]} executed");

			dbClose();
			exit(0);
		} elseif ($argv[1] != "-d") {
			die("Syntax: {$argv[0]}\n\t  -> to run as a daemon\n\t{$argv[0]} -d\n\t  -> to run in debugging mode\n\t{$argv[0]} -r <game> <tick>\n\t  -> to run a tick manually\n");
		}
	} else {
		/* Starts the main thread */
		new tick_manager($argv[1] == "-d");
	}

}

?>

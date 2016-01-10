<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// proxycheck.php
//
// This is the main script for the proxy detector.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


/* Checks for fork() */
if (!(function_exists('pcntl_fork') || $__runFromAdmin)) {
	die("This script may not be called from the web server.\n");
}

/* Load the required libraries */
$__loader = array(
	'log', 'classloader',
	'version', 'game', 'tick', 'config',
	'db_connection', 'db_accessor', 'db_copy',
	'pcheck_thread', 'pcheck_manager', 'pcheck'
);
require_once("loader.inc");


l::setSyslogPrefix("lwProxy");

if (count($argv) > 1) {
	/* Checks for command line arguments */
	if (count($argv) > 2 && ($argv[1] == '-c' || $argv[1] == '-f')) {
		$addresses = $argv;
		array_shift($addresses);
		array_shift($addresses);

		print "Running manual check, please wait ...\n";
		try {
			$results = pcheck::check($addresses, $argv[1] == '-f');
		} catch (Exception $e) {
			print "{$argv[0]}: " . $e->getMessage() . "\n";
			exit(1);
		}
		foreach ($results as $address => $proxy) {
			print "\t$address - ";
			switch ($proxy) {
				case -1: print "detection failed"; break;
				case 0: print "no proxy detected"; break;
				case 1: print "OPEN PROXY DETECTED!"; break;
			}
			print "\n";
		}

		exit(0);

	} elseif ($argv[1] != "-d") {
		die("Syntax: {$argv[0]}\n\t  -> to run as a daemon\n\t{$argv[0]} -d\n\t  -> to run in debugging mode\n\t{$argv[0]} -c ip [ip [...]]\n\t  -> to check addresses for open proxies\n\t{$argv[0]} -f ip [ip [...]]\n\t  -> same as -c, but doesn't check for a running server\n");
	}
}

/* Starts the main thread */
@posix_setgid(33);
new pcheck_manager($argv[1] == "-d");

?>

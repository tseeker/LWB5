<?php

// This file contains the main configuration for a LegacyWorlds game
// engine instance

$config = array(
	// Database connection parameters
	"dbhost"	=> "localhost",
	"dbuser"	=> "legacy",
	"dbpass"	=> "",
	"dbname"	=> "legacy",

	// Path and URL to static contents
	"staticdir"	=> dirname(__FILE__) . "/../site/static",
	"staticurl"	=> "http://www.legacyworlds.com/static",

	// Path to game scripts
	"scriptdir"	=> dirname(__FILE__),

	// Path to the cache
	"cachedir"	=> "/tmp/lwcache",

	// Debugging level
	"debug"		=> 2,

	// Maintenance mode
	"maintenance"	=> null,
/*
	"maintenance"	=> array(
				// Hour/Minutes/Seconds/Month/Day/Year
		"until"		=> mktime(10, 20, 0, 1, 11, 2007),
		"reason"	=> "Upgrading the MySQL server."
	),
*/

	// Mac widget version numbers and location
	"latestWidget"	=> 1,
	"oldestWidget"	=> 1,
	"widgetURL"	=> "http://www.legacyworlds.com/downloads/LegacyWorlds-Dashboard-latest.zip",

	// Version numbers to make us feel good
	"v_engine"	=> "0.85a",
	"v_game"	=> "Beta 5",
	"v_rev"		=> "2284",

	// Control script fifo and directory
	"cs_fifo"	=> "/tmp/.lwFifo",
	"cs_path"	=> "/tmp/.lwControl",

	// Trace users?
	"trace"		=> array(),

	// Do we need to actually send emails?
	"sendmails"	=> false
);

if (file_exists($config['cachedir'] . "/maintenance.ser")) {
	$__maintenanceFile = fopen($config['cachedir'] . "/maintenance.ser", "r");
	$config['maintenance'] = unserialize(fgets($__maintenanceFile));
	fclose($__maintenanceFile);
}

?>

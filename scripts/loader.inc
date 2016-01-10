<?php

/****************************************************************************
 * LOADER
 ****************************************************************************/

/* Load configuration file */
if (!include_once('config.inc')) {
	if (include_once("log.inc")) {
		l::fatal(0);
	} else {
		die("Unhandled error.");
	}
}

/* Include library files */
if (is_array($__loader)) {
	foreach ($__loader as $lf) {
		if (!include_once($config['scriptdir'] . "/lib/$lf.inc")) {
			if (function_exists('fatalError')) {
				l::fatal(25, "Library file not found: $lf");
			} else {
				die("Unhandled error");
			}
		}
	}
}

?>

<?php

/****************************************************************************
 * HELPER FUNCTIONS
 ****************************************************************************/

function tooltip($n) {
	return " onmouseover=\"return escape('".utf8entities($n,ENT_QUOTES)."')\"";
}

/** This function checks the static elements directory for a specified path.
 * If the path is found to exist, an URL to this path is returned, otherwise
 * a null value is returned.
 */
function getStatic($path) {
	$f = config::$main['staticdir'] . "/$path";
	if (!file_exists($f)) {
		return null;
	}
	return config::$main['staticurl'] . "/$path";
}


/** This function generates a link to a script page. It accepts either one or
 * two arguments: a page name and an optional version identifier. If the
 * version identifier isn't specified, the current version is used.
 */
function makeLink($p, $v = null, $e = null) {
	if (is_null($v)) {
		$v = display_engine::$version;
	}
	$es = is_null($e) ? "" : ".$e";
	return $_SERVER['SCRIPT_NAME'] . "/$v/$p$es";
}


/** This function terminates request processing by storing session and
 * tracking information. It then exits the script.
 */
function endRequest($quit = true) {
	session::store();
	tracking::store();
	dbEnd();
	if ($quit) {
		exit(0);
	}
}


/** This function identifies the directory containing the lay-out scripts for
 * the current version+theme combination, checks for its existence (the script
 * bails out if it doesn't exist) and returns it.
 */
function getLayoutDirectory($version) {
	$d = config::$main['scriptdir'] . "/site/$version/layout";
	if ($version != 'main') {
		$t = prefs::get("$version/theme");
		if ($t == "") {
			$t = "default";
		}
		$d .= "/$t";
	}

	if (!file_exists($d)||!is_dir($d)) {
		l::fatal(17, "Directory '$d' for version '$version'");
	}

	return $d;
}


/** This function determines the current language from either:
 * 1) the user's preferences it they are available,
 * 2) tracking data if a user has already logged in,
 * 3) the default value, "en".
 * If user preferences are currently available, tracking data is modified
 * accordingly.
 */
function getLanguage() {
	$l = prefs::get('main/language');
	if ($l != "") {
		if ($_SESSION['authok']) {
			tracking::$data['language'] = $l;
		}
		return $l;
	} elseif ($_SESSION['authok']) {
		return "en";
	}

	$l = tracking::$data['language'];
	if ($l != "") {
		return $l;
	}
	return "en";
}


/** This function acts like utf8entities, except for the fact that it uses
 * UTF-8 by default.
 */
function utf8entities($string, $mode = ENT_COMPAT) {
	return htmlentities($string, $mode, 'UTF-8');
}


/****************************************************************************
 * PAGE OUTPUT FUNCTIONS
 ****************************************************************************/

function handleInput() {
	engine::$e->initSession();
	engine::$e->checkAuth();

	if (is_array($_SESSION) && is_array(config::$main['trace'])
			&& in_array($_SESSION['userid'], config::$main['trace'])) {

		$path = input::$path;
		$page = input::$page;
		ob_start();
		print "REQUEST AT $path/$page." . input::$eType . "\n";
		print "==========================================\n";
		print "Time: " . gmstrftime("%Y-%m-%d %H:%M:%S", time()) . "\n\n";
		print "SESSION DATA\n";
		print_r($_SESSION);
		print "\n\nINPUT:";
		print_r(input::$input);
		print "\n\n";
		$log = ob_get_contents();
		$logFile = fopen("/tmp/trace-{$_SESSION['userid']}" . gmstrftime("-%Y-%m-%d", time()) . ".log", "a");
		fwrite($logFile, $log);
		fclose($logFile);
		ob_end_clean();
	}

	engine::$e->handleInput();
	engine::$e->initRPC();
	engine::$e->outputData();
	endRequest();
}

?>

<?php

/** This function writes an entry to the system log. */
function __logAdmin($txt, $level = null) {
	static	$logInit = false;
	if (!$logInit) {
		global $__logPrefix;

		define_syslog_variables();
		openlog("lwControl", LOG_PID, LOG_USER);
		$logInit = true;
	}
	if (is_null($level)) {
		$level = LOG_INFO;
	}
	syslog($level, "*** ADMIN {$_SERVER['PHP_AUTH_USER']} $txt");
}


?>

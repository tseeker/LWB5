<?php


/****************************************************************************
 * LOG AND FATAL ERRORS
 ****************************************************************************/

class l {

	/** This variable should contain the name of the function to call when
	 * handling fatal errors.
	 */
	private static $fatalErrorCallback = "l::defaultFatalError";

	/** This array lists the errors the engine can encounter and handle
	 * through the fatalError() function.
	 */
	private static $engineErrors = array(
		0	=> "Could not open configuration file",
		1	=> "Could not connect to database",
		2	=> "Failed to set up tracking data",
		3	=> "Failed to set up tracking data",
		4	=> "Failed to set up tracking data",
		5	=> "Failed to set up tracking data",
		6	=> 'Invalid request',
		7	=> 'Invalid request',
		8	=> 'Page not found',
		9	=> 'Page not found',
		10	=> 'Internal error',
		11	=> 'Internal error',
		12	=> "Failed to set up session data",
		13	=> "Failed to set up session data",
		14	=> "Failed to set up session data",
		15	=> "Failed to set up session data",
		16	=> 'Internal error',
		17	=> 'Internal error',
		18	=> "Internal error",
		19	=> 'Internal error',
		20	=> 'Internal error',
		21	=> 'Internal error',
		22	=> 'Internal error',
		23	=> 'Internal error',
		24	=> 'Internal error',
		25	=> 'Internal error',
		26	=> 'Invalid extension',
		27	=> 'Unhandled extension',
		28	=> 'Internal error',
		29	=> 'Internal error',
		30	=> 'Resource not found',
		31	=> 'Unhandler extension',
	);


	/** This variable indicates whether the logging system has
	 * been initialised.
	 */
	private static $initialised = false;


	/** This variable contains the prefix to use when writing
	 * to syslog.
	 */
	private static $syslogPrefix = "lwEngine";


	/** This variable defines a text that is used as a prefix
	 * when logging strings.
	 */
	private static $prefix = "";


	/** This variable prevents multiple "deprecated" entries from
	 * being logged.
	 */
	private static $deprecatedLogged = false;


	/** This variable prevents multiple "FIXME" entries from
	 * being logged.
	 */
	private static $fixmeLogged = false;


	/** This function is the default fatal error display function. */
	private function defaultFatalError($errno, $error, $info) {
		ob_start();
?>
<html>
 <head>
  <title>Legacy Worlds</title>
 </head>
 <body>
  <p>
   <b>The LegacyWorlds game engine encountered a fatal error:</b><br/>
   <?=$error?><br/>
   Please report this error by sending an e-mail to the staff at
   <a href='mailto:webmaster@legacyworlds.com'>webmaster@legacyworlds.com</a>
   explaining what happened.
  </p>
 </body>
</html>
<?php
		$html = ob_get_contents();
		ob_end_clean();
		die($html);
	}


	/** This method initialises communications with syslog.
	 */
	private static function init() {
		if (self::$initialised) {
			return;
		}

		define_syslog_variables();
		openlog(self::$syslogPrefix, LOG_PID, LOG_USER);
		self::$initialised = true;
	}


	/** This method writes a string to the system log.
	 */
	private static function __write($txt, $level) {
		syslog($level, self::$prefix . $txt);
	}


	/** This method writes a string to the system log;
	 * it is used by the external "logText" function and
	 * should disappear in time.
	 */
	public static function write($txt, $level) {
		self::init();
		if (is_null($level)) {
			$level = LOG_INFO;
		}
		self::__write($txt, $level);
	}


	/** This method displays one of the game engine's
	 * fatal errors and adds log entries accordingly.
	 */
	public function fatal($errno, $information = null) {
		// Log the error
		$errorText = self::$engineErrors[$errno] . " [" . sprintf("%03d", $errno) . "]";
		self::critical($errorText);

		// Log additional information
		if (is_null($information)) {
			$information = array();
		} elseif (!is_array($information)) {
			$information = array($information);
		}

		foreach ($information as $it) {
			self::info($it);
		}

		// Log additional details
		if (class_exists('input') && !is_null(input::$page)) {
			self::debug("Request: " . input::$path . "/" . input::$page . "." . input::$eType);

			if (tracking::$id != '') {
				$txt = "tracking: " . tracking::$id . " (" . tracking::$dbId . ")";
				if (session::$id != '') {
					$txt .= "; session: " . session::$id . " (" . session::$dbId . ")";
					if (!is_null($_SESSION['userid'])) {
						$txt .= "; user {$_SESSION['login']} ({$_SESSION['userid']})";
					}
				}
				$txt .= "; user agent: {$_SERVER['HTTP_USER_AGENT']}";
				self::debug($txt);
			}
		}
		self::backtrace();

		eval(self::$fatalErrorCallback . '($errno, $errorText, $information);');
	}


	/** This method writes a backtrace to the log, removing all
	 * entries from the logging class.
	 */
	public function backtrace() {
		$bt = debug_backtrace();
		while (is_array($bt[1]) && $bt[1]['class'] == 'l') {
			array_shift($bt);
		}

		self::info("Backtrace to the error:");
		$base = dirname(config::$main['scriptdir']);
		foreach ($bt as $data) {
			$cnLength = strlen($data['class']);
			$str = "... " . str_repeat(' ', $cnLength > 30 ? 1 : (31 - $cnLength)) . $data['class']
				. " :: " . $data['function'];
			if (!is_null($data['file'])) {
				$cnLength = strlen($data['function']);
				$fn = preg_replace("#^$base/#", "", $data['file']);
				$str .= str_repeat(' ', $cnLength > 25 ? 1 : (26 - $cnLength))
					. "  (line {$data['line']}, file '$fn')";
			}
			self::info($str);
		}
	}


	/** This method changes the syslog prefix.
	 */
	public function setSyslogPrefix($prefix) {
		self::$syslogPrefix = $prefix;
		if (self::$initialised) {
			closelog();
			self::$initialised = false;
		}
	}

	/** This method changes the string prefix.
	 */
	public function setPrefix($prefix) {
		self::$prefix = "$prefix ";
	}

	/** This method changes the function to call
	 * for fatal errors.
	 */
	public function setFatalHandler($function) {
		self::$fatalErrorCallback = $function;
	}

	/** This function logs the use of a deprecated
	 * function call and prevents further logging of
	 * similar occurences.
	 */
	public function deprecated($function) {
		if (config::$main['debug'] == 2 && !self::$deprecatedLogged) {
			l::trace("DEPRECATED: $function");
			l::backtrace();
			self::$deprecatedLogged = true;
		}
	}

	/** This function logs FIXME's.  */
	public function FIXME($text) {
		if (config::$main['debug'] >= 1 && !self::$fixmeLogged) {
			l::debug("FIXME: $text");
			if (config::$main['debug'] == 2) {
				l::backtrace();
			}
			self::$fixmeLogged = true;
		}
	}


	/******************* LOGGING METHODS ************************/
	/* These methods should replace logText() wherever possible */
	/************************************************************/
	public function crit($txt) { self::init(); self::__write($txt, LOG_CRIT); }
	public function critical($txt) { self::init(); self::__write($txt, LOG_CRIT); }
	public function error($txt) { self::init(); self::__write($txt, LOG_ERR); }
	public function warn($txt) { self::init(); self::__write($txt, LOG_WARNING); }
	public function warning($txt) { self::init(); self::__write($txt, LOG_WARNING); }
	public function notice($txt) { self::init(); self::__write($txt, LOG_NOTICE); }
	public function info($txt) { self::init(); self::__write($txt, LOG_INFO); }
	public function debug($txt) {
		if (config::$main['debug'] >= 1) {
			self::init(); self::__write($txt, LOG_DEBUG);
		}
	}
	public function trace($txt) {
		if (config::$main['debug'] == 2) {
			self::init(); self::__write($txt, LOG_DEBUG);
		}
	}
}



/** This function writes an entry to the system log. */
function logText($txt, $level = null) {
	l::deprecated("logText()");
	l::write($txt, $level);
}


/** This function displays one of the game engine's fatal errors and adds
 * log entries accordingly.
 */
function fatalError($errno, $information = null) {
	l::deprecated("fatalError($errno)");
	l::fatal($errno, $information);
}

?>

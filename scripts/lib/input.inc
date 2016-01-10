<?php

/****************************************************************************
 * INPUT DATA GATHERING FUNCTIONS
 ****************************************************************************/


class input {

	static $input	= array();

	static $path	= null;
	static $page	= null;
	static $eType	= null;
	static $game	= null;

	static $IE	= null;
	static $safari	= null;

	/** This function analyzes the request to find out which "sub-version" is being
	 * accessed, and which page should be displayed.
	 */
	static function identify() {
		$p = $_SERVER["PATH_INFO"];

		if (preg_match('/\.[a-z0-9]{2,10}$/', $p)) {
			input::$eType = preg_replace('/^.*\.([a-z0-9]{2,10})$/', '\1', $p);
			$p = preg_replace('/\.[a-z0-9]{2,10}$/', '', $p);
		} else {
			input::$eType = null;
		}

		if ($p == "") {
			input::$path = 'main';
			input::$page = 'index';
		} elseif (preg_match('/[^A-Za-z0-9\/]/', $p)) {
			l::notice("Invalid path requested: '$p'");
			l::debug("Referer was '{$_SERVER['HTTP_REFERER']}'");
			input::$path = 'main';
			input::$page = 'notfound';
			input::$eType = null;
			input::$game = config::getGame('main');
		} else {
			$p = preg_replace(
				array('/\/+/',	'/\/$/',	'/^\//'),
				array('/',	'',		''),
				$p
			);
			$tmp = explode('/', $p);
			if (count($tmp) > 2) {
				l::notice("Invalid path requested: '$p'");
				l::debug("Referer was '{$_SERVER['HTTP_REFERER']}'");
				input::$path = 'main';
				input::$page = 'notfound';
				input::$eType = null;
				input::$game = config::getGame('main');
			} else {
				if (count($tmp) == 1) {
					input::$path = 'main';
				} else {
					input::$path = array_shift($tmp);
				}
				input::$page = $tmp[0];
			}
		}

		// Find the game instance for this game
		if (!config::hasGame(input::$path)) {
			l::notice("Requested path '" . input::$path . "' doesn't match any game");
			l::debug("Referer was '{$_SERVER['HTTP_REFERER']}' ; requested page was '" . input::$page . "'");
			input::$path = 'main';
			input::$page = 'notfound';
			input::$eType = null;
			input::$game = config::getGame('main');
		} else {
			input::$game = config::getGame(input::$path);
		}

		l::setPrefix(input::$path . "/" . input::$page . (is_null(input::$eType) ? "" : ( "." . input::$eType)));

		return array(input::$path, input::$page, input::$eType);
	}


	/** This function reads the arguments in the $_POST variable, then in the $_GET
	 * variable, and stores them inside an hash table. If PHP's crappy "magic
	 * quotes" are enabled, remove them.
	 * NOTE: This behaviour should be reversed in a production version.
	 */
	function read() {
		input::$IE = preg_match('/MSIE/', $_SERVER['HTTP_USER_AGENT'])
			&& !preg_match('/Opera/', $_SERVER['HTTP_USER_AGENT']);
		input::$safari = preg_match('/AppleWebKit/', $_SERVER['HTTP_USER_AGENT']);

		$p = array();
		foreach ($_POST as $k => $v) {
			$p[$k] = $v;
		}
		foreach ($_GET as $k => $v) {
			$p[$k] = $v;
		}
		if (get_magic_quotes_gpc()) {
			foreach ($p as $k => $v) {
				if (is_scalar($v)) {
					$p[$k] = stripslashes($v);
				} elseif (is_array($v)) {
					$p[$k] = array();
					foreach ($v as $ak => $av) {
						$p[$k][$ak] = stripslashes($av);
					}
				}
			}
		}
		input::$input = $p;
		return $p;
	}
}


?>

<?php


class ajax {

	static $method		= array();
	static $fHandler	= array();
	static $fTheme		= array();
	static $init		= "";

	function getTheme() {
		$f = getLayoutDirectory(input::$game->version->id) . "/ajax.inc";
		if (!file_exists($f)) {
			return array();
		}
		return include($f);
	}

	static function init() {
		$handler = handler::$h;

		// Get AJAX functions from handler
		$a1 = is_array($handler->ajax) ? $handler->ajax : array();
		if (is_array($a1['func'])) {
			ajax::$fHandler = $a1['func'];
			$ml = is_array($a1['method']) ? $a1['method'] : array();
			foreach ($a1['func'] as $f) {
				$m = ($ml[$f] == "") ? ($handler->ajaxPOST ? "POST" : "GET") : $ml[$f];
				ajax::$method[$f] = $m;
			}
		}

		// Get theme-specific AJAX functions
		$a2 = ajax::getTheme();
		if (is_array($a2['func'])) {
			ajax::$fTheme = $a2['func'];
			$ml = is_array($a2['method']) ? $a2['method'] : array();
			foreach ($a2['func'] as $f) {
				$m = ($ml[$f] == "") ? ($handler->ajaxPOST ? "POST" : "GET") : $ml[$f];
				ajax::$method[$f] = $m;
			}
		}

		// Create init string
		ajax::$init = $a2['init'] . $a1['init'];
	}


	static function canCall($function) {
		return in_array($function, ajax::$fTheme) || in_array($function, ajax::$fHandler);
	}

	static function call($function, $args) {
		// We don't want an error in that part of the code to make the
		// client-side JS script go insane
		ob_start();

		// Call the function
		if (in_array($function, ajax::$fTheme)) {
			$res = call_user_func_array("thm_{$function}", $args);
		} else if (in_array($function, ajax::$fHandler)) {
			$res = call_user_func_array(array(handler::$h, $function), $args);
		} else {
			$res = null;
		}

		// Log any error / warning / whatever
		$buffer = ob_get_contents();
		if ($buffer != '') {
			$b = explode("\n", $buffer);
			foreach ($b as $line) {
				if ($line != '') {
					l::warning("AJAX ($function): $line");
				}
			}
		}
		ob_end_clean();

		return $res;
	}
}


?>

<?php


/****************************************************************************
 * PREFERENCES MANAGEMENT FUNCTIONS
 ****************************************************************************/

class prefs {
	private static $prefs = null;

	/** This function transforms user_preferences database records into values
	 * stored inside $prefs.
	 */
	private static function fromBase($qr, $toTrack = false) {
		while ($r = dbFetchHash($qr)) {
			$v = $r['version'];
			$i = $r['id'];
			if (!is_array(prefs::$prefs[$v])) {
				prefs::$prefs[$v] = array();
			}
			prefs::$prefs[$v][$i] = $r['value'];

			if ($toTrack && $v == "main") {
				if (!is_array(tracking::$data['pref_override'])) {
					tracking::$data['pref_override'] = array();
				}
				tracking::$data['pref_override'][$i] = $r['value'];
			}
		}
	}


	/** This function initializes $prefs to an empty array
	 * then reads default preferences from the user_preferences table.
	 */
	private static function getDefaults() {
		prefs::$prefs = array();
		$qr = dbQuery("SELECT id,version,value FROM user_preferences WHERE account=0");
		if (!$qr || !count($qr)) {
			return;
		}
		prefs::fromBase($qr);
	}


	/** This function reads the current user's preferences from the base. */
	private static function loadUser() {
		$qr = dbQuery("SELECT id,version,value FROM user_preferences WHERE account={$_SESSION['userid']}");
		if (!$qr || !count($qr)) {
			return;
		}
		prefs::fromBase($qr, true);
	}


	/** This function loads preferences by loading default values and then, if
	 * an user is authenticated, his own preferences.
	 */
	private static function load() {
		prefs::getDefaults();
		if ($_SESSION['authok']) {
			prefs::loadUser();
		} elseif (is_array(tracking::$data['pref_override'])) {
			foreach (tracking::$data['pref_override'] as $pref => $value) {
				prefs::$prefs['main'][$pref] = $value;
			}
		}
	}


	/** This function sets user preferences. */
	static function set($path, $val) {
		if (!$_SESSION['authok']) {
			return false;
		}

		if (!is_array(prefs::$prefs)) {
			prefs::load();
		}

		list($ver, $pref) = explode('/', $path);
		$q = dbQuery("DELETE FROM user_preferences WHERE account={$_SESSION['userid']} AND id='$pref' AND version='$ver'");
		$q = dbQuery("INSERT INTO user_preferences VALUES('$pref','$ver',{$_SESSION['userid']},'".addslashes($val)."')");
		if ($q) {
			prefs::$prefs[$ver][$pref] = $val;
			return true;
		}
		return false;
	}

	/** This function returns a value from the preferences using a version/pref_name path.
	 */
	static function get($path, $default = null) {
		if (!is_array(prefs::$prefs)) {
			prefs::load();
		}

		list($version, $name) = explode('/', $path);
		if (is_array(prefs::$prefs[$version])) {
			$v = prefs::$prefs[$version][$name];
		} else {
			$v = null;
		}
		if (is_null($v) && !is_null($default)) {
			$v = $default;
		}
		return $v;
	}

}

?>

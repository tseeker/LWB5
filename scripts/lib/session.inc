<?php

/****************************************************************************
 * SESSION MANAGEMENT FUNCTIONS
 ****************************************************************************/

class session {

	static $cName	= "";
	static $id	= null;
	static $dbId	= null;
	static $new	= null;
	static $dataMD5	= null;

	/** This function generates a new session identifier. */
	private static function generateId() {
		// Create a new tracking cookie if none exists 
		do {
			$v = md5(uniqid(rand()));
			$q  = "SELECT id FROM web_session WHERE cookie = '$v'";
			$qr = dbQuery($q);
		} while ($qr && dbCount($qr));

		// If $qr is null, something went wrong
		if (!$qr) {
			l::warning('No query result in session::generateId()');
			return null;
		}
		return $v;
	}


	/** This function reads the session cookie from the user's browser or generates
	 * a value for the session cookie if none is found.
	 */
	private static function readId($create) {
		// Get session identifier from cookie if it exists
		if (isset($_COOKIE[self::$cName]) && ctype_alnum($_COOKIE[self::$cName]) && strlen($_COOKIE[self::$cName]) == 32) {
			$v = pg_escape_string($_COOKIE[self::$cName]);
			$q  = "SELECT id FROM web_session WHERE cookie='$v' AND tracking=" . tracking::$dbId;
			$qr = dbQuery($q);
			if ($qr && dbCount($qr) == 1) {
				return array($v, false);
			} else {
				l::notice("Session '$v' not found");
				$create = true;
			}
		}
		return $create ? array(self::generateId(), true) : array(null, false);
	}


	/** This function inserts session data into the database's web_session table. */
	static private function createData() {
		$q  = "INSERT INTO web_session(cookie,ip_addr,tracking,stored_data) VALUES ('"
			. self::$id . "','" . $_SERVER['REMOTE_ADDR'] . "'," . tracking::$dbId . ",'a:0:{}')";
		return dbQuery($q);
	}


	/** This function updates a session's last access timestamp. */
	static private function updateAccess() {
		$q = "UPDATE web_session SET last_used=unix_timestamp(now()) WHERE cookie='" . self::$id . "'";
		return dbQuery($q);
	}


	/** This function reads tracking data from the web_tracking table and stores it
	 * in the $_SESSION autoglobal variable.
	 */
	static private function readData() {
		$q  = "SELECT stored_data,id FROM web_session WHERE cookie = '" . self::$id . "'";
		$qr = dbQuery($q);
		if (!$qr || dbCount($qr) != 1) {
			return false;
		}

		$tmp = dbFetchArray($qr);
		$_SESSION = unserialize($tmp[0]);
		self::$dataMD5 = md5($tmp[0]);
		self::$dbId = $tmp[1];
		return is_array($_SESSION);
	}


	/** This function updates the session record in the web_session table using
	 * the serialized contents of $_SESSION.
	 */
	static function store() {
		if (is_null(self::$dbId)) {
			return true;
		}
		$ser = serialize($_SESSION);
		if ($ser != self::$dataMD5) {
			$txt = pg_escape_string($ser);
			$q = "UPDATE web_session SET stored_data='$txt' WHERE id=" . self::$dbId;
			return dbQuery($q);
		}
		return true;
	}


	/** This function terminates the current session and removes the record from
	 * the web_session table. If an user was authenticated, accountLog() is
	 * called to account for the fact that the user has "logged out".
	 */
	static function kill() {
		if (is_null(self::$dbId)) {
			return;
		}

		if ($_SESSION['userid'] != "") {
			dbQuery("UPDATE account SET last_logout=".time()." WHERE id={$_SESSION['userid']}");
			accountLog('o', $_SESSION['userid']);
		}
		$_SESSION = array();

		setcookie(self::$cName, false, 0, dirname($_SERVER['SCRIPT_NAME']));
		dbQuery("DELETE FROM web_session WHERE id=".self::$dbId);
		self::$dbId = self::$id = null;
	}


	/** This method sets the account identifier associated with a session. */
	static function setAccount($account) {
		if (is_null($account)) {
			$account = "NULL";
		}
		dbQuery("UPDATE web_session SET account=$account WHERE id=" . self::$dbId);
	}


	/** This function manages the session, creating it if it is required or
	 * reading it from the base.
	 */
	static function handle($create) {
		self::$cName = config::getParam('sessname');

		// If tracking is disabled, return
		if (tracking::$disabled) {
			return 1;
		}

		// Get or generate session identifier
		if (!tracking::$new) {
			list($sessId, $sessNew) = self::readId($create);
		} elseif ($create) {
			list($sessId, $sessNew) = array(self::generateId(), true);
		}

		// If no session identifier has been returned, end.
		if (is_null($sessId)) {
			return 2;
		}

		self::$id	= $sessId;
		self::$new	= $sessNew;

		// Create or update session record
		if ($sessNew && !self::createData()) {
			l::fatal(13, "Session ID was $sessId");
		} elseif (!$sessNew && !self::updateAccess()) {
			l::fatal(14, "Session ID was $sessId");
		}

		// Read session data
		if (!self::readData()) {
			l::fatal(15, "Session ID was $sessId");
		}

		// If no user is currently authenticated and no
		// authentication is required, terminate the session
		if (!($create || $_SESSION['authok'])) {
			self::kill();
			return 0;
		}

		// Sets session cookie
		setcookie(self::$cName, $sessId, 0, dirname($_SERVER['SCRIPT_NAME']));
		return 0;
	}
}


?>

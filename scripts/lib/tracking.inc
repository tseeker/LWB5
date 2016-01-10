<?php

/****************************************************************************
 * TRACKING FUNCTIONS
 ****************************************************************************/


class tracking {

	static $cName		= "";
	static $disabled	= false;
	static $id		= null;
	static $dbId		= null;
	static $data		= array();
	static $new		= false;
	static $dataMD5;

	/** This function reads the tracking cookie from the user's browser or generates
	 * a value for the tracking cookie if none is found.
	 */
	private static function readId() {
		// Get tracking identifier from cookie if it exists
		if (isset($_COOKIE[tracking::$cName]) && ctype_alnum($_COOKIE[tracking::$cName]) && strlen($_COOKIE[tracking::$cName]) == 32) {
			$v = $_COOKIE[tracking::$cName];
			$q  = "SELECT id FROM web_tracking WHERE cookie = '$v'";
			$qr = dbQuery($q);
			if (!$qr) {
				return array(null, false);
			}
			if (dbCount($qr) == 1) {
				return array($v, false);
			}
		}

		// Create a new tracking cookie if none exists 
		do {
			$v = md5(uniqid(rand()));
			$q  = "SELECT id FROM web_tracking WHERE cookie = '$v'";
			$qr = dbQuery($q);
		} while ($qr && dbCount($qr));

		// If $qr is null, something went wrong
		if (!$qr) {
			l::debug("tracking: query failed for ID '$v'");
			return array(null, false);
		}

		return array($v, true);
	}


	/** This function inserts tracking data into the database's web_tracking table. */
	private static function createData() {
		// Look for tracking records from the same IP/agent combination in the last
		// 10 minutes that have been used for less than 10 seconds
		$q = "SELECT COUNT(*) FROM web_tracking WHERE ip_addr = '" . $_SERVER['REMOTE_ADDR']
			. "' AND browser = '".addslashes($_SERVER['HTTP_USER_AGENT'])."'"
			. " AND UNIX_TIMESTAMP(now()) - created < 600"
			. " AND last_used - created < 10";
		$r = dbQuery($q);
		list($c) = dbFetchArray($r);

		// If more than 5 recent unused records are found, tracking is disabled.
		if ($c > 5) {
			tracking::$disabled = true;
			return true;
		}

		$q  = "INSERT INTO web_tracking(cookie,created,last_used,ip_addr,"
		    . "browser,stored_data) VALUES ('" . tracking::$id
		    . "',unix_timestamp(now()),unix_timestamp(now()),'".$_SERVER['REMOTE_ADDR']."','"
		    . addslashes($_SERVER['HTTP_USER_AGENT']) . "',"
		    . "'a:0:{}')";
		return dbQuery($q);
	}


	/** This function updates a tracking entry's last access timestamp. */
	private static function updateAccess() {
		$q = "UPDATE web_tracking SET last_used=unix_timestamp(now()) WHERE cookie='" . tracking::$id . "'";
		return dbQuery($q);
	}


	/** This function reads tracking data from the web_tracking table and stores it
	 * in the tracking::$data variable.
	 */
	private static function readData() {
		$trackId = tracking::$id;

		$q  = "SELECT stored_data,id FROM web_tracking WHERE cookie = '$trackId' FOR UPDATE";
		$qr = dbQuery($q);
		if (!$qr || dbCount($qr) != 1) {
			l::notice("Tracking data not found for cookie '$trackId'");
			return false;
		}

		$tmp = dbFetchArray($qr);
		$trackData = unserialize($tmp[0]);
		$trackDBId = $tmp[1];
		if (!is_array($trackData)) {
			// Make sure we delete the tracking data that caused the problem
			l::notice("Invalid tracking data for '$trackId'");
			l::debug("DB id= $trackDBId, data type= '" . gettype($trackData) . "'");
			l::info("Moving entry out of the way");
			//dbQuery("DELETE FROM web_tracking WHERE id=$trackDBId");
			dbQuery("UPDATE web_tracking SET cookie='DISABLED $trackDBId' WHERE id=$trackDBId");
			dbEnd();
		} else {
			tracking::$dbId = $trackDBId;
			tracking::$data = $trackData;
			tracking::$dataMD5 = md5($tmp[0]);
		}

		return is_array($trackData);
	}


	/** This function initializes the tracking system */
	static function init() {
		tracking::$cName = config::getParam('trackname');

		list($trackId,$trackNew) = tracking::readId();
		if (handler::$h->noTracking && (is_null($trackId) || $trackNew)) {
			tracking::$disabled = true;
			return;
		}

		if (is_null($trackId)) {
			l::fatal(2);
		}

		tracking::$id	= $trackId;
		tracking::$new	= $trackNew;

		if ($trackNew && !tracking::createData()) {
			l::fatal(3);
		} elseif (!$trackNew && !tracking::updateAccess()) {
			l::fatal(4);
		}

		if (tracking::$disabled) {
			return;
		}

		if (tracking::readData()) {
			setcookie(tracking::$cName, $trackId, time() + 31536000, dirname($_SERVER['SCRIPT_NAME']));
		} else {
			$trackDBId = tracking::$dbId;
			l::fatal(5, "Tracking data: ID='$trackId',DB ID=$trackDBId" . ($trackNew ? ",new" : ""));
		}
	}


	/** This function updates the web_tracking table using the serialized contents
	 * of tracking::$data
	 */
	static function store() {
		if (is_null(tracking::$dbId)) {
			if (tracking::$disabled) {
				return 1;
			}
			l::warn("storeTrackingData: database identifier is null");
			return 1;
		}

		$serialized = serialize(tracking::$data);
		if (self::$dataMD5 != md5($serialized)) {
			$txt = pg_escape_string(serialize(tracking::$data));
			$q = "UPDATE web_tracking SET last_used=unix_timestamp(now()),stored_data='$txt' WHERE id='" . tracking::$dbId . "'";
		} else {
			$q = "UPDATE web_tracking SET last_used=unix_timestamp(now()) WHERE id='" . tracking::$dbId . "'";
		}
		return dbQuery($q);
	}
}


?>

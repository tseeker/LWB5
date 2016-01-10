<?php

//---------------------------------------------------------------
// "Session tick": removes outdated sessions and tracking data
// Executed once per minute, starting from 7:00:40 ST
//---------------------------------------------------------------

class main_ticks_session_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->accounts	= $this->lib->game->getLib("main/account");
	}


	public function runTick() {
		$this->db->safeTransaction(array($this, 'removeTracking'));
		$this->db->safeTransaction(array($this, 'removeSessions'));
	}


	public function removeTracking() {
		// Removes old tracking data (> 3 months or > 1 day and last used within a minute of creation)
		$this->db->query(
			"UPDATE web_tracking SET stored_data = '' "
			. "WHERE UNIX_TIMESTAMP(NOW()) - last_used > 7776000"
		);
		$this->db->query(
			"DELETE FROM web_tracking "
			. "WHERE (UNIX_TIMESTAMP(NOW()) - last_used > 86400 AND last_used - created < 60)"
		);
	}

	public function removeSessions() {
		// Removes old sessions
		$q = $this->db->query(
			"SELECT s.id, t.id, a.id FROM web_session s, web_tracking t, account a "
				. "WHERE UNIX_TIMESTAMP(NOW()) - s.last_used > 3600 "
				  . "AND a.id = s.account "
				  . "AND t.id = s.tracking "
				. "FOR UPDATE OF s, t, a"
		);
		if (! dbCount($q)) {
			return;
		}

		$accounts = array();
		while ($r = dbFetchArray($q)) {
			list($sid, $tid, $uid) = $r;
			if (is_null($uid)) {
				continue;
			}

			$this->accounts->call('log', $uid, 'o');
			array_push($accounts, $uid);
			l::info("Player #$uid logged out automatically");
		}

		$this->db->query(
			"UPDATE account SET last_logout = UNIX_TIMESTAMP(NOW()) "
				. "WHERE id IN (" . join(',', $accounts) . ")"
		);
		$this->db->query("DELETE FROM web_session WHERE UNIX_TIMESTAMP(NOW()) - last_used > 3600");
	}
}

?>

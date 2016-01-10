<?php

//---------------------------------------------------------------
// "Day tick": adds vacation credits to registered players who
//	have less than 240 credits (2 months), deletes old logs,
//	warn inactive players or delete their accounts
// Executed once per day at 07:00 ST
//---------------------------------------------------------------

class main_ticks_day_library {

	function main_ticks_day_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->main	= $this->lib->game->getLib();
		$this->accounts	= $this->lib->game->getLib("main/account");
	}

	function runTick() {
		// Delete logs and update vacation credits
		$this->db->safeTransaction(array($this, "updateStatus"));
		// Manage inactive / leaving accounts
		$this->db->safeTransaction(array($this, "terminateAccounts"));

		// Send mails if needed
		foreach ($this->mails as $mail) {
			if ($this->main->call('sendMail', $mail[0], $mail[1], $mail[2])) {
				l::info($mail[3]);
			} else {
				l::warn("Failed to send mail {$mail[0]} to {$mail[1]}");
			}
		}
	}

	public function updateStatus() {
		// Delete old logs
		$this->db->query(
			"DELETE FROM account_log "
				. "WHERE UNIX_TIMESTAMP(NOW())-t>3600*24*60 AND action IN ('IN','OUT')"
		);

		// Add vacation credits
		$this->db->query(
			"UPDATE account SET vac_credits=vac_credits + 1 "
				. "WHERE status='STD' AND vac_credits<240 AND vac_start IS NULL"
		);
	}

	public function terminateAccounts() {
		$this->mails = array();

		// Send mails to accounts that have been inactive for 21 days
		$this->warnInactive();

		// Close really inactive accounts
		$this->closeInactive();

		// Close accounts on request
		$this->closeQuitters();

		// Delete accounts that haven't been confirmed yet
		$this->deleteUnconfirmed();
	}


	private function sendMail($file, $addr, $data, $log) {
		array_push($this->mails, array($file, $addr, $data, $log));
	}


	private function warnInactive() {
		// Get accounts that have been inactive for 21 days
		$q = $this->db->query(
			"SELECT id, name, email FROM account "
				. "WHERE NOT admin AND status='STD' "
				  . "AND last_logout IS NOT NULL AND last_logout > last_login "
				  . "AND (UNIX_TIMESTAMP(NOW()) - last_logout BETWEEN 24*3600*21 AND 24 * 3600 * 22)"
		);
		while ($r = dbFetchArray($q)) {
			list($id, $name, $addr) = $r;

			// Get the user's language
			$l = $this->accounts->call('getLanguage', $id);

			// Send the warning e-mail
			$this->sendMail("mail-warn-inactive.$l.txt", $addr, array("USER" => $name),
				"main/day: inactivity warning mail sent to player $name");
		}
	}


	private function closeInactive() {
		// Disable accounts that have been inactive for 28 days
		$q = $this->db->query(
			"SELECT id, name, email FROM account "
				. "WHERE NOT admin AND status='STD' AND last_logout IS NOT NULL "
				  . "AND UNIX_TIMESTAMP(NOW()) - last_logout >= 24 * 3600 * 28"
		);
		while ($r = dbFetchArray($q)) {
			list($uid, $name, $addr) = $r;
			$this->closeAccount($uid, $name, $addr, 'INAC', 'kick-inactive');
		}
	}


	private function closeQuitters() {
		// Close accounts for players who requested to quit
		$q = $this->db->query(
			"SELECT id, name, email FROM account "
				. "WHERE (status='STD' OR status='VAC') AND quit_ts IS NOT NULL "
				  . "AND UNIX_TIMESTAMP(NOW()) > quit_ts + 86400"
		);
		while ($r = dbFetchArray($q)) {
			list($uid, $name, $addr) = $r;
			$this->closeAccount($uid, $name, $addr, 'QUIT', 'quit');
		}
	}


	private function closeAccount($uid, $name, $addr, $reason, $mail) {
		// Get the user's language
		$l = $this->accounts->call('getLanguage', $uid);

		// Send the termination e-mail
		$this->sendMail("mail-$mail.$l.txt", $addr, array("USER" => $name),
			"main/day: mail sent to player $name ($reason)");

		// Terminate the account
		$this->accounts->call('terminate', $uid, $reason);
	}


	private function deleteUnconfirmed() {
		// Silently delete accounts that have the 'NEW' status and have been created more than 6 days ago
		$q = $this->db->query(
			"SELECT a.id FROM account a,account_log l "
				. "WHERE a.status='NEW' AND l.account=a.id AND l.action='CREATE' "
				  . "AND UNIX_TIMESTAMP(NOW()) - l.t > 6 * 24 * 3600"
		);
		$nd = array();
		while ($r = dbFetchArray($q)) {
			array_push($nd, $r[0]);
		}
		if (count($nd)) {
			$this->db->query("DELETE FROM account WHERE id IN (" . join(',', $nd) . ")");
		}
	}
}

?>

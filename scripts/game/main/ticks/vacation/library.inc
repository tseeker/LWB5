<?php

//--------------------------------------------------------------------
// "Vacation tick": decrements credits for players in vacation mode
//	and, should they reach 0, cancel vacation mode. Check for
//	accounts which must enter vacation mode as well.
// Executed four times a day
//--------------------------------------------------------------------

class main_ticks_vacation_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->main	= $this->lib->game->getLib();
		$this->vacation	= $this->lib->game->getLib("main/vacation");
	}

	public function runTick() {
		$this->db->safeTransaction(array($this, "decreaseCredits"));
		$this->db->safeTransaction(array($this, "enterVacation"));
	}

	public function decreaseCredits() {
		// Decrease credits for players in vacation mode
		$q = $this->db->query(
			"SELECT id FROM account WHERE status = 'VAC' AND vac_credits = 0 FOR UPDATE"
		);
		if ($q && dbCount($q)) {
			while ($r = dbFetchArray($q)) {
				list($uid) = $r;
				$this->vacation->call('leave', $uid);
			}
		}
		$this->db->query("UPDATE account SET vac_credits = vac_credits - 1 WHERE status = 'VAC'");
	}

	public function enterVacation() {
		// Look for players who should enter vacation mode
		$q = $this->db->query(
			"SELECT id FROM account "
				. "WHERE status='STD' AND vac_start IS NOT NULL AND UNIX_TIMESTAMP(NOW()) > vac_start "
				. "FOR UPDATE"
		);
		if (! dbCount($q)) {
			return;
		}

		while ($r = dbFetchArray($q)) {
			list($uid) = $r;
			$this->vacation->call('start', $uid);
		}
		$this->db->query(
			"UPDATE account SET status = 'VAC', vac_start = NULL $vsWhere"
				. "WHERE status='STD' AND vac_start IS NOT NULL "
				  . "AND UNIX_TIMESTAMP(NOW()) > vac_start"
		);
	}
}


?>

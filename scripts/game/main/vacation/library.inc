<?php

class main_vacation_library {
	var $index	= array(
		'canSet',
		'start',
		'leave'
	);

	function main_vacation_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function isOnVacation($uid) {
		$q = $this->db->query("SELECT COUNT(*) FROM account WHERE id=$uid AND status='VAC'");
		list($onVac) = dbFetchArray($q);
		return ($onVac == 1);
	}


	function getStatus($uid) {
		$q = $this->db->query("SELECT status,vac_start,vac_credits FROM account WHERE id=$uid");
		if ($q && dbCount($q) == 1) {
			return dbFetchHash($q);
		}
		return null;
	}


	function setStart($uid) {
		$vs = time() + 86400;
		$this->db->query("UPDATE account SET vac_start=$vs WHERE id=$uid AND status='STD'");
		logText("Account $uid requested vacation mode");
	}


	function resetStart($uid) {
		$this->db->query("UPDATE account SET vac_start=NULL WHERE id=$uid AND status='STD'");
		logText("Account $uid cancelled request for vacation mode");
	}
}

?>

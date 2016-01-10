<?php

class main_vacation_canSet {

	function main_vacation_canSet($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($uid) {
		$q = $this->db->query("SELECT t FROM account_log WHERE account=$uid AND action='VEND'");
		if (!$q) {
			return false;
		} elseif (dbCount($q) > 0) {
			list($last) = dbFetchArray($q);
			if (!(time() - $last > 604800 /* 7 * 24 * 3600 */ )) {
				return false;
			}
		}

		$q = $this->db->query("SELECT vac_credits,quit_ts FROM account WHERE id=$uid");
		if (!($q && dbCount($q) == 1)) {
			return false;
		}
		list($cred,$quit) = dbFetchArray($q);

		return is_null($quit) && ($cred > 0);
	}
}

?>

<?php

class main_account_getQuitCountdown {

	function main_account_getQuitCountdown($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid) {
		$q = $this->db->query("SELECT quit_ts FROM account WHERE id=$uid");
		if ($q && dbCount($q) == 1) {
			list($quit) = dbFetchArray($q);
		} else {
			$quit = null;
		}
		return $quit;
	}
}

?>

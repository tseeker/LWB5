<?php

class beta5_leaveVacation {

	function beta5_leaveVacation($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Called when a player leaves vacation mode
	function run($player) {
		if ($this->lib->game->params['novacation'] == 1) {
			return;
		}

		$this->db->query("UPDATE planet SET vacation='NO' WHERE owner=$player");

		// FIXME: Send messages
	}
}

?>

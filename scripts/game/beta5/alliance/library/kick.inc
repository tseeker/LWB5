<?php

class beta5_alliance_kick {

	function beta5_alliance_kick($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Kick an alliance member
	function run($pid) {
		$this->lib->call('leave', $pid, true);
		// FIXME: message
	}
}

?>

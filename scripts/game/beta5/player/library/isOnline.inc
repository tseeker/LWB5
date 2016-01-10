<?php

class beta5_player_isOnline {

	function beta5_player_isOnline($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->main	= $this->lib->game->getLib('main/account');
	}


	// Checks whether a player is currently online
	function run($pid) {
		$p = $this->lib->call('get', $pid);
		if (is_null($p)) {
			return false;
		}
		return $this->main->call('isOnline', $p['uid']);
	}
}

?>

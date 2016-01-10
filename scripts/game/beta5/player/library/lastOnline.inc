<?php

class beta5_player_lastOnline {

	function beta5_player_lastOnline($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->main	= $this->lib->game->getLib('main/account');
	}


	// Returns the timestamp at which the player last logged out,
	// or 0 if the player is online at the moment
	function run($playerID) {
		$player = $this->lib->call('get', $playerID, true);
		if (is_null($player)) {
			return 0;
		}

		return $this->main->call('lastOnline', $player['uid']);
	}
}

?>

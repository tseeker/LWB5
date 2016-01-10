<?php

class beta5_player_isOnVacation {

	function beta5_player_isOnVacation($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->getDBAccess();
		$this->main	= $this->game->getLib();
		$this->vac	= $this->game->getLib('main/vacation');
	}


	// Checks whether a player is currently on vacation
	function run($pid) {
		if ($this->main->call('isFinished')) {
			return true;
		}

		if ($this->lib->game->params['novacation'] == 1) {
			return false;
		}

		$p = $this->lib->call('get', $pid);
		if (is_null($p)) {
			return false;
		}
		return $this->vac->call('isOnVacation', $p['uid']);
	}
}

?>

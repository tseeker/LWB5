<?php

class beta5_player_checkAllies {

	function beta5_player_checkAllies($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	// Checks players that have set a player as trusted allies in order to know whether the player can control their fleets
	function run($pid) {
		$allies = $this->lib->call('isAllyOf', $pid);
		$res = array();
		foreach	($allies as $ally => $crap) {
			if ($this->lib->call('isOnline', $ally) || $this->lib->call('isOnVacation', $ally)) {
				continue;
			}

			$aList = $this->lib->call('getAllies', $ally);
			foreach	($aList as $aa) {
				if ($aa['id'] == $pid) {
					array_push($res, $ally);
					break;
				}
				if ($this->lib->call('isOnline', $aa['id'])) {
					break;
				}
			}
		}

		return $res;
	}
}

?>

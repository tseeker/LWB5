<?php

class beta5_player_getEnemyAlliances {

	function beta5_player_getEnemyAlliances($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of enemy alliances
	function run($pid) {
		$q = $this->db->query(
			"SELECT a.id,a.tag FROM enemy_alliance e,alliance a "
			. "WHERE e.player=$pid AND e.alliance=a.id"
		);
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0]] = is_null($r[1]) ? $r[2] : $r[1];
		}
		return $rs;
	}
}

?>

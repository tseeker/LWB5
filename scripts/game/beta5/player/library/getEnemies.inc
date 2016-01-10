<?php

class beta5_player_getEnemies {

	function beta5_player_getEnemies($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of enemy players
	function run($pid) {
		$q = $this->db->query(
			"SELECT p.id,p.name,u.name FROM enemy_player e,player p,account u "
			. "WHERE e.player=$pid AND e.enemy=p.id AND u.id=p.userid"
		);
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0]] = is_null($r[1]) ? $r[2] : $r[1];
		}
		return $rs;
	}
}

?>

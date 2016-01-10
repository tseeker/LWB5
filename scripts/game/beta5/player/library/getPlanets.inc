<?php

class beta5_player_getPlanets {

	function beta5_player_getPlanets($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the list of a player's planets
	function run($pid) {
		$q = $this->db->query(
			"SELECT p.id, p.name FROM planet p, system s "
				. "WHERE p.owner = $pid AND s.id = p.system "
				. "ORDER BY s.x, s.y, p.orbit");
		$a = array();
		while ($r = dbFetchArray($q)) {
			$a[$r[0]] = $r[1];
		}
		return $a;
	}
}

?>

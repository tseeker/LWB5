<?php

class beta5_player_getFleets {

	function beta5_player_getFleets($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the list of a player's fleets
	function run($pid) {
		$q = $this->db->query("SELECT id,name FROM fleet WHERE owner = $pid");
		$a = array();
		while ($r = dbFetchArray($q)) {
			$a[$r[0]] = $r[1];
		}
		return $a;
	}
}

?>

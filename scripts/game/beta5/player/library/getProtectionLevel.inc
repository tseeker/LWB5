<?php

class beta5_player_getProtectionLevel {

	function beta5_player_getProtectionLevel($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns a player's protection level
	function run($pid) {
		$q = $this->db->query(
			"SELECT s.prot FROM system s "
				. "WHERE s.id IN (SELECT DISTINCT p.system FROM planet p WHERE p.owner = $pid)"
		);
		if (dbCount($q) != 1) {
			return 0;
		}
		list($prot) = dbFetchArray($q);
		return $prot;
	}
}

?>

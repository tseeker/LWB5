<?php

class beta5_player_getDiploSummary {

	function beta5_player_getDiploSummary($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns a summary of the player's diplomatic relations
	function run($pid) {
		$q = $this->db->query("SELECT COUNT(*) FROM enemy_player WHERE player=$pid");
		list($c0) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM enemy_alliance WHERE player=$pid");
		list($c1) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM trusted WHERE player=$pid");
		list($c2) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM trusted WHERE friend=$pid");
		list($c3) = dbFetchArray($q);
		return array($c0,$c1,$c2,$c3);
	}
}

?>

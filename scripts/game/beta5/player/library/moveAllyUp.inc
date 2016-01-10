<?php

class beta5_player_moveAllyUp {

	function beta5_player_moveAllyUp($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Move an ally up the list
	function run($pid, $it) {
		$q = $this->db->query("SELECT COUNT(*) FROM trusted WHERE player=$pid");
		list($all) = dbFetchArray($q);
		$np = $it - 1;
		$this->db->query("UPDATE trusted SET level=$all WHERE player=$pid AND level=$it");
		$this->db->query("UPDATE trusted SET level=$it WHERE player=$pid AND level=$np");
		$this->db->query("UPDATE trusted SET level=$np WHERE player=$pid AND level=$all");
	}
}

?>

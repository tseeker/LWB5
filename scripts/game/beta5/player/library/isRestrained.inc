<?php

class beta5_player_isRestrained {

	function beta5_player_isRestrained($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Checks whether a player is restrained or not
	function run($pid) {
		$q = $this->db->query("SELECT restrain FROM player WHERE id = $pid");
		if (!($q || dbCount($q))) {
			return 666;
		}
		list($r) = dbFetchArray($q);
		return $r;
	}
}

?>

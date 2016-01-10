<?php

class beta5_getPlayerCount {

	function beta5_getPlayerCount($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	/** Get player count. */
	function run() {
		$r = $this->db->query("SELECT COUNT(*) FROM player "
			. "WHERE (quit IS NULL OR UNIX_TIMESTAMP(NOW())-quit<86400) AND NOT hidden");
		if (!$r) {
			return 0;
		}
		list($c) = dbFetchArray($r);
		return $c;
	}
}

?>

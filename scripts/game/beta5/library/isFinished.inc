<?php

class beta5_isFinished {

	function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->getDBAccess();
	}

	function run() {
		if ($this->game->params['victory'] == 0) {
			return false;
		}

		// Check for victory on normal matches
		if ($this->game->params['victory'] == 1) {
			$q = $this->db->query("SELECT COUNT(*) FROM alliance_victory "
				. "WHERE UNIX_TIMESTAMP(NOW()) >= time_of_victory");
			list($c) = dbFetchArray($q);
			return ($c > 0);
		}

		// Check for victory on CTF matches
		$q = $this->db->query("SELECT * FROM ctf_points WHERE points = 100");
		return (dbCount($q) == 1);
	}

}

?>

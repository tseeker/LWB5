<?php

class beta5_getPlayerStatus {

	function beta5_getPlayerStatus($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($pid) {
		$q = $this->db->query("SELECT cash FROM player WHERE id=$pid");
		if ($q && dbCount($q)) {
			list($cash) = dbFetchArray($q);
		} else {
			$cash = 0;
		}

		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE owner=$pid");
		if ($q && dbCount($q)) {
			list($planets) = dbFetchArray($q);
		} else {
			$planets = 0;
		}

		return array($planets, $cash, 0);
	}
}

?>

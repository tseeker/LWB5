<?php

class beta5_startVacation {

	function beta5_startVacation($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Called when a player enters vacation mode
	function run($player) {
		if ($this->lib->game->params['novacation'] == 1) {
			return;
		}

		$q = $this->db->query("SELECT id FROM planet WHERE owner=$player");
		while ($r = dbFetchArray($q)) {
			$qa = $this->db->query("SELECT COUNT(*) FROM fleet WHERE location={$r[0]} AND attacking");
			if (!$qa) {
				$mode = 'YES';
			} else {
				list($c) = dbFetchArray($qa);
				$mode = $c ? 'PEND' : 'YES';
			}
			$this->db->query("UPDATE planet SET vacation='$mode' WHERE id={$r[0]}");
		}

		// FIXME: Send messages
	}
}

?>

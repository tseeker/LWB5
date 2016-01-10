<?php

class beta5_player_addEnemyAlliance {

	function beta5_player_addEnemyAlliance($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Adds an alliance to the enemy list
	function run($pid, $eid) {
		$this->db->query("INSERT INTO enemy_alliance VALUES($pid,$eid)");
		$q = $this->db->query("SELECT id FROM player WHERE alliance=$eid AND a_status='IN'");
		$al = array();
		while ($r = dbFetchArray($q)) {
			array_push($al, $r[0]);
		}
		$this->lib->call('makeEnemies', $pid, $al);
	}
}

?>

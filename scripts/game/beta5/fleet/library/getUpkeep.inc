<?php

class beta5_fleet_getUpkeep {

	function beta5_fleet_getUpkeep($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Computes a fleet's upkeep
	function run($pl, $g, $f, $c, $b) {
		$r = $this->rules->call('get', $pl);
		$fu  = $g * $r['gaship_upkeep'];
		$fu += $f * $r['fighter_upkeep'];
		$fu += $c * $r['cruiser_upkeep'];
		$fu += $b * $r['bcruiser_upkeep'];
		return $fu;
	}
}

?>

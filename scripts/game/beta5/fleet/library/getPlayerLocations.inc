<?php

class beta5_fleet_getPlayerLocations {

	function beta5_fleet_getPlayerLocations($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns all of the locations at which a player has stationned fleets
	function run($pid) {
		$q = $this->db->query("SELECT DISTINCT location FROM fleet WHERE location IS NOT NULL AND owner=$pid");
		$a = array();
		while ($r = dbFetchArray($q)) {
			array_push($a, $r[0]);
		}
		return $a;
	}
}

?>

<?php

class beta5_fleet_getLocation {

	function beta5_fleet_getLocation($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the list of fleets in orbit around a planet
	function run($pid, $pl = null) {
		$q = $this->db->query("SELECT id,name FROM fleet WHERE location = $pid" . (is_null($pl) ? "" : " AND owner=$pl"));
		$a = array();
		while ($r = dbFetchArray($q)) {
			$a[$r[0]] = $r[1];
		}
		return $a;
	}
}

?>

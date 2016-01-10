<?php

class beta5_fleet_library {
	var $fleets		= array();
	var $fleetDepartures	= array();
	var $fleetArrivals	= array();

	var $index	= array (
		"arrival",
		"autoSplit",
		"disband",
		"get",
		"getLocation",
		"getPlayerLocations",
		"getPower",
		"getStats",
		"getUpkeep",
		"merge",
		"sendMoveMessages",
		"setOrders",
		"split",
		"switchStatus"
	);


	function beta5_fleet_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Invalidates the fleet cache
	function invCache($id = null) {
		if (is_null($id)) {
			$this->fleets = array();
		} else {
			$this->fleets[$id] = null;
		}
	}


	// Renames a fleet
	function rename($fid, $name) {
		$n = addslashes($name);
		$this->db->query("UPDATE fleet SET name='$n' WHERE id=$fid");
		$this->fleets[$fid] = null;
	}
}

?>

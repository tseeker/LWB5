<?php

class beta5_moving_getLocation {

	function beta5_moving_getLocation($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get a moving object's current location
	function run($oid) {
		$q = $this->db->query(
			"SELECT w.location FROM waypoint w,moving_object o "
			. "WHERE o.id=$oid AND w.move_id=$oid AND w.until_eta<=o.time_left "
			. "ORDER BY w.until_eta DESC LIMIT 1"
		);
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($loc) = dbFetchArray($q);
		return $loc;
	}
}

?>

<?php

class beta5_moving_getTrajectory {

	function beta5_moving_getTrajectory($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get an object's current trajectory
	function run($oid) {
		$q = $this->db->query("SELECT w.location AS id,w.until_eta AS eta,s.x AS x,s.y AS y,p.orbit AS orbit,p.status AS opacity,p.name AS name "
			. "FROM waypoint w,planet p,system s "
			. "WHERE w.move_id=$oid AND p.id=w.location AND s.id=p.system "
			. "ORDER BY w.until_eta DESC");
		if (!($q && dbCount($q))) {
			return array();
		}

		$rv = array();
		while ($r = dbFetchHash($q)) {
			array_push($rv, $r);
		}

		return $rv;
	}
}

?>

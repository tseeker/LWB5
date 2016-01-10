<?php

class beta5_moving_stop {

	function beta5_moving_stop($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	// Stop a moving object
	function run($oid, $newWait) {
		$q = $this->db->query(
			"SELECT w.location,w.until_eta,o.time_left "
			. "FROM waypoint w,moving_object o "
			. "WHERE o.id=$oid AND w.move_id=$oid AND w.until_eta<=o.time_left "
			. "ORDER BY w.until_eta DESC LIMIT 1"
		);
		if (!($q && dbCount($q) == 1)) {
			return null;
		}

		list($loc,$ue,$tl) = dbFetchArray($q);
		if ($ue == 0) {
			$this->db->query("UPDATE moving_object SET wait_order="
				. (is_null($newWait)?"NULL":$newWait)
				. " WHERE id=$oid");
			return false;
		}

		$newDelay = 1 + $tl - $ue;
		$this->db->query("UPDATE moving_object SET m_to=$loc,changed=changed+10,"
			. "wait_order=".(is_null($newWait)?"NULL":$newWait).","
			. "time_left=$newDelay WHERE id=$oid");
		logText("Deleting waypoints for moving object #$oid", LOG_DEBUG);
		$this->db->query("DELETE FROM waypoint WHERE move_id=$oid");
		logText("Inserting single waypoint moving object #$oid", LOG_DEBUG);
		$this->db->query("INSERT INTO waypoint VALUES($oid,$loc,0)");
		return true;
	}
}

?>

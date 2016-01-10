<?php

class beta5_moving_redirect {

	function beta5_moving_redirect($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Redirects a moving object to a new destination
	function run($oid, $newDest, $speed, $cruisers, $newWait) {
		$loc = $this->lib->call('getLocation', $oid);
		if (is_null($loc)) {
			return false;
		} elseif ($loc == $newDest) {
			return $this->lib->call('stop', $oid, $newWait);
		}

		$t = $this->lib->call('computeTrajectory', $loc, $newDest, $speed, $cruisers);

		$this->db->query("UPDATE moving_object SET m_to=$newDest,changed=changed+10,"
			. "hyperspace=" . dbBool($t['hs'] || !is_null($newWait))
			. ",wait_order=".(is_null($newWait)?"NULL":$newWait)
			. ",time_left=".$t['time']." WHERE id=$oid");
		logText("Deleting waypoints for moving object #$oid", LOG_DEBUG);
		$this->db->query("DELETE FROM waypoint WHERE move_id=$oid");

		// Create trajectory data
		$qsl = array(); $sum = 0;
		$tl = array_reverse($t['list']);
		foreach ($tl as $w) {
			array_push($qsl, "($oid,".$w['pid'].",$sum)");
			$sum += $w['time'];
		}
		logText(count($qsl) . " new waypoint(s) reinserted for moving object #$oid", LOG_DEBUG);

		// Insert trajectory data
		foreach ($qsl as $qs) {
			if (!$this->db->query("INSERT INTO waypoint VALUES $qs")) {
				return false;
			}
		}

		return true;
	}
}

?>

<?php

class beta5_moving_newObject {

	function beta5_moving_newObject($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Generate a new moving object entry
	function run($srcId, $dstId, $speed, $cruisers, $wait) {
		// Compute trajectory
		$t = $this->lib->call('computeTrajectory', $srcId, $dstId, $speed, $cruisers);

		// Insert object entry
		$qs  = "INSERT INTO moving_object(m_from,m_to,changed,time_left,hyperspace";
		if (!is_null($wait)) {
			$qs .= ",wait_order";
		}
		$isHS = ($t['hs'] || !is_null($wait)) ? 1 : 0;
		$qs .= ") VALUES ($srcId,$dstId,0,".$t['time']."," . dbBool($isHS);
		if (!is_null($wait)) {
			$qs .= ",$wait";
		}
		$qs .= ")";
		$mid = $this->db->query($qs);
		if (!$mid) {
			logText("New object: mid was null :s", LOG_WARNING);
			return null;
		}

		// Create trajectory data
		$qsl = array(); $sum = 0;
		$tl = array_reverse($t['list']);
		foreach ($tl as $w) {
			array_push($qsl, "($mid,".$w['pid'].",$sum)");
			$sum += $w['time'];
		}
		logText(count($qsl) . " waypoint(s) to be added for new object #$mid", LOG_DEBUG);

		// Insert trajectory data
		foreach ($qsl as $qs) {
			if (!$this->db->query("INSERT INTO waypoint VALUES $qs")) {
				$this->db->query("DELETE FROM moving_object WHERE id=$mid");
				return null;
			}
		}
		return $mid;
	}
}

?>

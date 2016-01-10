<?php

class beta5_standby_library {
	var $index = array ();

	function beta5_standby_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Generates a new Hyperspace stand-by order
	function create($time, $location, $origin = null, $spent = null) {
		if (is_null($origin)) {
			$origin = "NULL";
		}
		if (is_null($spent)) {
			$spent = "0";
		}
		return $this->db->query("INSERT INTO hs_wait (time_left,time_spent,drop_point,origin) VALUES ($time,$spent,$location,$origin)");
	}


	// Checks whether fleets can be destroyed while waiting in hyperspace at a given location
	function canDestroy($location, $owner) {
		// Conditions for fleet destruction: no HS beacon OR (HS beacon AND not in alliance AND not trusted)
		$p = $this->planets->call('byId', $location);
		$canDestroy = ($p['beacon'] == 0);
		if (!($canDestroy || is_null($p['owner']) || $p['owner'] == $owner)) {
			$fo = $this->players->call('get', $owner);
			$po = $this->players->call('get', $p['owner']);
			$canDestroy = !$this->players->call('isAlly', $p['owner'], $owner) && (is_null($fo['aid']) || $fo['aid'] !== $po['aid']);
		}
		return $canDestroy;
	}


	// Computes the probability for fleet destruction when standing by in Hyperspace
	function getLossProb($timeSpent) {
		$fact = ($timeSpent+1) / 36;
		$fact *= $fact;
		if ($fact > 1) {
			$fact = 1;
		}
		return floor($fact * 100);
	}
}

?>

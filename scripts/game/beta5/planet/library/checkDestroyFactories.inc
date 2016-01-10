<?php

class beta5_planet_checkDestroyFactories {

	function beta5_planet_checkDestroyFactories($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	// Check a planet to see whether it's possible to destroy factories
	function run($id, $nb, $t) {
		$q = $this->db->query("SELECT ${t}fact,sale FROM planet WHERE id = $id");
		list($nf,$sale) = dbFetchArray($q);
		logText("checkDestFact($id, $nb, $t) : nf=$nf, sale=$sale");
		if ($nf < $nb) {
			return 1;
		}
		if (!is_null($sale)) {
			return 4;
		}
		if ($t == 'm' && $nf == $nb) {
			return 2;
		}

		// Get changes for this type of factories within the past "2h" (actually, two hour ticks)
		$interval = $this->game->ticks['hour']->interval * 2;
		$q = $this->db->query(
			"SELECT SUM(change) FROM facthist "
				. "WHERE planet=$id AND UNIX_TIMESTAMP(NOW()) - moment <= $interval "
				  . "AND " . ($t == 'm' ? "" : "NOT ") . "is_military"
		);
		if (dbCount($q)) {
			list($ch2) = dbFetchArray($q);
			if (is_null($ch2)) {
				$ch2 = 0;
			}
		} else {
			$ch2 = 0;
		}
		if ($ch2 > 0) {
			return 30;
		}

		// Get changes for this type of factories within the past 24h
		$interval = $this->game->ticks['day']->interval;
		$q = $this->db->query(
			"SELECT SUM(change) FROM facthist "
				. "WHERE planet=$id AND UNIX_TIMESTAMP(NOW()) - moment <= $interval "
				  . "AND " . ($t == 'm' ? "" : "NOT ") . "is_military"
		);
		if (dbCount($q)) {
			list($ch24) = dbFetchArray($q);
			if (is_null($ch24)) {
				$ch24 = 0;
			}
		} else {
			$ch24 = 0;
		}

		logText("checkDestFact($id, $nb, $t) : ch24=$ch24 ch2=$ch2");
		$onf = $nf - $ch24;
		if (($onf > 100 && $nf - $nb < floor($onf * 0.9)) || ($onf < 100 && ($nf - $nb < 0 || $nf - $nb < $onf - 10)) ) {
			return 3;
		}

		// FIXME : siege

		return 0;
	}
}

?>

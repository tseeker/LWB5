<?php

class beta5_alliance_updateVictory {

	function beta5_alliance_updateVictory($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Compute victory conditions for an alliance
	function run($aid) {
		// Get the total amount of planets
		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE status = 0");
		list($pCount) = dbFetchArray($q);

		// Get the amount of planets the alliance controls
		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE owner IN ("
			. "SELECT id FROM player WHERE alliance = $aid AND a_status='IN')");
		list($aCount) = dbFetchArray($q);

		// Compute the ratio
		$pRatio = $aCount / $pCount;
		if ($pRatio > 0.75) {
			$pRatio = 0.75;

			// Check if the alliance was already scheduled for victory
			$q = $this->db->query("SELECT time_of_victory FROM alliance_victory WHERE alliance=$aid");
			if (dbCount($q)) {
				list($tov) = dbFetchArray($q);
				if (time() > $tov) {
					$tValue = 0.25;
				} else {
					$diff = $tov - time();
					if ($diff > 604800) {
						$diff = 604800;
					}
					$tValue = (604800 - $diff) / 2419200; // 7*24*3600*4
				}
				logText("tov = $tov, diff = $diff, tval = $tValue");
			} else {
				$this->db->query("INSERT INTO alliance_victory(alliance, time_of_victory)"
					. " VALUES ($aid, UNIX_TIMESTAMP(NOW()) + 604800)");
				$tValue = 0;
			}
		} else {
			$tValue = 0;
			$this->db->query("DELETE FROM alliance_victory WHERE alliance=$aid");
		}

		return round(($pRatio + $tValue) * 100);
	}
}
?>

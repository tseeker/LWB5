<?php

class beta5_planet_destroyTurrets {

	function beta5_planet_destroyTurrets($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	// Try to destroy turrets on a planet
	function run($id, $nb) {
		$q = $this->db->query("SELECT turrets,sale FROM planet WHERE id = $id");
		list($nt,$sale) = dbFetchArray($q);
		if ($nt < $nb) {
			return 1;
		}
		if (!is_null($sale)) {
			return 3;
		}

		$interval = $this->game->ticks['day']->interval;
		$q = $this->db->query(
			"SELECT SUM(change) FROM turhist "
				. "WHERE planet = $id AND UNIX_TIMESTAMP(NOW()) - moment < $interval"
		);
		if (dbCount($q)) {
			list($ch) = dbFetchArray($q);
			if (is_null($ch)) {
				$ch = 0;
			}
		} else {
			$ch = 0;
		}
		$ont = $nt - $ch;
		if ($nt - $nb < floor($ont * 0.8)) {
			return 2;
		}

		// FIXME : siege

		$tm = time();
		$this->db->query("UPDATE planet SET turrets = turrets - $nb WHERE id = $id");
		$this->db->query("INSERT INTO turhist VALUES ($id,$tm,-$nb)");
		$this->lib->call('updateHappiness', $id);

		return 0;
	}
}

?>

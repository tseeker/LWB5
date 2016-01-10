<?php

class beta5_planet_ownerChange {

	function beta5_planet_ownerChange($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
		$this->bq	= $lib->game->getLib('beta5/bq');
	}

	function run($planet, $newOwner = null, $noVacation = 'NO') {
		$info = $this->lib->call('byId', $planet);
		if (is_null($info)) {
			logText("beta5::planet::ownerChange(): planet '$planet' not found", LOG_WARNING);
			return;
		}

		$this->bq->call('flush', $planet);
		$rqs = is_null($info['owner']) ? "" : (", renamed = " . time() . " ");
		$this->db->query(
			"UPDATE planet "
				. "SET beacon = 0, built_probe = FALSE, probe_policy = NULL, abandon = NULL, "
					. "vacation = '$noVacation', bh_prep = NULL, sale = NULL, "
					. "owner = " . (is_null($newOwner) ? "NULL" : $newOwner) . $rqs
				. "WHERE id = $planet"
		);

		if (is_null($newOwner)) {
			$this->db->query("DELETE FROM planet_abandon_time WHERE id = $planet");
		} elseif (is_null($info['owner'])) {
			$this->db->query(
				"INSERT INTO planet_abandon_time (id, time_required) "
					. "VALUES ($planet, 6)"
			);
		} else {
			$this->db->query(
				"UPDATE planet_abandon_time SET time_required = 6 WHERE id = $planet"
			);
		}

		$this->lib->call('updateHappiness', $planet);
		return $this->lib->call('updateMaxPopulation', $planet, $info['owner'], $newOwner);
	}
}

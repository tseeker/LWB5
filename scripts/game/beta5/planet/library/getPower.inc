<?php

class beta5_planet_getPower {
	var $ePower = array();

	function beta5_planet_getPower($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	function run($pid, $turrets = null) {
		if (is_null($turrets)) {
			$q = $this->db->query("SELECT owner,turrets FROM planet WHERE id=$pid");
			if (!($q && dbCount($q) == 1)) {
				return 0;
			}
			list($owner,$turrets) = dbFetchArray($q);
		} else {
			$owner = $pid;
		}

		if (is_null($this->ePower[$owner])) {
			$rules = $this->rules->call('get', $owner);
			if (is_null($rules)) {
				return 0;
			}
			$this->ePower[$owner] = floor($rules['turret_power'] * $rules['effective_fleet_power'] / 100);
		}

		return $turrets * $this->ePower[$owner];
	}
}

?>

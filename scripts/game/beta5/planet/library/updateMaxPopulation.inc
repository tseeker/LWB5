<?php

class beta5_planet_updateMaxPopulation {
	var $maxPops = array();

	function beta5_planet_updateMaxPopulation($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Updates a planet's maximum population
	function run($id, $formerOwner, $newOwner) {
		// Get the current technological levels for both players
		$r = $this->rules->call('get', $formerOwner);
		$oldLevel = ($r['planet_max_pop'] - 10000) / 10000;
		if ($formerOwner == $newOwner) {
			$oldLevel --;
		}
		$r = $this->rules->call('get', $newOwner);
		$newLevel = ($r['planet_max_pop'] - 10000) / 10000;

		// If the levels are equivalent, return
		if ($oldLevel == $newLevel) {
			return -1;
		}

		// Get the planet's maximum populations for each tech level
		$maxPops = $this->getMaxPopulations($id);

		// Get the planet's current population
		$pinf = $this->lib->call('byId', $id);
		$pop = $pinf['pop'];

		// If the population's greater than the new max. pop., fix the "real" max pop
		$maxPop = $maxPops[$newLevel];
		if ($pop > $maxPop) {
			$maxPop = $pop + rand(0, min(500, $maxPops[3] - $pop));
		}

		// Update the planet
		$this->db->query("UPDATE planet SET max_pop=$maxPop WHERE id=$id");

		return $maxPop;
	}


	// Get a planet's maximum populations
	function getMaxPopulations($id) {
		if (is_null($id)) {
			return null;
		}
		if (!is_array($this->maxPops[$id])) {
			$this->maxPops[$id] = array();
			$q = $this->db->query("SELECT max_pop FROM planet_max_pop WHERE planet=$id ORDER BY tech_level ASC");
			$i = 0;
			while ($r = dbFetchArray($q)) {
				$this->maxPops[$id][$i++] = $r[0];
			}
		}
		return $this->maxPops[$id];
	}
}

?>

<?php

class beta5_planet_library {
	var $pOwner	= array();
	var $pPlanets	= array();

	var $index	= array(
		'buildFactories',
		'checkBuildFactories',
		'checkDestroyFactories',
		'destroyTurrets',
		'detectFleets',
		'getIncome',
		'getPower',
		'getStats',
		'ownerChange',
		'rename',
		'restoreNeutral',
		'updateHappiness',
		'updateMaxPopulation',
		'updateMilStatus'
	);


	function beta5_planet_library($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	// Returns data regarding a planet with a specified identifier
	function byId($id) {
		if (is_null($id)) {
			logText("****** BUG: planet::byId(null) called", LOG_ERR);
			l::backtrace();
			return null;
		}
		$q = $this->db->query(
			"SELECT p.id AS id,p.name AS name,s.x AS x,s.y AS y,"
			 . "p.orbit AS orbit,s.prot AS prot,p.status AS status,p.owner AS owner,"
			 . "p.pop AS pop,p.max_pop AS max_pop,p.happiness AS happiness,p.ifact AS ifact,"
			 . "p.mfact AS mfact,p.turrets AS turrets,p.abandon AS abandon,"
			 . "p.beacon AS beacon,p.bh_prep AS bh_prep,p.beacon AS beacon,"
			 . "p.sale AS sale,p.bh_unhappiness AS bh_unhappiness,p.vacation AS vacation,"
			 . "s.id AS system,p.built_probe AS built_probe,p.corruption AS corruption,"
			 . "(UNIX_TIMESTAMP(NOW()) - p.renamed > 1296000) AS rename "
			. "FROM planet p, system s "
			. "WHERE p.id=$id AND s.id=p.system"
		);
		if (!($q && dbCount($q) == 1)) {
			logText("****** BUG: planet::byId($id) called (orbit not found)");
			return null;
		}
		$r = dbFetchHash($q);
		$r["rename"] = ($r['rename'] == 't') ? 1 : 0;

		if (is_null($this->pOwner[$r['id']])) {
			$this->pOwner[$r['id']] = $r['owner'];
		}

		return $r;
	}


	// Returns data regarding a planet with a specified name
	function byName($n) {
		$q = $this->db->query("SELECT id FROM planet WHERE LOWER(name)='".strtolower(addslashes($n))."'");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($id) = dbFetchArray($q);

		return $this->byId($id);
	}


	// Checks whether a planet name exists, either in the game or in the
	// registration queue
	public function nameExists($name) {
		$okName = strtolower(addslashes($name));
		$q = $this->db->query(
			"SELECT id FROM planet WHERE LOWER(name) = '$okName' "
				. "UNION SELECT account AS id FROM planet_reg_queue "
					. "WHERE LOWER(p_name) = '$okName'"
		);
		return (dbCount($q) > 0);
	}


	// Gets a planet's owner
	function getOwner($pid) {
		if (!is_null($this->pOwner[$pid])) {
			return $this->pOwner[$pid];
		}
		$q = $this->db->query("SELECT owner FROM planet WHERE id=$pid");
		list($r) = dbFetchArray($q);
		return $r;
	}


	// Destroy factories on a planet
	function destroyFactories($id, $nb, $t) {
		$this->db->query("UPDATE planet SET ${t}fact = ${t}fact - $nb WHERE id = $id");
		$this->lib->call('updateFactHistory', $id, $t == "m", -$nb);
		$this->lib->call('updateHappiness', $id);
	}


	// Updates a planet's factory history and delete old history records
	function updateFactHistory($pid, $mil, $nb) {
		$this->db->query("INSERT INTO facthist (planet, is_military, change) VALUES($pid, " . ($mil?'TRUE':'FALSE') . ", $nb)");
		$this->db->query("DELETE FROM facthist WHERE UNIX_TIMESTAMP(NOW()) - moment > 86400");
	}


	// Abandon a planet
	function setAbandon($pid, $start = true) {
		if ($start) {
			$q = $this->db->query("SELECT time_required FROM planet_abandon_time WHERE id = $pid");
			list($time) = dbFetchArray($q);
			if (is_null($time)) {
				l::warn("**** BUG: Planet #{$pid} has no abandon record, creating one with 24h");
				$this->db->query(
					"INSERT INTO planet_abandon_time (id, time_required) "
						. "VALUES ($pid, 24)"
				);
				$time = 24;
			}
		} else {
			$time = "NULL";
		}
		$this->db->query(
			"UPDATE planet SET abandon = $time WHERE id = $pid"
		);
	}


	// Destroy a planet
	function setBoom($pid, $start = true) {
		$this->db->query("UPDATE planet SET bh_prep=".($start?"3":"NULL")." WHERE id=$pid");
	}

	// Checks if a planet can be allocated
	function canAllocate($planetID) {
		// Get the planet
		$planet = $this->lib->call('byId', $planetID);

		// Compute optimal factories and turrets
		$x = ($planet['pop'] - 2000) / 48000;
		$optFact = ($planet['pop'] / 30) - 754 * $x * $x;
		$optTurrets = ($planet['pop'] / 22) - 510 * $x * $x;

		// Check if the planet is OK already
		$facts = $planet['ifact'] + $planet['mfact'];
		$turrets = $planet['turrets'];
		return ($facts <= $optFact * 1.1 && $turrets <= $optTurrets && $planet['corruption'] < 3200);
	}

	// Gets the list of fleets a beacon detects
	function getDetectedFleets($planetID) {
		$detected = array();

		$q = $this->db->query("SELECT i_level,fl_size,fl_owner FROM beacon_detection WHERE planet = $planetID");
		while ($r = dbFetchHash($q)) {
			if ($r['i_level'] == 4) {
				$r['owner'] = $this->game->getLib('beta5/player')->call('getName', $r['fl_owner']);
			}
			array_push($detected, $r);
		}

		return $detected;
	}
}

?>

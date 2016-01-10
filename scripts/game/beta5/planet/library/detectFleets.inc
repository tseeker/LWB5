<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/planet/library/detectFleets.inc
//
// This function checks fleets in HyperSpace standby around a planet and
// tries to detect them if possible.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_planet_detectFleets {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;

		$this->ecm	= $this->game->getLib('beta5/ecm');
		$this->msgs	= $this->game->getLib('beta5/msg');
		$this->players	= $this->game->getLib('beta5/player');
		$this->fleets	= $this->game->getLib('beta5/fleet');
	}


	public function run($planet) {
		// Get information about the planet
		$q = $this->db->query("SELECT owner, beacon, name FROM planet WHERE id = $planet");
		list($owner, $beacon, $name) = dbFetchArray($q);

		// If there's no beacon or no owner, we're done
		$reqLevel = $this->game->params['fakebeacons'] ? 1 : 2;
		if (is_null($owner) || $beacon < $reqLevel) {
			return;
		}

		// Get fleets that are in Hyperspace stand-by above the planet
		$hsFleets = $this->getHSFleets($planet);
		if (empty($hsFleets)) {
			return;
		}

		// Store planet name
		if (is_null($this->pNames)) {
			$this->pNames = array();
		}
		if (is_null($this->pNames[$planet])) {
			$this->pNames[$planet] = $name;
		}

		// Get the planet owner's trusted allies and alliance
		$allies = $this->getAllies($owner);
		$alliance = $this->getAlliance($owner);

		// Check each fleet for detection
		foreach ($hsFleets as $fleet) {
			if ($fleet['owner'] == $owner || in_array($fleet['owner'], $allies)) {
				continue;
			}

			$this->tryDetect($planet, $owner, $alliance, $fleet);
		}
	}


	private function getHSFleets($planet) {
		$q = $this->db->query(
			"SELECT f.id, f.owner, w.time_spent FROM fleet f, hs_wait w "
				. "WHERE f.location IS NULL AND f.moving IS NULL "
				  . "AND w.id = f.waiting AND w.drop_point = $planet "
				  . "AND f.id NOT IN (SELECT fleet FROM beacon_detection WHERE planet = $planet)"
		);

		$fleets = array();
		while ($r = dbFetchHash($q)) {
			array_push($fleets, $r);
		}

		return $fleets;
	}


	private function getAllies($player) {
		$rawAllies = $this->players->call('getAllies', $player);
		$allies = array();
		foreach ($rawAllies as $ally) {
			array_push($allies, $ally['id']);
		}

		return $allies;
	}

	private function getAlliance($player) {
		if (is_null($this->pAlliances)) {
			$this->pAlliances = array();
		}
		if (is_null($this->pAlliances[$player])) {
			$q = $this->db->query("SELECT alliance FROM player WHERE id = $player AND a_status = 'IN'");
			if (dbCount($q)) {
				list($alliance) = dbFetchArray($q);
			} else {
				$alliance = null;
			}
			$this->pAlliances[$player] = $alliance;
		}
		return $this->pAlliances[$player];
	}


	private function tryDetect($planet, $owner, $alliance, $fleet) {
		// Compute probability of detection according to time spent
		$tsProb = ($fleet['time_spent'] >= 3) ? 1 : (0.5 + $fleet['time_spent'] / 6);

		// Get ECM/ECCM levels
		$ecm = $this->getECMLevel($fleet['owner']);
		$eccm = $this->getECCMLevel($owner);

		// Compute probability of detection based on ECM/ECCM
		$ecmProb = (1 + $this->ecm->call('getInformationLevel', $ecm, $eccm)) * 0.2;

		// Get fleet owner's alliance
		$fAlliance = $this->getAlliance($fleet['owner']);

		// Actual probability
		$prob = $tsProb * $ecmProb * 0.8;
		if (!is_null($alliance) && $fAlliance === $alliance) {
			$prob += 1/3;
		}
		$prob = min(1, $prob);

		// Log it
		logText("Planet #$planet: probability of detecting fleet #{$fleet['id']} is $prob "
			. "(tsProb = $tsProb, ecmProb = $ecmProb)");

		// Try detecting it
		$rnd = rand(0, 100000) / 100000;
		if ($rnd < $prob) {
			$this->fleetDetected($planet, $owner, $fleet, $ecm, $eccm);
		}
	}

	private function getECCMLevel($player) {
		if (is_null($this->eccmLevel)) {
			$q = $this->db->query("SELECT value FROM rule WHERE player = $player AND name = 'eccm_level'");
			list($this->eccmLevel) = dbFetchArray($q);
		}
		return $this->eccmLevel;
	}

	private function getECMLevel($player) {
		if (is_null($this->ecmLevel)) {
			$this->ecmLevel = array();
		}
		if (is_null($this->ecmLevel[$player])) {
			$q = $this->db->query("SELECT value FROM rule WHERE player = $player AND name = 'ecm_level'");
			list($this->ecmLevel[$player]) = dbFetchArray($q);
		}
		return $this->ecmLevel[$player];
	}


	private function fleetDetected($planet, $owner, $fleet, $ecm, $eccm) {
		// Get the information level
		$iLevel = $this->ecm->call('getInformationLevel', $ecm, $eccm);

		// Detected fleet size
		if ($iLevel > 0) {
			$fleetSize = $this->computeDetectedSize($iLevel, $fleet['id']);
		} else {
			$fleetSize = null;
		}
		logText("Planet #$planet: fleet #{$fleet['id']} detected at level $iLevel"
			. ($iLevel == 0 ? "" : " (size: $fleetSize)"));

		// Did we detect the owner?
		if ($iLevel == 4) {
			$fleetOwner = $fleet['owner'];
			$fleetOwnerName = $this->players->call('getName', $fleetOwner);
		} else {
			$fleetOwner = $fleetOwnerName = null;
		}

		// Insert into the status table
		$sQuery = "INSERT INTO beacon_detection(planet, fleet, i_level";
		if ($iLevel > 0) {
			$sQuery .= ", fl_size";
			if ($iLevel == 4) {
				$sQuery .= ", fl_owner";
			}
		}
		$sQuery .= ") VALUES ($planet, {$fleet['id']}, $iLevel";
		if ($iLevel > 0) {
			$sQuery .= ", $fleetSize";
			if ($iLevel == 4) {
				$sQuery .= ", $fleetOwner";
			}
		}
		$this->db->query("$sQuery)");

		// Send messages
		$this->msgs->call('send', $owner, 'detect', array(
			"planet"	=> $planet,
			"p_name"	=> $this->pNames[$planet],
			"is_owner"	=> 'f',
			"i_level"	=> $iLevel,
			"fl_size"	=> $fleetSize,
			"flo_id"	=> $fleetOwner,
			"flo_name"	=> $fleetOwnerName
		));
		$this->msgs->call('send', $fleet['owner'], 'detect', array(
			"planet"	=> $planet,
			"p_name"	=> $this->pNames[$planet],
			"is_owner"	=> 't',
			"i_level"	=> $iLevel
		));
	}


	private function computeDetectedSize($iLevel, $fleet) {
		// Compute the actual fleet size
		$fData = $this->fleets->call('get', $fleet);
		$actualFleetSize = $this->fleets->call('getPower', $fData['owner'], $fData['gaships'],
			$fData['fighters'], $fData['cruisers'], $fData['bcruisers']);

		// Compute the detected fleet size
		if ($iLevel >= 3) {
			$variation = 0;
		} elseif ($iLevel == 2) {
			$variation = rand(5, 10) / 100;
		} elseif ($iLevel == 1) {
			$variation = rand(20, 30) / 100;
		}
		$variation = rand(0,1) ? $variation : (-$variation);
		return round($actualFleetSize * ($variation + 1));
	}
}

?>

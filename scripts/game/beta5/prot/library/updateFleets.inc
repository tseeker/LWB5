<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/prot/library/updateFleets.inc
//
// This function checks all protected planets on which a player is
// marked as attacking to make sure he gets destroyed. In addition,
// it checks planets on which the Peacekeepers have fleets and on which
// they are no longer required.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_prot_updateFleets {

	private static $fleetName = "Peacekeeper Commando";
	private $peacekeepers = null;

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->fleets	= $this->game->getLib('beta5/fleet');
		$this->msgs	= $this->game->getLib('beta5/msg');
	}


	public function run() {
		// Get Peacekeepers player ID
		$this->peacekeepers = $this->lib->call('getPeacekeepers');

		// Check for systems with enemy fleets
		$rPlanets = $this->checkBattleSystems();

		// Check for fleets to evacuate
		$ePlanets = $this->checkEvacuations($rPlanets);
	}


	private function checkBattleSystems() {
		$reinforce = array();

		$enemyFleets = $this->getEnemyFleets();
		if (empty($enemyFleets)) {
			return $reinforce;
		}

		$sysOwners = $this->getSystemOwners(array_keys($enemyFleets));
		foreach ($enemyFleets as $systemID => $planets) {
			$ownerID = $sysOwners[$systemID]['owner'];
			$ownPlanets = $sysOwners[$systemID]['planets'];

			foreach ($planets as $planetID => $eFleets) {
				// Check enemy fleet size
				$q = $this->db->query(
					"SELECT SUM(fighters + gaships / 2), SUM(cruisers), SUM(bcruisers) "
							. "FROM fleet "
						. "WHERE id IN (" . join(',', $eFleets) . ")"
				);
				list($eFighters, $eCruisers, $eBattleCruisers) = dbFetchArray($q);

				// Check Peacekeeper fleet size
				$q = $this->db->query(
					"SELECT SUM(fighters), SUM(cruisers), SUM(bcruisers) FROM fleet "
						. "WHERE location = $planetID AND owner = {$this->peacekeepers}"
				);
				list($pkFighters, $pkCruisers, $pkBattleCruisers) = dbFetchArray($q);

				// Compute fleet size requirements for Peacekeepers
				$rFighters = ceil($eFighters * 2 - (int) $pkFighters);
				$rCruisers = $eCruisers * 2 - (int) $pkCruisers;
				$rBattleCruisers = $eBattleCruisers * 2 - (int) $pkBattleCruisers;

				// Do we need to reinforce ?
				if ($rFighters > 0 || $rCruisers > 0 || $rBattleCruisers > 0) {
					// If we need fighters, do we have enough haul space?
					$haul = $rCruisers * 20 + $rBattleCruisers * 15;
					if ($haul * 0.9 < $rFighters) {
						// Add cruisers to match haul size
						$rCruisers += ceil(($rFighters - ($haul * 0.9)) / 20);
					}

					$reinforce[$planetID] = array(true, $rFighters, $rCruisers, $rBattleCruisers);
				} else {
					$reinforce[$planetID] = array(false);
				}

				// Make sure all enemy fleets are attacking and battle-ready
				$this->db->query(
					"UPDATE fleet SET attacking = 't', time_spent = (CASE "
							. "WHEN time_spent <= 15 THEN 16 "
							. "ELSE time_spent "
						. "END) WHERE id IN (" . join(',', $eFleets) . ")"
				);

				// If we're on a planet not owned by the system's owner,
				// make sure his fleets are defending
				if (!in_array($planetID, $ownPlanets)) {
					$this->db->query(
						"UPDATE fleet SET attacking = 'f' "
							. "WHERE location = $planetID AND owner = $ownerID"
					);
				}
			}
		}

		foreach ($reinforce as $planetID => $reinforcements) {
			if (!$reinforcements[0]) {
				continue;
			}
			array_shift($reinforcements);
			$this->reinforce($planetID, $reinforcements);
		}

		return array_keys($reinforce);
	}


	private function getEnemyFleets() {
		$q = $this->db->query(
			"SELECT f.id, p.id, p.system FROM fleet f, planet p, pk_sys_status s "
				. "WHERE s.status = 'O' AND p.system = s.system "
				  . "AND f.owner = s.player AND f.location = p.id "
				. "ORDER BY p.id, f.owner "
				. "FOR UPDATE OF f, p"
		);

		$result = array();
		while ($r = dbFetchArray($q)) {
			if (!is_array($result[$r[2]])) {
				$result[$r[2]] = array();
			}
			if (!is_array($result[$r[2]][$r[1]])) {
				$result[$r[2]][$r[1]] = array();
			}
			array_push($result[$r[2]][$r[1]], $r[0]);
		}

		return $result;
	}


	private function getSystemOwners($systems) {
		$q = $this->db->query(
			"SELECT system, id, owner FROM planet "
				. "WHERE system IN (" . join(',', $systems) . ") "
				. "ORDER BY system, (owner IS NOT NULL) DESC"
		);

		$result = array();
		while ($r = dbFetchArray($q)) {
			if (!is_array($result[$r[0]])) {
				$result[$r[0]] = array(
					'owner'		=> $r[2],
					'planets'	=> array($r[1])
				);
			} elseif (!is_null($r[2])) {
				array_push($result[$r[0]]['planets'], $r[1]);
			}
		}

		return $result;
	}


	private function reinforce($planetID, $fSize) {
		// Generate fleet
		$name = addslashes(self::$fleetName);
		$this->db->query(
			"INSERT INTO fleet (location, owner, fighters, cruisers, bcruisers, time_spent, name) "
				. "VALUES ($planetID, {$this->peacekeepers}, {$fSize[0]}, {$fSize[1]}, "
					. "{$fSize[2]}, 16, '$name')"
		);
		$fPower = $this->fleets->call('getPower', $this->peacekeepers, 0, $fSize[0], $fSize[1], $fSize[2]);

		// Get planet name
		$q = $this->db->query("SELECT name FROM planet WHERE id = $planetID");
		list($pName) = dbFetchArray($q);

		// Get random origin (nebula or planetary remains)
		$q = $this->db->query(
			"SELECT id, name FROM planet WHERE status > 0 ORDER BY RANDOM() LIMIT 1"
		);
		list($originID, $origin) = dbFetchArray($q);
		$origin = addslashes($origin);

		// Get list of players and status
		$q = $this->db->query(
			"SELECT owner AS player, FALSE AS attacking FROM planet "
				. "WHERE id = $planetID AND owner IS NOT NULL "
			. "UNION SELECT DISTINCT owner AS player, attacking FROM fleet "
				. "WHERE location = $planetID AND owner <> {$this->peacekeepers}"
		);

		// Send messages
		while ($r = dbFetchArray($q)) {
			$mid = $this->msgs->call('send', $r[0], 'flmove', array(
				'p_id'		=> $planetID,
				'p_name'	=> $pName
			));
			$this->db->query(
				"INSERT INTO flmove_data VALUES ($mid, '$name', {$this->peacekeepers}, 0, "
					. "{$fSize[0]}, {$fSize[1]}, {$fSize[2]}, $fPower, '{$r[1]}', 't', "
					. "$originID, '$origin')"
			);
		}
	}


	private function checkEvacuations($noCheck) {
		if (empty($noCheck)) {
			$ncQuery = "";
		} else {
			$ncQuery = "AND location NOT IN (" . join(',', $noCheck) . ")";
		}

		// Get PK fleets on planets where no battle is taking place
		$q = $this->db->query(
			"SELECT id FROM fleet "
				. "WHERE owner = {$this->peacekeepers} AND location IS NOT NULL $ncQuery "
				. "FOR UPDATE"
		);
		if (dbCount($q) == 0) {
			return;
		}

		// Merge PK fleets
		$fleets = array();
		while ($r = dbFetchArray($q)) {
			array_push($fleets, $r[0]);
		}
		$fleets = $this->fleets->call('merge', $fleets, array($this->peacekeepers), "");

		// Remove extra fighters and delete empty fleets if required
		$this->db->query(
			"UPDATE fleet SET fighters = (CASE "
					. "WHEN fighters < cruisers * 20 + bcruisers * 10 THEN fighters "
					. "ELSE cruisers * 20 + bcruisers * 10 "
				. "END) WHERE id IN (" . join(',', $fleets) . ")"
		);
		$this->db->query(
			"DELETE FROM fleet WHERE cruisers = 0 AND bcruisers = 0 AND id IN (" . join(',', $fleets) . ")"
		);

		// Fetch all remaining fleets and evacuate them
		$q = $this->db->query(
			"SELECT p.id, p.name, f.id, f.fighters, f.cruisers, f.bcruisers FROM fleet f, planet p "
				. "WHERE p.id = f.location AND f.id IN (" . join(',', $fleets) . ")"
		);
		$locations = array();
		$fleets = array();
		while ($r = dbFetchArray($q)) {
			$this->evacuate($r);
			array_push($locations, $r[0]);
			array_push($fleets, $r[2]);
		}

		// Delete fleets
		if (! empty($fleets)) {
			$this->db->query("DELETE FROM fleet WHERE id IN (" . join(',', $fleets) . ")");
		}

		return $locations;
	}


	private function evacuate($fleetRecord) {
		list($planetID, $planet, $fleet, $fighters, $cruisers, $bCruisers) = $fleetRecord;
		l::trace("Peacekeeper fleet #$fleet leaving planet $planet (#$planetID)");
		$planet = addslashes($planet);
		$fName = addslashes(self::$fleetName);
		$fPower = $this->fleets->call('getPower', $this->peacekeepers, 0, $fighters, $cruisers, $bCruisers);

		// Get list of players and status
		$q = $this->db->query(
			"SELECT owner AS player, FALSE AS attacking FROM planet "
				. "WHERE id = $planetID AND owner IS NOT NULL "
			. "UNION SELECT DISTINCT owner AS player, attacking FROM fleet "
				. "WHERE location = $planetID AND owner <> {$this->peacekeepers}"
		);

		// Send messages
		while ($r = dbFetchArray($q)) {
			$mid = $this->msgs->call('send', $r[0], 'flmove', array(
				'p_id'		=> $planetID,
				'p_name'	=> $planet
			));
			$this->db->query(
				"INSERT INTO flmove_data VALUES ($mid, '$fName', {$this->peacekeepers}, 0, "
					. "$fighters, $cruisers, $bCruisers, $fPower, '{$r[1]}', 'f', NULL, NULL)"
			);
		}
	}
}

?>

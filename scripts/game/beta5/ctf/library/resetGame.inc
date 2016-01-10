<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/ctf/library/resetGame.inc
//
// This function is called when a team wins a round.
//
// It first increases the team's victory status. If the game is finished
// it sends messages, updates the rankings and stops there.
//
// FIXME: incomplete explanation
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_ctf_resetGame {

	private static $ftFields = array('gaships', 'fighters', 'cruisers', 'bcruisers');

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->fleets	= $this->game->getLib('beta5/fleet');
		$this->planets	= $this->game->getLib('beta5/planet');
		$this->players	= $this->game->getLib('beta5/player');
	}

	public function run($winningTeam) {
		// Starts by increasing the team's score
		$cPoints = $this->increaseScore($winningTeam);

		// Did the team win the whole game ?
		if ($cPoints == 100) {
			$this->endGame($winningTeam);
			return;
		}

		// Neutralize all planets that are owned by players of a team in an area allocated to another team
		// as well as planets owned by players in other players' spawning systems
		$this->neutralizeColonies();

		// Reset all planets that had been WHSN'd
		$this->resetWHSN();

		// Remove all WHSN penalties
		$this->removePenalties();

		// Neutralize and consolidate planets in target systems
		$this->neutralizeTargets();

		// And the dead shall rise again ...
		$this->respawnPlayers();

		// Equalize fleet sizes and send the fleets home
		$this->equalizeFleets();

		// Update corruption, happiness and attack status for all planets
		$this->updatePlanets();

		// Send messages
		$this->sendRoundMessages($winningTeam);
	}


	private function increaseScore($winningTeam) {
		$q = $this->db->query("SELECT points FROM ctf_points WHERE team = $winningTeam FOR UPDATE");
		if (dbCount($q)) {
			list($cPoints) = dbFetchArray($q);
			$cPoints = min(100, $cPoints + $this->game->params['v2points']);
			$this->db->query("UPDATE ctf_points SET points = $cPoints WHERE team = $winningTeam");
		} else {
			$cPoints = $this->game->params['v2points'];
			$this->db->query("INSERT INTO ctf_points (team, points) VALUES ($winningTeam, $cPoints)");
		}
		return $cPoints;
	}


	private function endGame($winningTeam) {
		// Fetch the list of players and their teams
		$q = $this->db->query("SELECT id,alliance FROM player");
		while ($r = dbFetchArray($q)) {
			$this->lib->call('message', $r[0], ($r[1] == $winningTeam ? 14 : 15), $winningTeam);
		}

		// Update the rankings
		$this->game->getLib()->call('updateRankings');
	}


	private function neutralizeColonies() {
		// Get the list of all planets to neutralize
		$q = $this->db->query(
			"SELECT p.id, y.id FROM planet p, ctf_alloc a, player y "
				. "WHERE p.owner = y.id AND p.system = a.system "
				  . "AND a.alliance <> y.alliance "
			. "UNION SELECT p.id, y.id FROM planet p, ctf_alloc a, player y "
				. "WHERE p.system = a.system AND y.id = p.owner AND a.alliance = y.alliance "
				. "AND a.spawn_point AND a.player <> y.id"
		);
		// Neutralize the planets
		while ($r = dbFetchArray($q)) {
			$this->planets->call('ownerChange', $r[0]);
			$this->players->call('losePlanet', $r[1], $r[0]);
		}
	}


	private function resetWHSN() {
		// Get the list of WHSN'd planets
		$q = $this->db->query(
			"SELECT p.id FROM planet p, system s WHERE s.nebula = 0 AND p.status <> 0 AND p.system = s.id"
		);
		// Recreate planets instead
		while ($r = dbFetchArray($q)) {
			$this->regenPlanet($r[0]);
		}
	}

	private function regenPlanet($id) {
		$ttn = rand(3, 12);

		do {
			$rn = strtoupper(substr(md5(uniqid(rand())), 0, 7));
			$q = $this->db->query("SELECT id FROM planet WHERE name='P-[$rn]'");
		} while (dbCount($q));

		$q = $this->db->query("SELECT max_pop FROM planet_max_pop WHERE planet = $id AND tech_level = 0");
		list($maxPop) = dbFetchArray($q);

		$this->db->query(
			"UPDATE planet SET status = 0, pop = 2000, ifact = 3, mfact = 3, "
					. "turrets = $ttn, name = 'P-[$rn]', max_pop = $maxPop "
				. "WHERE id = $id"
		);
	}


	private function removePenalties() {
		$this->db->query("UPDATE planet SET bh_unhappiness = 0");
		$this->db->query("UPDATE player SET bh_unhappiness = 0");
	}


	private function neutralizeTargets() {
		// Reset target status
		$this->db->query("UPDATE ctf_target SET held_by = NULL, held_since = NULL, grace_expires = NULL");

		// Get the list of all planets to neutralize
		$q = $this->db->query(
			"SELECT p.id, y.id, p.pop FROM planet p, ctf_target t, player y "
				. "WHERE p.owner = y.id AND p.system = t.system"
		);

		while ($r = dbFetchArray($q)) {
			// Neutralize the planet
			$this->planets->call('ownerChange', $r[0]);
			$this->players->call('losePlanet', $r[1], $r[0]);

			// Compute factories and turrets for the planet
			$x = ($r[2] - 2000) / 48000;
			$facts = floor((($r[2] / 30) - 754 * $x * $x) / 2);
			$turrets = floor(($r[2] / 22) - 510 * $x * $x);

			// Set the planet's factories and turrets, reset its corruption
			$this->db->query(
				"UPDATE planet SET ifact = $facts, mfact = $facts, turrets = $turrets, corruption = 0 "
					. "WHERE id = {$r[0]}"
			);
		}
	}


	private function respawnPlayers() {
		// Get the list of players who don't have planets
		$q = $this->db->query(
			"SELECT id FROM player WHERE NOT hidden AND id NOT IN ("
				. "SELECT DISTINCT owner FROM planet WHERE owner IS NOT NULL)"
		);
		while ($r = dbFetchArray($q)) {
			$this->respawn($r[0]);
		}
	}

	private function respawn($player) {
		// Get the player's initial system
		$q = $this->db->query("SELECT system FROM ctf_alloc WHERE player = $player");
		list($system) = dbFetchArray($q);

		// Choose a random planet
		$orbit = rand(0, 5);
		$q = $this->db->query("SELECT id FROM planet WHERE system = $system AND orbit = $orbit");
		list($planet) = dbFetchArray($q);

		// Assign the planet
		$this->planets->call('ownerChange', $planet, $player);
		$this->players->call('takePlanet', $player, $planet);
	}


	private function equalizeFleets() {
		// Get the list of fleets
		list($pFleets, $aFleets) = $this->getAllFleets();

		// Compute fleet reduction based on alliance fleets
		$fleetReductions = $this->computeReductions($aFleets);

		// Compute the reductions for each player
		foreach ($pFleets as $player => $fleets) {
			$team = array_shift($fleets);
			$pFleets[$player] = $this->playerReduction($fleets, $aFleets[$team], $fleetReductions[$team]);
		}

		// Reinsert each player's fleet at one of his planets
		foreach ($pFleets as $player => $fleet) {
			$this->insertFleet($player, $fleet);
		}
	}

	private function getAllFleets() {
		// Get the list of all fleets
		$q = $this->db->query(
			"SELECT f.id, p.id, p.alliance FROM fleet f, player p WHERE f.owner = p.id"
		);

		// Compute totals and disband the fleets as we go
		$playerFleets = array();
		$allianceFleets = array();
		while ($r = dbFetchArray($q)) {
			list($fleetID, $player, $alliance) = $r;

			if (is_null($playerFleets[$player])) {
				$playerFleets[$player] = array($alliance,0,0,0,0);
			}
			if (is_null($allianceFleets[$alliance])) {
				$allianceFleets[$alliance] = array(0,0,0,0);
			}

			$fleet = $this->fleets->call('get', $fleetID);
			for ($i = 0; $i < 4; $i ++) {
				$playerFleets[$player][$i + 1] += $fleet[self::$ftFields[$i]];
				$allianceFleets[$alliance][$i] += $fleet[self::$ftFields[$i]];
			}

			$this->fleets->call('disband', $fleetID);
		}

		return array($playerFleets, $allianceFleets);
	}

	private function computeReductions($fleets) {
		// Find the smallest values for each type of ship
		$smallest = null;
		foreach ($fleets as $team => $tFleets) {
			if (is_null($smallest)) {
				$smallest = $tFleets;
				continue;
			}

			for ($i = 0; $i < 4; $i ++) {
				if ($tFleets[$i] < $smallest[$i]) {
					$smallest[$i] = $tFleets[$i];
				}
			}
		}

		// Compute reductions for each team
		$reductions = array();
		foreach ($fleets as $team => $tFleets) {
			$reductions[$team] = array();

			for ($i = 0; $i < 4; $i ++) {
				if ($tFleets[$i] == $smallest[$i]) {
					$nAmount = 0;
				} else {
					$rnd = ($smallest == 0) ? rand(101, 105) : rand(98, 105);
					$nAmount = $tFleets[$i] - floor($rnd * $smallest[$i] / 100);
				}

				$reductions[$team][$i] = $nAmount;
			}
		}

		return $reductions;
	}

	private function playerReduction($pFleets, $aFleets, $aReduction) {
		$reduc = array();
		for ($i = 0; $i < 4; $i ++) {
			if ($aFleets[$i] == 0) {
				continue;
			}
			$ratio = $pFleets[$i] / $aFleets[$i];
			$reduction = floor($aReduction[$i] * $ratio);
			$reduc[$i] = $pFleets[$i] - $reduction;
		}
		return $reduc;
	}

	private function insertFleet($player, $fleet) {
		if ($fleet[0] + $fleet[1] + $fleet[2] + $fleet[3] == 0) {
			return;
		}

		// Get a planet belonging to the player
		$q = $this->db->query("SELECT id FROM planet WHERE owner = $player LIMIT 1");
		list($planet) = dbFetchArray($q);

		$qString1 = "INSERT INTO fleet (owner, location";
		$qString2 = ") VALUES ($player, $planet";

		for ($i = 0; $i < 4; $i ++) {
			if ($fleet[$i] == 0) {
				continue;
			}
			$qString1 .= ", " . self::$ftFields[$i];
			$qString2 .= ", {$fleet[$i]}";
		}

		$this->db->query("$qString1$qString2)");
	}


	private function updatePlanets() {
		$this->db->query("UPDATE planet SET corruption = corruption / 2");

		$q = $this->db->query(
			"SELECT p.id FROM planet p, system s "
				. "WHERE s.id = p.system AND s.nebula = 0"
		);

		while ($r = dbFetchArray($q)) {
			$this->planets->call('updateHappiness', $r[0]);
			$this->planets->call('updateMilStatus', $r[0]);
		}
	}


	private function sendRoundMessages($winningTeam) {
		$q = $this->db->query("SELECT id,alliance FROM player");
		while ($r = dbFetchArray($q)) {
			$this->lib->call('message', $r[0], ($r[1] == $winningTeam ? 12 : 13), $winningTeam);
		}
	}
}

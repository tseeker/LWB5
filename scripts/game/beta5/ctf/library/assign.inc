<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/ctf/library/assign.inc
//
// This function assigns a planet to a player in a CTF game.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_ctf_assign {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
	}

	public function run($player, $planetName, $team) {
		logText("Assigning planet $planetName for player #$player in team #$team");

		// Find unassigned systems marked as spawning area for the team
		$system = $this->findFreeSystem($team);

		// Assign a random planet in the system
		$planet = $this->assignPlanet($player, $system, $planetName);

		// Mark the system as the player's spawning point
		$this->markSystem($player, $system);

		return $planet;
	}


	private function findFreeSystem($team) {
		$q = $this->db->query(
			"SELECT s.id FROM system s, ctf_alloc a "
				. "WHERE s.id = a.system AND a.alliance = $team "
				.   "AND a.spawn_point AND NOT s.assigned "
				. "FOR UPDATE OF s, a"
		);

		$n = rand(0, dbCount($q) - 2);
		while ($n) {
			dbFetchArray($q);
			$n --;
		}
		
		list($system) = dbFetchArray($q);
		return $system;
	}


	private function assignPlanet($player, $system, $name) {
		// Randomize orbit
		$npl = 6;
		$porb = rand(0, $npl - 1);

		// Give the planet to the player
		$tm = time();
		$this->db->query("UPDATE planet SET name='$name', owner=$player,renamed=$tm,mod_check=FALSE "
			. "WHERE system = $system AND orbit = $porb");

		// Get planet ID and population
		$q = $this->db->query("SELECT id, pop FROM planet WHERE system = $system AND orbit = $porb FOR UPDATE");
		list($plid, $cPop) = dbFetchArray($q);

		// Update happiness and maximum population
		$this->planets->call('updateHappiness', $plid);
		$this->planets->call('updateMaxPopulation', $plid, null, $player);

		return $plid;
	}


	private function markSystem($player, $system) {
		// Mark the system itself
		$this->db->query("UPDATE system SET assigned = TRUE WHERE id = $system");

		// Mark the allocation record
		$this->db->query("UPDATE ctf_alloc SET player = $player WHERE system = $system");
	}
}

?>

<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/planet/library/restoreNeutral.inc
//
// This function handles a neutral planet by starting to restore it to
// a new player-friendly planet.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_planet_restoreNeutral {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	public function run($planetID) {
		// Get the planet
		$planet = $this->lib->call('byId', $planetID);

		// Compute optimal factories and turrets
		$x = ($planet['pop'] - 2000) / 48000;
		$optFact = ($planet['pop'] / 30) - 754 * $x * $x;
		$optTurrets = ($planet['pop'] / 22) - 510 * $x * $x;

		// Check if the planet is OK already
		$facts = $planet['ifact'] + $planet['mfact'];
		$turrets = $planet['turrets'];
		if ($facts >= $optFact * 0.9 && $facts <= $optFact * 1.1 && $turrets <= 0.6 * $optTurrets) {
			return true;
		}

		// If the planet's factories are outside the range, improve that
		if ($facts < $optFact * 0.9) {
			$facts = $this->buildFactories($planet, $optFact);
		} else if ($facts > $optFact * 1.1) {
			$facts = $this->destroyFactories($planet, $optFact);
		}

		// If the planet's turrets are outside the range, improve that
		if ($turrets > 0.5 * $optTurrets) {
			$turrets = $this->destroyTurrets($planet, $optTurrets * 0.5);
		}

		// Recompute happiness
		$this->lib->call('updateHappiness', $planet['id']);

		return ($facts >= $optFact * 0.9 && $facts <= $optFact * 1.1 && $turrets <= 0.6 * $optTurrets);
	}


	private function buildFactories($planet, $optimal) {
		// Compute how many factories we should build
		$total = $planet['ifact'] + $planet['mfact'];
		$toBuild = ($optimal - $total) * 0.02;

		// Use ratios to determine which types to build
		$mFacts = ceil($toBuild * $planet['ifact'] / $total);
		$iFacts = ceil($toBuild * $planet['mfact'] / $total);

		// Update the planet
		$this->db->query("UPDATE planet SET mfact = mfact + $mFacts, ifact = ifact + $iFacts "
			. "WHERE id = {$planet['id']}");

		// Log it
		logText("NEUTRAL RESTORE (#{$planet['id']}): +$mFacts MFs/$iFacts IFs ($total => $optimal)");

		return $total + $mFacts + $iFacts;
	}


	private function destroyFactories($planet, $optimal) {
		// Compute how many factories we should destroy
		$total = $planet['ifact'] + $planet['mfact'];
		$toDestroy = ($total - $optimal) * 0.05;

		// Use ratios to determine which types to destroy
		$mFacts = ceil($toDestroy * $planet['mfact'] / $total);
		$iFacts = ceil($toDestroy * $planet['ifact'] / $total);

		// Update the planet
		$this->db->query("UPDATE planet SET mfact = mfact - $mFacts, ifact = ifact - $iFacts "
			. "WHERE id = {$planet['id']}");

		// Log it
		logText("NEUTRAL RESTORE (#{$planet['id']}): -$mFacts MFs/$iFacts IFs ($total => $optimal)");

		return $total - ( $mFacts + $iFacts );
	}


	private function destroyTurrets($planet, $target) {
		// Compute the amount of turrets to destroy
		$turrets = $planet['turrets'];
		$toDestroy = ceil(($turrets - $target) * 0.1);

		// Update the planet
		$this->db->query("UPDATE planet SET turrets = turrets - $toDestroy WHERE id = {$planet['id']}");

		// Log it
		logText("NEUTRAL RESTORE (#{$planet['id']}): -$toDestroy turrets ($turrets => $target)");

		return $total - $toDestroy;
	}
}

?>

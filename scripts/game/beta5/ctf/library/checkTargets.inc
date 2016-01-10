<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/ctf/library/checkTargets.inc
//
// This function checks the targets' status and sends messages if needed.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_ctf_checkTargets {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	public function run() {
		// Get the current status (used later to send messages)
		$initialStatus = $this->getStatus();
		$this->grace = ($initialStatus[0] != 'nohold') ? (3600 * $this->game->params['v2grace']) : 0;

		// Get the list of all target systems
		$q = $this->db->query("SELECT * FROM ctf_target");
		$targets = array();
		while ($r = dbFetchHash($q)) {
			array_push($targets, $r);
		}

		// For each target, check the system's status
		foreach ($targets as $target) {
			$this->checkTargetStatus($target);
		}

		// Get the new status
		$newStatus = $this->getStatus();

		// Check status changes
		$this->checkStatusChanges($initialStatus, $newStatus);
	}

	private function getStatus() {
		// Get the target count
		$q = $this->db->query("SELECT COUNT(*) FROM ctf_target");
		list($tCount) = dbFetchArray($q);

		// For each team, get the count of targets held without grace
		$q = $this->db->query(
			"SELECT held_by, COUNT(*) FROM ctf_target "
				   . "WHERE held_by IS NOT NULL AND grace_expires IS NULL "
				. "GROUP BY held_by"
		);
		if (dbCount($q) == 1) {
			list($team, $hCount) = dbFetchArray($q);
			if ($hCount == $tCount) {
				$q = $this->db->query("SELECT MAX(held_since) FROM ctf_target");
				list($heldSince) = dbFetchArray($q);
				return array('held', $team, $heldSince);
			}
		}

		// For each team, get the count of targets held
		$q = $this->db->query(
			"SELECT held_by, COUNT(*) FROM ctf_target "
				   . "WHERE held_by IS NOT NULL "
				. "GROUP BY held_by"
		);
		if (dbCount($q) == 1) {
			list($team, $gCount) = dbFetchArray($q);
			if ($gCount == $tCount) {
				$q = $this->db->query("SELECT MIN(grace_expires) FROM ctf_target");
				list($graceExpires) = dbFetchArray($q);
				return array('grace', $team, $graceExpires);
			}
		}

		return array('nohold');
	}


	private function checkTargetStatus($target) {
		// Get the team for each planet in the system
		$q = $this->db->query(
			"SELECT y.alliance FROM planet p, player y "
				. "WHERE y.id = p.owner AND p.system = {$target['system']}"
		);

		// If all planets are being held, list the alliances
		$teams = array();
		$teamPlanets = array();
		while ($r = dbFetchArray($q)) {
			if (array_key_exists($r[0], $teamPlanets)) {
				$teamPlanets[$r[0]] ++;
			} else {
				$teamPlanets[$r[0]] = 1;
				array_push($teams, $r[0]);
			}
		}

		if (count($teams) == 1 && $teamPlanets[$teams[0]] == 6) {
			// One team is holding the whole system
			$this->heldByTeam($target, $teams[0]);
		} else {
			// The system is not being held by any particular team
			$this->notHeld($target);
		}
	}


	private function heldByTeam($target, $team) {
		$tid = $target['system'];

		if (is_null($target['held_by'])) {
			// If only one team is holding the planets and the system was
			// not marked as being held, mark it
			logText("Target #$tid now held by team #$team");
			$this->assignTarget($target['system'], $team);
		} elseif ($target['held_by'] == $team && ! is_null($target['grace_expires'])) {
			// If the target was being held by the team but had a grace expiration,
			// cancel grace period expiration and increase held_by accordingly
			logText("Target #$tid held by team #$team again, grace cancelled");
			$gracePeriod = time() - $target['grace_expires'] + $this->grace;
			$this->db->query(
				"UPDATE ctf_target "
				. "SET grace_expires = NULL, held_since = held_since + $gracePeriod "
				. "WHERE system = {$target['system']}"
			);
		} elseif ($target['held_by'] != $team && $this->grace > 0) {
			// The target is now being held by another team and the game
			// has support for grace periods
			if (is_null($target['grace_expires'])) {
				// No grace was set - set it
				logText("Target #$tid held by team #$team, setting grace for previous holder #"
					. $target['held_by']);
				$this->setGrace($target['system'], $this->grace);
			} elseif ($target['grace_expires'] <= time()) {
				// Grace has expired, but a new team is holding the system
				logText("Target #$tid now held by team #$team, grace ended for previous holder #"
					. $target['held_by']);
				$this->assignTarget($target['system'], $team);
			}
		} elseif ($target['held_by'] != $team) {
			// The target is now being held by another team, and there is no
			// grace period
			logText("Target #$tid now held by team #$team, no grace for previous holder #"
				. $target['held_by']);
			$this->assignTarget($target['system'], $team);
		}
	}


	private function notHeld($target) {

		// If the target wasn't being held before, we're done
		if (is_null($target['held_by'])) {
			return;
		}

		$tid = $target['system'];
		if ($this->grace > 0) {

			// If there is support for grace periods
			if (is_null($target['grace_expires'])) {
				// Grace period wasn't set - set it
				$this->setGrace($target['system'], $this->grace);
				logText("Target #$tid no longer held by team #{$target['held_by']}, setting grace");
			} elseif ($target['grace_expires'] <= time()) {
				// Grace period expired, no-one's holding the system
				logText("Target #$tid no longer held by team #{$target['held_by']}, grace ended");
				$this->unassignTarget($target['system']);
			}

		} else {
			// No grace periods, no-one's holding the system
			logText("Target #$tid no longer held by team #{$target['held_by']}, no grace");
			$this->unassignTarget($target['system']);
		}
	}


	private function assignTarget($target, $team) {
		$this->db->query(
			"UPDATE ctf_target "
				  . "SET held_by = $team, "
				      . "held_since = UNIX_TIMESTAMP(NOW()), "
				      . "grace_expires = NULL "
				. "WHERE system = $target"
		);
	}

	private function setGrace($target, $grace) {
		$this->db->query(
			"UPDATE ctf_target SET grace_expires = UNIX_TIMESTAMP(NOW()) + $grace WHERE system = $target"
		);
	}

	private function unassignTarget($target) {
		$this->db->query(
			"UPDATE ctf_target "
				  . "SET held_by = NULL, held_since = NULL, grace_expires = NULL "
				. "WHERE system = $target"
		);
	}


	private function checkStatusChanges($initialStatus, $newStatus) {
		$winTime = $this->game->params['v2time'] * 3600;

		// Status hasn't changed
		if ($initialStatus[0] == $newStatus[0] && $initialStatus[1] == $newStatus[1]) {
			// Check for victory / halfway to victory
			if ($initialStatus[0] == 'held') {
				$halfWay = $initialStatus[2] + $winTime / 2;
				$victory = $initialStatus[2] + $winTime;
				$now = time();
				if ($now >= $halfWay && $now < $halfWay + 21) {
					$this->statusMessage(10, 11, $newStatus[1], $victory);
				} elseif ($now >= $victory) {
					// If the game is finished, we shouldn't be here anyways
					$this->lib->call('resetGame', $newStatus[1]);
				}
			}
			return;
		}

		// Status changed, send messages
		logText("CTF Status: (" . join(',',$initialStatus) . ") -> (" . join(',',$newStatus) . ")");

		if ($initialStatus[0] == 'nohold' && $newStatus[0] == 'held') {
			// The targets are now being held by a team
			$this->statusMessage(2, 3, $newStatus[1], $newStatus[2] + $winTime);

		} elseif ($initialStatus[0] == 'grace' && $newStatus[0] == 'nohold') {
			// Targets are no longer being held (no grace period)
			$this->statusMessage(6, 9, $initialStatus[1]);
			$this->cancelAllGraces();

		} elseif ($initialStatus[0] == 'held' && $newStatus[0] == 'nohold') {
			// Targets are no longer being held (no grace period)
			$this->statusMessage(5, 8, $initialStatus[1]);

		} elseif ($initialStatus[0] == 'held' && $newStatus[0] == 'grace') {
			// Targets are no longer being held (with grace period)
			$this->statusMessage(4, 7, $initialStatus[1], $newStatus[2]);

		} elseif ($initialStatus[0] == 'held' && $newStatus[0] == 'held') {
			// Targets are now held by another team
			$this->statusMessage(5, 8, $initialStatus[1]);
			$this->statusMessage(2, 3, $newStatus[1], $newStatus[2] + $winTime);

		} elseif ($initialStatus[0] == 'grace' && $newStatus[0] == 'held') {
			if ($initialStatus[1] != $newStatus[1]) {
				// Other team gained control
				$this->statusMessage(6, 9, $initialStatus[1]);
			}
			$this->statusMessage(2, 3, $newStatus[1], $newStatus[2] + $winTime);
		}
	}


	private function statusMessage($toTeam, $toOthers, $team, $timestamp = null) {
		$q = $this->db->query("SELECT id,alliance FROM player");
		while ($r = dbFetchArray($q)) {
			$this->lib->call('message', $r[0], $r[1] == $team ? $toTeam : $toOthers, $team, $timestamp);
		}
	}

	private function cancelAllGraces() {
		$this->db->query("UPDATE ctf_target SET grace_expires = NULL");
	}
}


?>

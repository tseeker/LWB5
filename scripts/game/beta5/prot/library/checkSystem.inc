<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/prot/library/checkSystem.inc
//
// This function checks a protected system's status and updates the
// Peacekeepers status tables.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_prot_checkSystem {

	private $checkedSystems = array();
	private $peacekeepersID = null;
	private $systemID = null;
	private $ownerID = null;
	private $fleetOwners = null;
	private $pkRecords = null;
	private $pkOffenders = null;
	private $pkEnemies = null;
	private $deleteRecords = null;
	private $actions = null;

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($planetID) {
		// Get the system's ID
		$systemID = $this->getSystem($planetID);
		if ($systemID == -1) {
			return;
		}
		$this->systemID = $systemID;
		l::trace("Checking protection status in system #$systemID");
		$this->now = time();

		// Get the protected player's ID
		$this->fetchProtectedPlayerID();
		if (is_null($this->ownerID)) {
			l::error("No owner found in system #$systemID!");
			$this->db->query("UPDATE system SET prot = 0 WHERE id = $systemID");
			return;
		}

		// Get the ID of the Peacekeepers
		$this->fetchPeacekeepersID();

		// Get the list of all players who have fleets in the system
		$this->fetchFleetOwners();

		// Get the Peacekeeper's records for the system, as well
		// as fleet owners status (offenses and enemy status)
		$this->fetchPKRecords();
		$this->fetchPKOffenders();
		$this->fetchPKEnemies();

		// Update status records using the current list of fleets
		$this->updateRecords();
	}


	private function getSystem($planetID) {
		$q = $this->db->query(
			"SELECT s.id, s.prot FROM planet p "
				. "LEFT JOIN system s ON s.id = p.system "
				. "WHERE p.id = $planetID"
		);

		list($systemID, $protection) = dbFetchArray($q);
		if ($protection == 0 || in_array($systemID, $this->checkedSystems)) {
			return -1;
		}

		array_push($this->checkedSystems, $systemID);
		return $systemID;
	}


	private function fetchProtectedPlayerID() {
		$q = $this->db->query(
			"SELECT owner FROM planet WHERE system = {$this->systemID} AND owner IS NOT NULL LIMIT 1"
		);
		list($this->ownerID) = dbFetchArray($q);
		$this->ownerAllies = null;
	}


	private function fetchPeacekeepersID() {
		if (is_null($this->peacekeepersID)) {
			$this->peacekeepersID = $this->lib->call('getPeacekeepers');
		}
	}


	private function fetchFleetOwners() {
		$q = $this->db->query(
			"SELECT DISTINCT f.owner, f.attacking FROM fleet f "
				. "LEFT JOIN planet p ON p.id = f.location "
				. "WHERE p.system = {$this->systemID} "
				  . "AND f.owner NOT IN ({$this->ownerID}, {$this->peacekeepersID})"
		);

		$result = array();
		while ($r = dbFetchArray($q)) {
			$attack = ($r[1] == 't');
			if (array_key_exists($r[0], $result)) {
				$result[$r[0]] = $result[$r[0]] || $attack;
			} else {
				$result[$r[0]] = $attack;
			}
		}

		$this->fleetOwners = $result;
	}


	private function fetchPKRecords() {
		$q = $this->db->query(
			"SELECT player, status, switch_at FROM pk_sys_status "
				. "WHERE system = {$this->systemID} "
				. "FOR UPDATE"
		);

		$result = array();
		while ($r = dbFetchArray($q)) {
			$result[$r[0]] = array(
				"status"	=> $r[1],
				"switch_at"	=> $r[2]
			);
		}

		$this->pkRecords = $result;
	}


	private function fetchPKOffenders() {
		$this->pkOffenders = array();
		if (empty($this->fleetOwners)) {
			return;
		}

		$q = $this->db->query(
			"SELECT player, nb_offenses FROM pk_offenses "
				. "WHERE player IN (" . join(",", array_keys($this->fleetOwners)) . ") "
				. "FOR UPDATE"
		);

		while ($r = dbFetchArray($q)) {
			$this->pkOffenders[$r[0]] = $r[1];
		}
	}


	private function fetchPKEnemies() {
		$this->pkEnemies = array();
		if (empty($this->fleetOwners)) {
			return;
		}

		$q = $this->db->query(
			"SELECT player FROM pk_enemy "
				. "WHERE player IN (" . join(",", array_keys($this->fleetOwners)) . ") "
				  . "AND until > UNIX_TIMESTAMP(NOW()) "
				. "FOR UPDATE"
		);

		while ($r = dbFetchArray($q)) {
			array_push($this->pkEnemies, $r[0]);
		}
	}


	private function updateRecords() {
		$this->deleteRecords = array();
		$this->actions = array(
			"enemy"	=> array(),
			"other"		=> array()
		);

		// Check all existing records and update them
		foreach ($this->pkRecords as $player => $record) {
			if (array_key_exists($player, $this->fleetOwners)) {
				$this->handleExistingRecord($player, $record);
			} else {
				array_push($this->deleteRecords, $player);
			}
		}

		// Add records for fleets that weren't here before
		foreach ($this->fleetOwners as $player => $attacking) {
			if (! array_key_exists($player, $this->pkRecords)) {
				$this->addRecord($player, $attacking);
			}
		}

		// Destroy records that are no longer necessary
		$this->purgeOldRecords();

		// Send the actions we generated to the main library
		$this->lib->call('addActions', $this->actions);
	}


	private function handleExistingRecord($player, $record) {
		l::trace(" Updating record for player #$player with status {$record['status']}");

		if ($record['status'] == 'E' || $record['status'] == 'O') {
			l::trace("  -> already attacking or enemy");
			return;
		}

		if ($record['status'] == 'W' && !$this->fleetOwners[$player]) {
			l::trace("  => warned player (switch_at = {$record['switch_at']}, now = {$this->now})");
			if ($this->isAlly($player)) {
				l::trace("   -> player is now an allied of the owner");
				$this->addAction($player, 'A', false);
			} elseif ($record['switch_at'] <= $this->now) {
				l::trace("   -> player has been around for too long");
				$this->addAction($player, 'O', false);
			}
			return;
		}

		if ($this->fleetOwners[$player]) {
			l::trace("  -> player has switched to attack");
			$this->increaseOffense($player, $record['status'] == 'W' ? 1 : 2, 'O', false);
			return;
		}

		l::trace("  -> allied player");
		if (! $this->isAlly($player)) {
			l::trace("   => player is no longer an ally of the owner");
			$this->addAction($player, 'W', false);
		}
	}


	private function addRecord($player, $attacking) {
		l::trace(" Adding record for " . ($attacking ? "attacking " : "") . "player #$player");

		if (in_array($player, $this->pkEnemies)) {
			l::trace("  -> player is an enemy");
			$this->addAction($player, 'O', true);
			return;
		}

		if ($attacking) {
			l::trace("  -> player is attacking");
			$this->increaseOffense($player, 2, 'O', true);
			return;
		}

		if ($this->isAlly($player)) {
			l::trace("  -> player is an ally of the owner");
			$this->addAction($player, 'A', true);
		} else {
			l::trace("  -> player must be warned");
			$this->increaseOffense($player, 1, 'W', true);
		}
	}


	private function purgeOldRecords() {
		if (!empty($this->deleteRecords)) {
			$this->db->query(
				"DELETE FROM pk_sys_status "
					. "WHERE system = {$this->systemID} "
					  . "AND player IN (" . join(',', $this->deleteRecords) . ")"
			);
		}
	}


	private function isAlly($player) {
		if (is_null($this->ownerAllies)) {
			$this->fetchOwnerAllies();
		}

		return in_array($player, $this->ownerAllies);
	}


	private function fetchOwnerAllies() {
		$q = $this->db->query(
			"SELECT alliance, a_status FROM player WHERE id = {$this->ownerID}"
		);
		list($alliance, $aStatus) = dbFetchArray($q);

		$qString = "SELECT friend AS ally FROM trusted WHERE player = {$this->ownerID}";
		if (!is_null($alliance) && $aStatus == 'IN') {
			$qString .= " UNION SELECT id AS trusted FROM player "
				. "WHERE a_status = 'IN' AND alliance = $alliance";
		}

		$q = $this->db->query($qString);
		$this->ownerAllies = array();

		while ($q = dbFetchArray($q)) {
			array_push($this->ownerAllies, $r[0]);
		}
	}


	private function increaseOffense($player, $points, $aType, $mustInsert) {
		if (is_null($this->pkOffenders[$player])) {
			$tot = $this->pkOffenders[$player] = $points;
			$this->db->query("INSERT INTO pk_offenses (player, nb_offenses) VALUES ($player, $points)");
		} else {
			$tot = ($this->pkOffenders[$player] + $points);
			$this->db->query(
				"UPDATE pk_offenses SET nb_offenses = nb_offenses + $points WHERE player = $player"
			);
		}

		if ($tot > 6) {
			l::trace("   => PLAYER #$player HAS BEEN DECLARED AN ENEMY");
			$this->declareEnemy($player);
		} else {
			$this->addAction($player, $aType, $mustInsert);
		}

		return ($tot > 6);
	}


	private function addAction($player, $aType, $mustInsert) {
		array_push($this->actions['other'], array(
			'player'	=> $player,
			'system'	=> $this->systemID,
			'type'		=> $aType,
			'mustInsert'	=> $mustInsert
		));
	}


	private function declareEnemy($player) {
		if (!in_array($player, $this->actions['enemy'])) {
			array_push($this->actions['enemy'], $player);
		}
	}
}

?>

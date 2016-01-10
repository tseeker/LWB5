<?php

class beta5_player_library {
	var $pNames	= array();
	var $players	= array();
	var $index	= array (
		'addEnemyAlliance',
		'assign',
		'breakProtection',
		'checkAllies',
		'get',
		'getAllies',
		'getDiploSummary',
		'getEnemies',
		'getEnemyAlliances',
		'getFleets',
		'getName',
		'getPlanetCount',
		'getPlanets',
		'getPlayerId',
		'getPower',
		'getProtectionLevel',
		'getRealPlanetCount',
		'getTAListBans',
		'isAllyOf',
		'isOnVacation',
		'isOnline',
		'isRestrained',
		'lastOnline',
		'makeEnemies',
		'moveAllyDown',
		'moveAllyUp',
		'reassign',
		'reorderAllies',
		'transferFunds'
	);

	function beta5_player_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the name of a player's first planet
	function getFirstPlanet($id) {
		list($name) = dbFetchArray($this->db->query("SELECT l.name FROM player p,planet l WHERE p.id=$id AND l.id=p.first_planet"));
		return $name;
	}

	// Checks whether a player is in another's enemy list
	function isEnemy($pid, $aid) {
		$q = $this->db->query("SELECT COUNT(*) FROM enemy_player WHERE player=$pid AND enemy=$aid");
		list($c) = dbFetchArray($q);
		return ($c > 0);
	}

	// Checks whether an alliance is in a player's enemy list
	function isAllianceEnemy($pid, $aid) {
		$q = $this->db->query("SELECT COUNT(*) FROM enemy_alliance WHERE player=$pid AND alliance=$aid");
		list($c) = dbFetchArray($q);
		return ($c > 0);
	}

	// Remove enemy players from a player's enemy list
	function removeEnemy($pid, $eid) {
		$this->db->query("DELETE FROM enemy_player WHERE player=$pid AND enemy=$eid");
	}

	// Remove enemy alliances from a player's enemy list
	function removeEnemyAlliance($pid, $eid) {
		$this->db->query("DELETE FROM enemy_alliance WHERE player=$pid AND alliance=$eid");
	}

	// Adds a player to the enemy list
	function addEnemy($pid, $eid) {
		$this->db->query("INSERT INTO enemy_player VALUES($pid,$eid)");
		$this->lib->call('makeEnemies', $pid, array($eid));
	}

	// Checks whether a player is in another's ally list
	function isAlly($pid, $aid) {
		$q = $this->db->query("SELECT COUNT(*) FROM trusted WHERE player=$pid AND friend=$aid");
		list($c) = dbFetchArray($q);
		return ($c > 0);
	}

	// Removes an ally from the list
	function removeAlly($pid, $it) {
		$this->db->query("DELETE FROM trusted WHERE player=$pid AND level=$it");
	}

	// Adds an ally to the list
	function addAlly($pid, $aid) {
		$q = $this->db->query("SELECT COUNT(*) FROM trusted WHERE player=$pid");
		list($all) = dbFetchArray($q);
		$this->db->query("INSERT INTO trusted VALUES($pid,$all,$aid)");
	}

	// Adds a player to another's T.A. blacklist
	function addTAListBan($pid, $bpid) {
		$this->db->query("INSERT INTO trusted_ban VALUES($pid, $bpid)");
	}

	// Checks a player's T.A. blacklist for another player
	function checkTAListBan($pid, $bpid) {
		$q = $this->db->query("SELECT COUNT(*) FROM trusted_ban WHERE player=$pid AND ban_player=$bpid");
		if (!$q) {
			return true;
		}
		list($n) = dbFetchArray($q);
		return ($n == 1);
	}

	// Removes a player from another's T.A. blacklist
	function delTAListBan($pid, $bpid) {
		$this->db->query("DELETE FROM trusted_ban WHERE player=$pid AND ban_player=$bpid");
	}

	// Marks a player as quitting
	function setQuit($pid) {
		logText("BETA5/QUIT: player #$pid set to abandon the game");
		$this->db->query("UPDATE player SET quit=".time()." WHERE id=$pid");
		if (is_array($this->lib->mainClass->players[$pid])) {
			$this->lib->mainClass->players[$pid] = null;
		}
	}

	// Marks a player as no longer quitting
	function cancelQuit($pid) {
		logText("BETA5/QUIT: player #$pid no longer set to abandon the game");
		$this->db->query("UPDATE player SET quit=NULL WHERE id=$pid");
		if (is_array($this->lib->mainClass->players[$pid])) {
			$this->lib->mainClass->players[$pid] = null;
		}
	}


	// Updates happiness on all of a player's planets
	function updateHappiness($id) {
		$lib = $this->lib->game->getLib('beta5/planet');
		$pl = $this->lib->call('getPlanets', $id);
		foreach	(array_keys($pl) as $pid) {
			$lib->call('updateHappiness', $pid);
		}
	}


	// Update the player's first_planet if needed when he loses a planet
	function losePlanet($player, $planet) {
		if (is_null($player) || is_null($planet)) {
			return;
		}

		// Fetch the first_planet value
		$q = $this->db->query("SELECT first_planet FROM player WHERE id = $player");
		if (!($q && dbCount($q))) {
			return;
		}
		list($fpID) = dbFetchArray($q);

		// Is it the planet that has just been lost?
		if ($fpID != $planet) {
			return;
		}

		// Does the player has other planets?
		$q = $this->db->query("SELECT id FROM planet WHERE owner = $player AND id <> $planet LIMIT 1");
		if (!($q && dbCount($q))) {
			return;
		}
		list($nFpID) = dbFetchArray($q);

		// Update first_planet
		$this->db->query("UPDATE player SET first_planet = $nFpID WHERE id = $player");
	}

	// Update the player's first_planet if needed when he takes a planet
	function takePlanet($player, $planet) {
		if (is_null($player) || is_null($planet)) {
			return;
		}

		// Fetch the first_planet value
		$q = $this->db->query("SELECT first_planet FROM player WHERE id = $player");
		if (!($q && dbCount($q))) {
			return;
		}
		list($fpID) = dbFetchArray($q);

		// Is it the planet that has just been taken?
		if ($fpID != $planet) {
			return;
		}

		// Does the player still have control on his first planet?
		$q = $this->db->query("SELECT owner FROM planet WHERE id = $fpID AND owner <> $player");
		if (!($q && dbCount($q))) {
			return;
		}

		// Update first planet
		$this->db->query("UPDATE player SET first_planet = $planet WHERE id = $player");
	}
}

?>

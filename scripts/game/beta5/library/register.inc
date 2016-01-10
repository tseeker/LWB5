<?php

class beta5_register {

	function beta5_register($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid, $planet, $nick) {
		// Player mustn't be playing already
		if ($this->lib->call('doesUserPlay', $uid)) {
			return 1;
		}

		// Extract the player's name
		$q = $this->db->query("SELECT name FROM account WHERE id=$uid");
		list($pn) = dbFetchArray($q);
		if (is_null($nick)) {
			$nick = $pn;
		}
		$n = addslashes($nick);

		// Nickname must be unique
		$q = $this->db->query("SELECT name FROM account WHERE (LOWER(name)=LOWER('$n') AND id <> $uid)");
		if (!$q || dbCount($q)) {
			return 2;
		}
		$q = $this->db->query("SELECT name FROM player WHERE LOWER(name)=LOWER('$n')");
		if (!$q || dbCount($q) || ($nick == $pn && $this->lib->call('hasPlayed', $uid))) {
			return 2;
		}

		// Planet name must be unique
		$planetLib = $this->lib->game->getLib('beta5/planet');
		if ($planetLib->call('nameExists', $planet)) {
			return 3;
		}

		// Create player record
		$ni = ($nick == $pn) ? "null" : "'$n'";
		$initCash = $this->lib->game->params['initialcash'];
		$cf = $initCash ? ",cash" : "";
		$cv = $initCash ? ",$initCash" : "";
		$pid = $this->db->query("INSERT INTO player(userid,name$cf) VALUES($uid,$ni$cv)");
		if (!$pid) {
			return 4;
		}

		// Initialize rules
		$this->db->query("INSERT INTO rule (name,player,value) SELECT name,$pid,value FROM rule_def");

		// Initialize rankings
		$rLib = $this->lib->game->getLib('main/rankings');
		$types = array('general','civ','military','financial','idr');
		foreach ($types as $t) {
			$rt = $rLib->call('getType', "p_$t");
			$rLib->call('append', $rt, $nick);
		}

		// Handle locked alliances if required
		if ((int) $this->lib->game->params['lockalliances'] > 1 && ! ($team = $this->lockedAlliances($pid))) {
			return 4;
		}

		if ((int) $this->lib->game->params['usemap'] == 0) {
			// Assign planet random planets for matches and rounds
			$pLib = $this->lib->game->getLib('beta5/player');
			$plid = $pLib->call('assign', $pid, $planet);
		} else {
			// Assign planet according to map specifications for CTF games
			$ctfLib = $this->lib->game->getLib('beta5/ctf');
			$plid = $ctfLib->call('assign', $pid, $planet, $team);

			// Send welcoming message to the player
			$ctfLib->call('message', $pid, 0, $team);
			$ctfLib->call('joinMessage', $team, $pid);
		}
		if (!$plid) {
			return 4;
		}
		$this->db->query("UPDATE player SET first_planet=$plid WHERE id=$pid");

		// Initialize session
		$_SESSION[game::sessName()] = array("player" => $pid);

		return 0;
	}


	private function lockedAlliances($player) {

		// Lock the table
		$this->db->query("LOCK TABLE alliance IN ACCESS EXCLUSIVE MODE");

		// Get the amount of alliances in the database
		$q = $this->db->query("SELECT COUNT(*) FROM alliance");
		if (!($q && dbCount($q))) {
			return 0;
		}
		list($nAlliances) = dbFetchArray($q);

		// Check if there are enough
		$nWanted = (int) $this->lib->game->params['lockalliances'];
		if ($nWanted > $nAlliances) {
			// No, we need to create one
			$rv = $this->createDefaultAlliance($player);
		} else {
			// Yes, have the player join the smallest alliance
			$rv = $this->joinDefaultAlliance($player);
		}

		return $rv;
	}


	private function createDefaultAlliance($player) {

		// Fetch default alliance records that haven't been used yet
		$q = $this->db->query("SELECT tag,name FROM default_alliance WHERE tag NOT IN ("
			. "SELECT tag FROM alliance)");
		if (!($q && dbCount($q))) {
			return false;
		}
		$defaults = array();
		while ($r = dbFetchHash($q)) {
			array_push($defaults, $r);
		}

		// Choose a random alliance tag from the list
		$toCreate = $defaults[rand(0, count($defaults) - 1)];

		// Create the alliance
		$aLib = $this->lib->game->getLib('beta5/alliance');
		$aID = $aLib->call('create', $toCreate['tag'], $toCreate['name'], $player);
		if (is_null($aID)) {
			return false;
		}

		// Make the alliance a democracy
		$aLib->call('setDemocratic', $aID, true);

		return $aID;
	}


	private function joinDefaultAlliance($player) {

		// Get the ID of the smallest alliance
		$q = $this->db->query("SELECT alliance,COUNT(*) AS members FROM player "
			. "WHERE alliance IS NOT NULL GROUP BY alliance "
			. "ORDER BY members,alliance LIMIT 1");
		if (!($q && dbCount($q))) {
			return false;
		}
		list($aID, $junk) = dbFetchArray($q);

		// Have the player join the alliance
		$this->db->query("UPDATE player SET alliance = $aID, a_status = 'IN', a_vote = NULL, a_grade = NULL "
			. "WHERE id = $player");
		return $aID;
	}

}

?>

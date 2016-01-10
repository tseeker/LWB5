<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/getTechList.inc
//
// This function retrieves the technology list for an alliance.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_getTechList {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->players	= $this->game->getLib('beta5/player');
	}

	public function run($alliance, $hasRequests) {
		// Get tech trading privileges for all players in the alliance
		$players = $this->getPlayers($alliance, $hasRequests);

		// Get the technology list for each player that is authorized to submit it,
		// and get requests for players who can submit requests
		$techList = array();
		foreach ($players as $player => $priv) {
			// Ignore unprivileged players
			if ($priv == 0) {
				continue;
			}

			// Get list and player status
			list($lastSub, $list) = $this->getPlayerList($player);
			$techList[$player] = array(
				"submitted"	=> $lastSub,
				"list"		=> $list,
				"vacation"	=> $this->players->call('isOnVacation', $player) ? 1 : 0,
				"restrict"	=> $this->players->call('isRestrained', $player) ? 1 : 0
			);

			// Ignore players who can't submit requess
			if ($priv == 1) {
				continue;
			}
			$techList[$player]['requests'] = $this->lib->call('getTechRequests', $player);
		}

		return $techList;
	}


	private function getPlayers($alliance, $hasRequests) {
		$q = $this->db->query(
			"SELECT p.id, (CASE p.id = a.leader WHEN TRUE THEN 'MR' ELSE r.tech_trade END) "
					. "FROM player p, alliance a, alliance_grade r "
				. "WHERE p.alliance = $alliance AND p.a_status = 'IN' AND r.alliance = p.alliance "
				  . "AND (p.a_grade = r.id OR p.a_grade IS NULL AND r.name IS NULL) "
				  . "AND a.id = p.alliance"
		);

		$players = array();
		while ($r = dbFetchArray($q)) {
			switch ($r[1]) {
				case 'NO': $priv = 0; break;
				case 'SL': $priv = 1; break;
				default: $priv = $hasRequests ? 2 : 1; break;
			}
			$players[$r[0]] = $priv;
		}

		return $players;
	}


	private function getPlayerList($player) {
		$list = array();
		$subAt = 0;

		$q = $this->db->query("SELECT tech, status, submitted FROM tech_trade_list WHERE member = $player");
		while ($r = dbFetchArray($q)) {
			$list[$r[0]] = $r[1];
			$subAt = $r[2];
		}

		return array($subAt, $list);
	}
}

?>

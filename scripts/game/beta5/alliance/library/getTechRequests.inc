<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/getTechRequests.inc
//
// This function returns the list of technology requests for a player.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_getTechRequests {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($player) {
		// Get the player's current requests, if any
		$q = $this->db->query(
			"SELECT tech FROM tech_trade_request "
				. "WHERE player = $player ORDER BY priority"
		);

		$requests = array();
		while ($r = dbFetchArray($q)) {
			array_push($requests, $r[0]);
		}

		return $requests;
	}

}

?>

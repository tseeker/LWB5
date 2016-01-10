<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/setTechRequests.inc
//
// This function updates the list of requests made by a player. It
// should only be called after it has been made sure that the player is
// a member of an alliance, that he can submit requests and that his
// list of requested technologies is valid.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_setTechRequests {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($player, $requests) {
		// Delete the player's requests
		$this->db->query("DELETE FROM tech_trade_request WHERE player = $player");

		// Get the player's alliance; if we're here, then the player *is* in an alliance
		$q = $this->db->query("SELECT alliance FROM player WHERE id = $player");
		list($alliance) = dbFetchArray($q);

		// Delete the player's requests and reinserts them while removing any tech that is now "seen"
		$this->db->query("DELETE FROM tech_trade_request WHERE player = $player");
		$prio = 0;
		foreach ($requests as $req) {
			$this->db->query(
				"INSERT INTO tech_trade_request (alliance, player, priority, tech) "
					. "VALUES ($alliance, $player, $prio, $req)"
			);
			$prio ++;
		}
	}

}

?>

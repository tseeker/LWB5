<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/updateRequests.inc
//
// This function checks whether a player's tech trading requests are ok.
// If they are not, it either deletes or re-arranges them.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_updateRequests {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($player) {
		// Get the player's alliance
		$q = $this->db->query("SELECT alliance,a_status FROM player WHERE id = $player");
		list($alliance, $aStat) = dbFetchArray($q);

		// If the player is not a member of any alliance, remove all requests and leave
		if (is_null($alliance) || $aStat != 'IN ') {
			$this->db->query("DELETE FROM tech_trade_request WHERE player = $player");
			return;
		}

		// Get the player's current requests, if any
		$q = $this->db->query(
			"SELECT alliance, tech FROM tech_trade_request "
				. "WHERE player = $player ORDER BY priority"
		);
		$requests = array();
		while ($r = dbFetchArray($q)) {
			if ($r[0] != $alliance) {
				// Requests are from another alliance; delete them and leave
				$this->db->query("DELETE FROM tech_trade_request WHERE player = $player");
				return;
			}
			array_push($requests, $r[1]);
		}

		// Get all technologies or laws that are at least foreseen
		$q = $this->db->query(
			"SELECT r.id FROM research_player p, research r "
				. "WHERE r.id = p.research AND p.points >= 75 * r.points / 100 AND p.player = $player"
		);
		$seenTechs = array();
		while ($r = dbFetchArray($q)) {
			array_push($seenTechs, $r[0]);
		}

		// Delete the player's requests and reinserts them while removing any tech that is now "seen"
		$this->db->query("DELETE FROM tech_trade_request WHERE player = $player");
		$prio = 0;
		foreach ($requests as $req) {
			if (in_array($req, $seenTechs)) {
				continue;
			}
			$this->db->query(
				"INSERT INTO tech_trade_request (alliance, player, priority, tech) "
					. "VALUES ($alliance, $player, $prio, $req)"
			);
			$prio ++;
		}
	}

}

?>

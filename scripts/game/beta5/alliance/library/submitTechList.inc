<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/submitTechList.inc
//
// This function stores a player's technology list in his alliance's
// tech trading database.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_submitTechList {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}

	public function run($player) {
		// Get alliance
		$pInfo = $this->game->getLib('beta5/player')->call('get', $player);
		$alliance = $pInfo['aid'];

		// Get the player's technologies
		$techLib = $this->game->getLib('beta5/tech');
		$implemented = $techLib->call('getTopics', $player, 1);
		$completed = $techLib->call('getTopics', $player, 0);
		$foreseen = $techLib->call('getTopics', $player, -1);
		$laws = $techLib->call('getLaws', $player);

		// Delete previous tech submission
		$this->db->query("DELETE FROM tech_trade_list WHERE member = $player");

		// Insert technologies
		for ($i = 0; $i < count($laws) / 2; $i ++) {
			$this->db->query(
				"INSERT INTO tech_trade_list (alliance, member, tech, status) "
					. "VALUES ($alliance, $player, {$laws[$i * 2]}, 'L')"
			);
		}
		for ($i = 0; $i < count($completed); $i ++) {
			$this->db->query(
				"INSERT INTO tech_trade_list (alliance, member, tech, status) "
					. "VALUES ($alliance, $player, {$completed[$i]}, 'N')"
			);
		}
		for ($i = 0; $i < count($implemented); $i ++) {
			$this->db->query(
				"INSERT INTO tech_trade_list (alliance, member, tech, status) "
					. "VALUES ($alliance, $player, {$implemented[$i]}, 'I')"
			);
		}
		for ($i = 0; $i < count($foreseen) / 2; $i ++) {
			$this->db->query(
				"INSERT INTO tech_trade_list (alliance, member, tech, status) "
					. "VALUES ($alliance, $player, {$foreseen[$i * 2]}, 'F')"
			);
		}

		// Update request list
		$this->lib->call('updateRequests', $player);
	}
}

?>

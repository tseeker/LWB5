<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/getTechSubmission.inc
//
// This function retrieves technology list submission data from the
// tech trading database and determines whether a player can submit his
// list again.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_getTechSubmission {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}

	public function run($player) {
		$q = $this->db->query(
			"SELECT submitted FROM tech_trade_list WHERE member = $player LIMIT 1"
		);
		if (!dbCount($q)) {
			return array(true, null, null);
		}

		list($when) = dbFetchArray($q);
		$nextSubmission = $when + $this->game->ticks['battle']->interval;
		return array($nextSubmission <= time(), $when, $nextSubmission);
	}
}

?>

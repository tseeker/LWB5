<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/getTechOrder.inc
//
// This function retrieves technology trading orders from the database.
// Depending on the $send parameter, it can fetch either orders to send
// technologies or orders to receive technologies.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_getTechOrder {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($player, $send) {
		$q = $this->db->query(
			"SELECT tech, " . ($send ? "send_to" : "player") . ", submitted, obeyed FROM tech_trade_order "
				. "WHERE " . ($send ? "player" : "send_to") . " = $player "
				. "FOR UPDATE"
		);
		if (!dbCount($q)) {
			return array(null, null, null, null);
		}
		return dbFetchArray($q);
	}
}

?>

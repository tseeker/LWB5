<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// main/library/preJoin.inc
//
// This function lets a player who had registered into the game
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class main_preJoin {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($account) {
		// Get the game's ID
		$q = $this->db->query("SELECT game FROM reg_queue WHERE account = $account");
		if (!($q && dbCount($q))) {
			$this->db->end(false);
			return null;
		}
		list($gameID) = dbFetchArray($q);

		// Get the game library on call its preJoin() function
		$game = config::getGame($gameID);
		if (!$game) {
			$this->db->end(false);
			return null;
		}
		if (!$game->getLib()->call('preJoin', $account)) {
			return null;
		}

		// Delete the queue entry
		$this->db->query("DELETE FROM reg_queue WHERE account = $account");

		return $gameID;
	}
}


?>

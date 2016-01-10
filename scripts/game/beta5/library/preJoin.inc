<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/library/preJoin.inc
//
// This function lets a player who had registered into the game
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_preJoin {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($account) {
		// Get the planet name from the queue
		$q = $this->db->query("SELECT p_name FROM planet_reg_queue WHERE account = $account");
		if (!($q && dbCount($q))) {
			$this->db->end(false);
			return false;
		}
		list($pName) = dbFetchArray($q);

		// Delete the registration entry
		$this->db->query("DELETE FROM planet_reg_queue WHERE account = $account");

		// Register
		if ($this->lib->call('register', $account, $pName, null)) {
			$this->db->end(false);
			return false;
		}

		return true;
	}
}


?>

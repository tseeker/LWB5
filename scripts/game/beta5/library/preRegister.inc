<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/library/preRegister.inc
//
// This function adds an account to the registration queue.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_preRegister {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($account, $planet) {
		$this->db->query(
			"INSERT INTO planet_reg_queue (account, p_name) "
				. "VALUES ($account, '" . addslashes($planet) . "')"
		);
	}
}


?>

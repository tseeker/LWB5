<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/prot/library/isPlayerMarked.inc
//
// This function checks whether a player is marked for destruction by
// the Peacekeepers on a specific planet.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_prot_isPlayerMarked {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($playerID, $planetID) {
		$q = $this->db->query(
			"SELECT p.id FROM planet p, pk_sys_status s "
				. "WHERE p.id = $planetID AND s.system = p.system "
				  . "AND s.status = 'O' AND s.player = $playerID"
		);
		return dbCount($q) == 1;
	}
}

?>

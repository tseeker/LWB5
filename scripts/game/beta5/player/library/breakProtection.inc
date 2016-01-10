<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/player/library/breakProtection.inc
//
// This function causes a player to break from protection by the
// Peacekeepers.
//
// Parameters:
//	$playerID	ID of the player
//	$breakType	Either 'ACT' or 'BRK'
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_player_breakProtection {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->msgs	= $this->lib->game->getLib('beta5/msg');
	}

	public function run($playerID, $breakType) {
		// Get protection level and system ID
		$q = $this->db->query(
			"SELECT s.id, s.prot FROM system s "
				. "WHERE s.id IN (SELECT DISTINCT p.system FROM planet p WHERE p.owner = $playerID) "
				. "FOR UPDATE"
		);
		if (dbCount($q) != 1) {
			return 0;
		}
		list($systemID, $protLevel) = dbFetchArray($q);
		if ($protLevel == 0) {
			return;
		}

		// Set the system's protection level to 0
		$this->db->query("UPDATE system SET prot = 0 WHERE id = $systemID");
		$this->db->query("DELETE FROM pk_sys_status WHERE system = $systemID");

		// Send message
		$this->msgs->call('send', $playerID, 'endprotection', array(
			'end_type'	=> $breakType
		));
	}

}

?>

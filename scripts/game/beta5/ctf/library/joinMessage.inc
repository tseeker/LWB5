<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/ctf/library/joinMessage.inc
//
// This function sends messages to members of a team when another player
// is assigned to it.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_ctf_joinMessage {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($team, $player) {
		// List all players in the team
		$q = $this->db->query(
			"SELECT id FROM player WHERE alliance=$team AND id <> $player"
		);
		if (!($q && dbCount($q))) {
			return;
		}

		// Send a message to each player
		while ($r = dbFetchArray($q)) {
			$this->lib->call('message', $r[0], 1, $team, $player);
		}
	}
}

?>

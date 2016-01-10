<?php

//--------------------------------------------
// "Quit tick": account deletion for quitters
//--------------------------------------------

class beta5_ticks_quit_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
		$this->main	= $lib->game->getLib();
	}


	public function runTick() {
		$this->db->safeTransaction(array($this, 'quit'));
	}

	public function quit() {
		$q = $this->db->query(
			"SELECT id FROM player "
				. "WHERE quit IS NOT NULL AND (UNIX_TIMESTAMP(NOW()) - quit) < 86400 "
				  . "AND (UNIX_TIMESTAMP(NOW()) - quit) > 86339"
		);
		while ($r = dbFetchArray($q)) {
			$this->main->call('leaveGame', $r[0], 'QUIT');
		}
	}
}


?>

<?php

class beta5_player_getName {

	function beta5_player_getName($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the name of a player
	function run($id) {
		if (is_null($id)) {
			logText("****** BUG: beta5::player::getName(null) called");
			l::backtrace();
			return null;
		}
		if (!is_null($this->lib->mainClass->pNames[$id])) {
			return $this->lib->mainClass->pNames[$id];
		}
		$q = $this->db->query(
			"SELECT u.name,p.name FROM account u, player p WHERE p.id=$id AND u.id=p.userid"
		);
		list($an,$pn) = dbFetchArray($q);
		return ($this->lib->mainClass->pNames[$id] = is_null($pn) ? $an : $pn);
	}
}

?>

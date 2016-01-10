<?php

class beta5_player_getPlayerId {
	var $players = array();

	function beta5_player_getPlayerId($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	/** Returns a player's ID using his name to locate him. */
	function run($name) {
		$n = strtolower($name);
		if (is_null($this->players[$n])) {
			$n2 = addslashes($n);
			$q = $this->db->query(
				"SELECT p.id FROM account u, player p "
				. "WHERE (p.quit IS NULL OR UNIX_TIMESTAMP(NOW())-p.quit<86400)"
				. " AND u.id=p.userid AND ((p.name IS NULL AND LOWER(u.name)='$n2') OR LOWER(p.name)='$n2')"
			);
			if (!($q && dbCount($q) == 1)) {
				return null;
			}
			list($this->players[$n]) = dbFetchArray($q);
		}
		return $this->players[$n];
	}
}

?>

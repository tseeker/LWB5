<?php

class beta5_player_getTAListBans {

	function beta5_player_getTAListBans($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of players in a player's T.A. blacklist
	function run($pid) {
		$q = $this->db->query(
			"SELECT l.ban_player,p.name,u.name FROM trusted_ban l,player p,account u "
			. "WHERE l.player=$pid AND p.id=l.ban_player AND u.id=p.userid"
			. " AND (p.quit IS NULL OR UNIX_TIMESTAMP(NOW())-p.quit<86400)"
		);
		$al = array();
		while ($r = dbFetchArray($q)) {
			logText(join(' - ', $r));
			$al[$r[0]] = is_null($r[1]) ? $r[2] : $r[1];
		}
		return $al;
	}
}

?>

<?php

class beta5_player_isAllyOf {

	function beta5_player_isAllyOf($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of players who have a player as a trusted ally
	function run($pid) {
		$q = $this->db->query(
			"SELECT l.player,l.level,p.name,u.name FROM trusted l,player p,account u "
			. "WHERE l.friend=$pid AND p.id=l.player AND u.id=p.userid"
		);
		$al = array();
		while ($r = dbFetchArray($q)) {
			$al[$r[0]] = array('name' => is_null($r[2]) ? $r[3] : $r[2], "level" => $r[1]);
		}
		return $al;
	}
}

?>

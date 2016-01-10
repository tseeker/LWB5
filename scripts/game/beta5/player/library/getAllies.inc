<?php

class beta5_player_getAllies {

	function beta5_player_getAllies($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	// Get the list of a player's trusted allies
	function run($pid) {
		$q = $this->db->query(
			"SELECT l.friend,p.name,u.name FROM trusted l,player p,account u "
			. "WHERE l.player=$pid AND p.id=l.friend AND u.id=p.userid "
			. "ORDER BY level ASC"
		);
		$al = array();
		while ($r = dbFetchArray($q)) {
			array_push($al, array('id' => $r[0], 'name' => is_null($r[1]) ? $r[2] : $r[1]));
		}
		return $al;
	}
}

?>

<?php

class beta5_tech_getLaws {

	function beta5_tech_getLaws($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the list of laws available to a player, as well as their status
	function run($pl) {
		$q = $this->db->query(
			"SELECT r.id, p.in_effect FROM research r, research_player p "
			. "WHERE r.id = p.research AND p.player = $pl AND r.points = p.points AND r.is_law"
		);
		$a = array();
		while ($r = dbFetchArray($q)) {
			array_push($a, $r[0]);
			array_push($a, $r[1]);
		}
		return $a;
	}
}

?>

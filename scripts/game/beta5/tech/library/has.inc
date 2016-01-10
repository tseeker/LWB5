<?php

class beta5_tech_has {

	function beta5_tech_has($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Checks whether a player has researched a technology
	function run($pid, $rid, $bt = false) {
		$cond = $bt ? "p.points >= (75 * r.points / 100)" : "p.points = r.points";
		$qs  = "SELECT ($cond) FROM research_player p, research r ";
		$qs .= "WHERE p.player = $pid AND r.id=p.research AND r.id=$rid";
		$q = $this->db->query($qs);
		list($r) = dbFetchArray($q);
		return ($r == "t");
	}
}

?>

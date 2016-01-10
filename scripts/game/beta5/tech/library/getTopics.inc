<?php

class beta5_tech_getTopics {

	function beta5_tech_getTopics($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the list of research topics for a player; the 'status argument determines
	// the type of list to return: 1 for implemented, 0 for completed, and -1 for "almost
	// completed" (more than 75%)
	function run($pid, $type) {
		$qs  = "SELECT r.id, p.points, r.points FROM research_player p, research r ";
		$qs .= "WHERE p.player = $pid AND r.id = p.research AND ";
		if ($type == 1) {
			$qs .= "NOT r.is_law AND p.in_effect <> 0";
		} elseif ($type == 0) {
			$qs .= "NOT r.is_law AND p.in_effect = 0 AND r.points = p.points";
		} elseif ($type == -1) {
			$qs .= "p.points < r.points AND p.points >= (75 * r.points / 100)";
		}

		$a = array();
		$q = $this->db->query($qs);
		while ($r = dbFetchArray($q)) {
			array_push($a, $r[0]);
			if ($type == -1)
				array_push($a, floor(100 * $r[1] / $r[2]));
		}
		return $a;
	}
}

?>

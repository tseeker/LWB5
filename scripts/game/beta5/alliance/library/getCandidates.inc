<?php

class beta5_alliance_getCandidates {

	function beta5_alliance_getCandidates($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of alliance candidates as well as the number of votes they have
	function run($aid) {
		$l = array();
		$q = $this->db->query("SELECT c.id,c.candidate,COUNT(*) FROM alliance_candidate c,"
			. "player p WHERE c.alliance=$aid AND p.a_vote=c.id GROUP BY c.id, c.candidate");
		while ($r = dbFetchArray($q)) {
			$q2 = $this->db->query("SELECT p.name,a.name FROM player p,account a WHERE p.id={$r[1]} AND p.userid=a.id");
			$r2 = dbFetchArray($q2);
			$l[$r[0]] = array(
				"votes" => $r[2],
				"pid"	=> $r[1],
				"name"	=> is_null($r2[0]) ? $r2[1] : $r2[0]
			);
		}
		return $l;
	}
}

?>

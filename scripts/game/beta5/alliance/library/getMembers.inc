<?php

class beta5_alliance_getMembers {

	function beta5_alliance_getMembers($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Reads the list of alliance members and their ranks
	function run($aid) {
		$q = $this->db->query(
			"SELECT p.id,p.name,a.name,p.a_grade FROM player p,account a "
			. "WHERE p.alliance=$aid AND p.a_status='IN' AND a.id=p.userid");
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0]] = array(
				"name"	=> is_null($r[1]) ? $r[2] : $r[1],
				"rank"	=> $r[3]
			);
		}
		return $rs;
	}
}

?>

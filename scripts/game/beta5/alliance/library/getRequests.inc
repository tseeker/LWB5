<?php

class beta5_alliance_getRequests {

	function beta5_alliance_getRequests($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of pending requests for an alliance
	function run($aid) {
		$q = $this->db->query("SELECT p.id,p.name,a.name FROM player p, account a WHERE p.alliance=$aid AND p.a_status='REQ' AND a.id=p.userid");
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0]] = ($r[1] == "") ? $r[2] : $r[1];
		}
		return $rs;
	}
}

?>

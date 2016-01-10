<?php

class beta5_alliance_getRanks {

	function beta5_alliance_getRanks($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Reads the list of ranks associated with an alliance
	function run($aid) {
		$q = $this->db->query("SELECT id,name FROM alliance_grade WHERE alliance=$aid");
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0]] = is_null($r[1]) ? "-" : $r[1];
		}
		return $rs;
	}
}

?>

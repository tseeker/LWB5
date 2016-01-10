<?php

class main_rankings_getAll {

	function main_rankings_getAll($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($type, $top = null) {
		if (is_null($top)) {
			$qs = "";
		} else {
			$qs = "AND ranking<=$top";
		}
		$rs = array();

		$q = $this->db->query("SELECT id,points,ranking FROM ranking WHERE r_type=$type $qs ORDER BY ranking ASC,id ASC");
		while ($r = dbFetchHash($q)) {
			array_push($rs, $r);
		}

		return $rs;
	}
}

?>

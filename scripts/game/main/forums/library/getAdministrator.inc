<?php

class main_forums_getAdministrator {

	function main_forums_getAdministrator($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid) {
		$q = $this->db->query("SELECT category FROM f_admin WHERE \"user\"=$uid");
		$a = array();
		while ($r = dbFetchArray($q)) {
			if (is_null($r[0])) {
				$q = $this->db->query("SELECT id FROM f_category");
				$a = array();
			} else {
				array_push($a, $r[0]);
			}
		}
		return $a;
	}
}

?>

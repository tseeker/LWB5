<?php

class main_forums_getCategories {

	function main_forums_getCategories($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run() {
		$q = $this->db->query("SELECT id,title,description FROM f_category WHERE title NOT ILIKE '!%!' ORDER BY corder ASC");
		$a = array();
		if ($q) {
			while ($rs = dbFetchHash($q)) {
				array_push($a, $rs);
			}
		}
		return $a;
	}
}

?>

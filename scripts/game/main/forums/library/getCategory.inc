<?php

class main_forums_getCategory {

	function main_forums_getCategory($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($c) {
		$q = $this->db->query("SELECT id,title,description FROM f_category WHERE id=$c");
		if (!$q||dbCount($q)!=1) {
			return null;
		}
		return dbFetchHash($q);
	}
}

?>

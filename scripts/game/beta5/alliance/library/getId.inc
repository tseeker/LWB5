<?php

class beta5_alliance_getId {

	function beta5_alliance_getId($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get alliance identifier by tag
	function run($tag) {
		$q = $this->db->query("SELECT id FROM alliance WHERE LOWER(tag)='" . addslashes(strtolower($tag)) . "'");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($id) = dbFetchArray($q);
		return $id;
	}
}
?>

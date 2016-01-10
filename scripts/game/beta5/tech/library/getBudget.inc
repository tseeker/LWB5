<?php

class beta5_tech_getBudget {

	function beta5_tech_getBudget($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns an array containing research budget allocations
	function run($pid) {
		$q = $this->db->query("SELECT research FROM player WHERE id = $pid");
		if (!($q && dbCount($q))) {
			return array();
		}

		list($b) = dbFetchArray($q);
		return split('!', $b);
	}
}

?>

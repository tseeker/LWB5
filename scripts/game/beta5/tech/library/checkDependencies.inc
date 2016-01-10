<?php

class beta5_tech_checkDependencies {

	function beta5_tech_checkDependencies($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Checks whether a player has already researched a technology's dependencies
	function run($pid, $rid) {
		$q = $this->db->query("SELECT depends_on FROM research_dep WHERE research=$rid");
		if (!dbCount($q)) {
			return true;
		}

		$rv = true;
		while ($rv && $r = dbFetchArray($q)) {
			$rv = $rv && $this->lib->call('has', $pid, $r[0]);
		}
		return $rv;
	}
}

?>

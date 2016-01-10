<?php

class beta5_tech_getPoints {

	function beta5_tech_getPoints($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Returns the amount of research points a player gains daily
	function run($pid) {
		$q = $this->db->query("SELECT SUM(pop) FROM planet WHERE owner = $pid");
		if (!($q && dbCount($q))) {
			return 0;
		}

		list($p) = dbFetchArray($q);
		$r = $this->rules->call('get', $pid);
		return floor($r['research_percent'] * $p / 100);
	}
}

?>

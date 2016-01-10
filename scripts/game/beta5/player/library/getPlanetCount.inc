<?php

class beta5_player_getPlanetCount {
	var $pPlanets = array();

	function beta5_player_getPlanetCount($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the amount of planets a player controls
	function run($pl) {
		if (is_null($pl)) {
			return 1;
		}
		if (!is_null($this->pPlanets[$pl])) {
			return $this->pPlanets[$pl];
		}
		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE owner=$pl");
		list($this->pPlanets[$pl]) = dbFetchArray($q);
		return $this->pPlanets[$pl];
	}
}

?>

<?php

class beta5_player_getPower {
	var $pFleets = array();

	function beta5_player_getPower($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
	}


	// Returns a player's total fleet power
	function run($pid) {
		if (!is_null($this->pFleets[$pid])) {
			return $this->pFleets[$pid];
		}
		$q = $this->db->query("SELECT SUM(gaships),SUM(fighters),SUM(cruisers),SUM(bcruisers) FROM fleet WHERE owner = $pid");
		list($g,$f,$c,$b) = dbFetchArray($q);
		return ($this->pFleets[$pid] = $this->fleets->call('getPower', $pid, $g, $f, $c, $b));
	}
}

?>

<?php

class beta5_planet_buildFactories {

	function beta5_planet_buildFactories($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Build factories on a planet
	function run($id, $nb, $t) {
		$q = $this->db->query("SELECT owner FROM planet WHERE id = $id");
		list($uid) = dbFetchArray($q);
		$ru = $this->rules->call('get', $uid);

		$this->db->query("UPDATE planet SET ${t}fact = ${t}fact + $nb WHERE id = $id");
		$cost = $ru[$t.'f_cost'] * $nb;
		$this->db->query("UPDATE player SET cash = cash - $cost WHERE id = $uid");

		$this->lib->call('updateFactHistory', $id, $t == "m", $nb);
		$this->lib->call('updateHappiness', $id);
	}
}

?>

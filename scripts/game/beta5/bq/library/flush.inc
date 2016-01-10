<?php

class beta5_bq_flush {

	function beta5_bq_flush($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Flush a planet's build queue
	function run($id) {
		$q = $this->db->query("SELECT owner FROM planet WHERE id = $id");
		list($uid) = dbFetchArray($q);
		if (is_null($uid)) {
			return;
		}
		$ru = $this->rules->call('get', $uid);

		$sum = 0;
		$q = $this->db->query("SELECT item,quantity FROM buildqueue WHERE planet = $id");
		while ($r = dbFetchArray($q)) {
			$sum += $r[1] * $ru['build_cost_'.$this->lib->mainClass->types[$r[0]]];
		}

		$this->db->query("DELETE FROM buildqueue WHERE planet = $id");
		$this->db->query("UPDATE player SET cash = cash + $sum WHERE id = $uid");
	}
}

?>

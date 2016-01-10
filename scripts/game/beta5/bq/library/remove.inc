<?php

class beta5_bq_remove {

	function beta5_bq_remove($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Removes an item from a build queue
	function run($pid, $item) {
		$q = $this->db->query("SELECT owner FROM planet WHERE id = $pid");
		list($uid) = dbFetchArray($q);
		$ru = $this->rules->call('get', $uid);

		$q = $this->db->query("SELECT item,quantity FROM buildqueue WHERE planet = $pid AND bq_order = $item");
		if (!dbCount($q)) {
			return;
		}
		list($t,$n) = dbFetchArray($q);
		$cost = $n * $ru['build_cost_'.$this->lib->mainClass->types[$t]];

		$this->db->query("DELETE FROM buildqueue WHERE planet = $pid AND bq_order = $item");
		$this->db->query("UPDATE player SET cash = cash + $cost WHERE id = $uid");
	}
}

?>

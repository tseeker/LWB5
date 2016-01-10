<?php

class beta5_bq_append {

	function beta5_bq_append($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Add items to a planet's build queue
	function run($id, $nb, $type) {
		$q = $this->db->query("SELECT owner FROM planet WHERE id = $id");
		list($uid) = dbFetchArray($q);
		$ru = $this->rules->call('get', $uid);

		$q = $this->db->query("SELECT MAX(bq_order) FROM buildqueue WHERE planet = $id");
		if (dbCount($q)) {
			list($o) = dbFetchArray($q);
			if (is_null($o)) {
				$o = -1;
			}
		} else {
			$o = -1;
		}
		$o ++;

		$this->db->query("INSERT INTO buildqueue VALUES($id, $o, $type, $nb, 0)");
		$cost = $ru['build_cost_'.$this->lib->mainClass->types[$type]] * $nb;
		$this->db->query("UPDATE player SET cash = cash - $cost WHERE id = $uid");
	}
}

?>

<?php

class beta5_bq_getReplacementCost {

	function beta5_bq_getReplacementCost($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}

	// Computes the cost of replacing a set of build queue items
	function run($pid, $items, $rCost) {
		if (count($items) == 0) {
			return 0;
		}

		$q = $this->db->query("SELECT owner FROM planet WHERE id = $pid");
		list($uid) = dbFetchArray($q);
		$ru = $this->rules->call('get', $uid);

		if (count($items) == 1) {
			$wc = "=".$items[0];
		} else {
			$wc = "IN (" . join(',', $items) . ")";
		}
		$q = $this->db->query("SELECT item,quantity FROM buildqueue WHERE planet = $pid AND bq_order $wc");
		if (!dbCount($q)) {
			return 0;
		}

		$sum = 0;
		while ($r = dbFetchArray($q)) {
			$sum -= $r[1] * $ru['build_cost_'.$this->lib->mainClass->types[$r[0]]];
			$sum += $rCost;
		}

		return $sum;
	}
}

?>

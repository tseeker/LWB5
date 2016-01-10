<?php

class beta5_bq_replace {

	function beta5_bq_replace($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}

	// Replaces items in a build queue
	function run($pid, $items, $nb, $type) {
		if (count($items) == 0) {
			return;
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
			return;
		}

		$rCost = $nb * $ru['build_cost_'.$this->lib->mainClass->types[$type]];
		$sum = 0;
		while ($r = dbFetchArray($q)) {
			$sum += $r[1] * $ru['build_cost_'.$this->lib->mainClass->types[$r[0]]];
			$sum -= $rCost;
		}

		$this->db->query("UPDATE buildqueue SET item=$type,quantity=$nb,workunits=0 WHERE planet = $pid AND bq_order $wc");
		$this->db->query("UPDATE player SET cash = cash + ($sum) WHERE id = $uid");
	}
}

?>

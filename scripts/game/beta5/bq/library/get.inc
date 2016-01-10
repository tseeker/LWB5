<?php

class beta5_bq_get {

	function beta5_bq_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Returns the contents of a planet's build queue
	function run($id) {
		$q = $this->db->query("SELECT owner,mfact,corruption FROM planet WHERE id=$id");
		list($uid, $mf, $cl) = dbFetchArray($q);

		$ru = $this->rules->call('get', $uid);
		$mp = $ru['mf_productivity'] * $ru['mf_productivity_factor'] * $mf;
		if ($mp == 0) {
			$mp = 1;
		}
		$cl = $cl / 32000;
		if ($cl > .1) {
			$mp = floor($mp * (1.1 - $cl));
		}

		$q = $this->db->query("SELECT item,quantity,workunits FROM buildqueue WHERE planet = $id ORDER BY bq_order ASC");
		$bq = array();
		$cwu = 0;
		while ($r = dbFetchArray($q)) {
			$i = array(
				"type"		=> $r[0],
				"quantity"	=> $r[1],
				"workunits"	=> $r[2]
			);
			$units = $ru['workunits_'.$this->lib->mainClass->types[$r[0]]] * $r[1] - $r[2];
			$i['units'] = $units;
			$cwu += $units;
			$mod = $units % $mp;
			$ttb = ($units - $mod) / $mp + ($mod ? 1 : 0);
			$i['time'] = $ttb;
			$mod = $cwu % $mp;
			$ttb = ($cwu - $mod) / $mp + ($mod ? 1 : 0);
			$i['ctime'] = $ttb;
			$i['cost'] = $r[1] * $ru['build_cost_'.$this->lib->mainClass->types[$r[0]]];
			array_push($bq, $i);
		}
		return $bq;
	}
}

?>

<?php

class beta5_planet_getIncome {

	function beta5_planet_getIncome($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Computes a planet's income
	function run($owner, $pop = null, $happ = null, $ifact = null, $mfact = null,
			$turrets = null, $corruption = null) {

		if (is_null($pop)) {
			$info = $owner;
			$owner = $info['owner'];
			$pop = $info['pop'];
			$happ = $info['happiness'];
			$ifact = $info['ifact'];
			$mfact = $info['mfact'];
			$turrets = $info['turrets'];
			$corruption = $info['corruption'];
		}

		$r = $this->rules->call('get', $owner);
		if (is_null($r)) {
			return array();
		}
		$biFact = ($happ >= 20) ? 1 : ($happ / 20);
		$bi = floor($pop * $r['base_income'] * $biFact);
		$ii = $ifact * $r['if_productivity'] * $r['if_productivity_factor'];
		$fc = ($ifact + $mfact) * $r['factory_cost'];
		$tc = $turrets * $r['turret_cost'];

		$ti = $bi + $ii;
		$cf = $corruption / 32000;
		if ($cf > .1) {
			$cf = $cf - .1;
			$cl = ceil($ti * $cf);
		} else {
			$cl = 0;
		}

		$tot = $ti - $fc - $tc - $cl;

		return array($tot, $bi, $ii, $fc, $tc, $cl);
	}
}

?>

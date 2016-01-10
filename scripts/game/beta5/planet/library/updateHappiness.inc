<?php

class beta5_planet_updateHappiness {
	var $avgFPower = null;

	function beta5_planet_updateHappiness($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
		$this->rules	= $this->lib->game->getLib('beta5/rules');
		$this->msg	= $this->lib->game->getLib('beta5/msg');
		$this->main	= $this->lib->game->getLib();
	}

	// Updates happiness for a planet
	function run($id) {
		$pinf = $this->lib->call('byId', $id);
		if ($pinf['status'] != 0) {
			return 0;
		}
		$oid = $pinf['owner'];
		$rules = $this->rules->call('get', $oid);

		if (!is_null($oid)) {
			$q = $this->db->query("SELECT bh_unhappiness FROM player WHERE id=$oid");
			list($bhu) = dbFetchArray($q);
		} else {
			$bhu = 0;
		}
		$uf = ($bhu * 7 + $rules['unhappiness_factor']) / 100;

		// Factory happiness
		$x = ($pinf['pop'] - 2000) / 48000;
		$optFact = ($pinf['pop'] / 30) - 754 * $x * $x; $bFHap = 0.9;
		$x = $pinf['mfact'] + $pinf['ifact'];
		if ($x <= $optFact) {
			$tmp = ($bFHap - 1) / $optFact;
			$hapFact = $x * $x * $tmp / $optFact - 2 * $x * $tmp + $bFHap;
		} elseif ($x <= 3 * $optFact) {
			$tmp = 4 * $optFact;
			$hapFact = - $x * $x / (2 * $tmp * $optFact) + $x / $tmp + 7 / 8;
		} else {
			$tmp = 2 * ($x - 3 * $optFact) / $optFact;
			if ($tmp < 500) {
				$hapFact = 1 - exp($tmp) / (exp($tmp) + 1);
			} else {
				$hapFact = 0;
			}
		}

		// Turret happiness
		$x = ($pinf['pop'] - 2000) / 48000;
		$optTurrets = ($pinf['pop'] / 22) - 510 * $x * $x; $bTHap = 0.7;
		$x = $pinf['turrets'];
		if ($x <= $optTurrets) {
			$tmp = ($bTHap - 1) / $optTurrets;
			$hapTurrets = $x * $x * $tmp / $optTurrets - 2 * $x * $tmp + $bTHap;
		} elseif ($x <= 4 * $optTurrets) {
			$tmp = 9 * $optTurrets;
			$hapTurrets = - $x * $x / (2 * $tmp * $optTurrets) + $x / $tmp + 17 / 18;
		} else {
			$tmp = 2 * ($x - 4 * $optTurrets) / $optTurrets;
			if ($tmp < 500) {
				$hapTurrets = 1 - exp($tmp) / (exp($tmp) + 1);
			} else {
				$hapTurrets = 0;
			}
		}
		$hapTurrets = (1 + $hapTurrets * 2) / 3;

		// Get average fleet power
		if (is_null($this->avgFPower)) {
			$q = $this->db->query("SELECT SUM(gaships),SUM(fighters),SUM(cruisers),SUM(bcruisers) FROM fleet");
			list($g,$f,$c,$b) = dbFetchArray($q);
			$np = $this->main->call('getPlayerCount');
			if ($np) {
				$g /= $np; $f /= $np; $c /= $np; $b /= $np;
				$fpAvg = $g * 5 + $f * 10 + $c * 40 + $b * 80;
			} else {
				$fpAvg = 0;
			}
			$this->avgFPower = $fpAvg;
		} else {
			$fpAvg = $this->avgFPower;
		}

		// Get total and local fleet powers
		if (!is_null($oid)) {
			$q = $this->db->query("SELECT SUM(gaships),SUM(fighters),SUM(cruisers),SUM(bcruisers) FROM fleet WHERE location=$id AND owner=$oid");
			list($g,$f,$c,$b) = dbFetchArray($q);
			$fpLoc = $this->fleets->call('getPower', $oid, $g, $f, $c, $b);
			$fpTot = $this->players->call('getPower', $oid);
		} else {
			$fpLoc = $fpTot = 0;
		}

		// Fleet happiness
		if ($fpAvg) {
			$hapFTot = exp($fpTot / $fpAvg) / (exp($fpTot / $fpAvg) + 1);
			if ($fpTot > 0) {
				$hapFLoc = $hapFTot * $fpLoc / $fpTot;
			} else {
				$hapFLoc = 0;
			}
		} else {
			$hapFLoc = 0;
		}
		$hapFLoc = (9 + $hapFLoc) / 10;

		// Compute local happiness
		$hapTot  = 100 * $hapFact * $hapTurrets * $hapFLoc;
		$hapTot -= $hapTot * $pinf['bh_unhappiness'] / 30;
		$hapTot  = max(0,min(100,round($hapTot)));

		// Compute modifier from planet count and unhappiness factor
		$x = min($this->players->call('getPlanetCount', $oid), 40);
		$pcMod = pow(1 - 0.63 * exp(($x-18)/2) / (exp(($x-18)/2)+1) - log($x) / 10, $uf*$uf);
		$hapTot  = round($hapTot * $pcMod);
		if ($hapTot == 'NAN') {
			$hapTot = 0;
		}

		// Check for revolt
		if (!is_null($pinf['owner'])) {
			$send = false;
			if ($pinf['happiness'] >= 20 && $hapTot < 20) {
				$rStat = 'TRUE';
				$send = true;
			} elseif ($pinf['happiness'] < 20 && $hapTot >= 20) {
				$rStat = 'FALSE';
				$send = true;
			}
			if ($send) {
				$tm = time();
				$player = $pinf['owner'];
				$this->msg->call('send', $player, 'revolt', array(
					'planet'	=> $id,
					'pname'		=> $pinf['name'],
					'started'	=> $rStat
				));
			}
		}

		$this->db->query("UPDATE planet SET happiness=$hapTot WHERE id=$id");
		return $hapTot;
	}
}

?>

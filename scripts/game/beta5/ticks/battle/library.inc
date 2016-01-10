<?php

//-------------------------------------------------------------------
// "Battle tick": computes battle outcomes
//-------------------------------------------------------------------

class beta5_ticks_battle_library {

	function beta5_ticks_battle_library($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
		$this->fleets	= $lib->game->getLib('beta5/fleet');
		$this->msgs	= $lib->game->getLib('beta5/msg');
		$this->planets	= $lib->game->getLib('beta5/planet');
		$this->players	= $lib->game->getLib('beta5/player');
		$this->rules	= $lib->game->getLib('beta5/rules');
		$this->sales	= $lib->game->getLib('beta5/sale');
		$this->rankings	= $lib->game->getLib('main/rankings');
	}


	function runTick() {
		$this->idrInc = array();
		$toThrow = null;
		try {
			$locations = $this->db->safeTransaction(array($this, 'getBattleLocations'));
			foreach ($locations as $lId) {
				$this->db->safeTransaction(array($this, 'battleAt'), array($lId), 5);
			}
		} catch (Exception $e) {
			$toThrow = $e;
		}

		$this->db->safeTransaction(array($this, 'updateStatus'));
		if (!is_null($toThrow)) {
			throw $toThrow;
		}

		$this->db->safeTransaction(array($this, 'updateRankings'));
	}


	public function getBattleLocations() {
		$q = $this->btQuery(
			"SELECT DISTINCT location FROM fleet WHERE location IS NOT NULL AND attacking"
		);
		$locations = array();
		while ($r = dbFetchArray($q)) {
			array_push($locations, $r[0]);
		}
		return $locations;
	}


	public function updateStatus() {
		// Mark fleets that were in battle as available and commit
		$this->btQuery("UPDATE fleet SET can_move='Y' WHERE can_move='B'");
	}

	public function updateRankings() {
		// Increase inflicted damage points
		$rt = $this->rankings->call('getType', 'p_idr');
		$rl = $this->rankings->call('getAll', $rt);
		$idr = array();
		foreach ($rl as $r) {
			$idr[$r['id']] = $r['points'];
		}
		foreach	($this->idrInc as $n => $inc) {
			$idr[$n] += $inc;
		}
		$idrR = array();
		foreach	($idr as $n => $p) {
			if (!is_array($idrR[$p])) {
				$idrR[$p] = array();
			}
			array_push($idrR[$p], $n);
		}
		$this->rankings->call('update', $rt, $idrR);
	}


	public function battleAt($lId) {
		// Execute computations
		$idrInc = $this->battleComputation($lId);

		// Increase stored IDR
		foreach ($idrInc as $n => $ii) {
			if (is_null($this->idrInc[$n])) {
				$this->idrInc[$n] = $ii;
			} else {
				$this->idrInc[$n] += $ii;
			}
		}
	}


	private function battleComputation($lId) {
		static $rnames = array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');
		static $addLosses = array(.5, .25, 1, .25, .125);

		// Get the ID of the planet's owner and the amount of turrets
		$q = $this->btQuery("SELECT name,owner,turrets,vacation FROM planet WHERE id=$lId "
			. "FOR UPDATE");
		list($pName,$pOwner,$pTurrets,$vacation) = dbFetchArray($q);
		$poFake = is_null($pOwner) ? 0 : $pOwner;

		// Compute the power of the planet's turrets
		if ($vacation == 'YES ') {
			$defPower = 0;
			$pTurrets = 0;
		} else {
			$defPower = $this->planets->call('getPower', $lId);
		}
		$turretPower = $defPower;

		// Get all fleets at that location
		$q = $this->btQuery("SELECT * FROM fleet WHERE location=$lId FOR UPDATE");
		$attFleets = array(0,0,0,0,0,0,0,0);
		$defFleets = array(0,0,0,0,0,0,0,0);
		$attPlayers = $defPlayers = 0;
		$fleets = array();
		$players = array(
			$poFake	=> array(false, $turretPower, $pTurrets, 0, 0, 0, 0, array())
		);
		$attPower = 0;
		$cSales = null;

		// Extract fleet data
		while ($r = dbFetchHash($q)) {
			if ( ($r['owner'] == $pOwner && $vacation == 'YES ')
			  || ($r['attacking'] == 't' && $r['time_spent'] < 15) ) {
				continue;
			}

			$r['power'] = $this->fleets->call('getPower', $r['owner'], $r['gaships'],
				$r['fighters'], $r['cruisers'], $r['bcruisers']);
			if ($r['attacking'] == 't') {
				$attFleets[0] += $r['gaships'];
				$attFleets[1] += $r['fighters'];
				$attFleets[2] += $r['cruisers'];
				$attFleets[3] += $r['bcruisers'];
				$attPower += $r['power'];
			} else {
				$defFleets[0] += $r['gaships'];
				$defFleets[1] += $r['fighters'];
				$defFleets[2] += $r['cruisers'];
				$defFleets[3] += $r['bcruisers'];
				$defPower += $r['power'];
			}

			if (is_null($players[$r['owner']])) {
				$players[$r['owner']] = array(
					$r['attacking'] == 't', $r['power'], 0, $r['gaships'],
					$r['fighters'], $r['cruisers'], $r['bcruisers'], array()
				);
				if ($r['attacking'] == 't') {
					$attPlayers ++;
				} else {
					$defPlayers ++;
				}
			} else {
				$players[$r['owner']][1] += $r['power'];
				$players[$r['owner']][3] += $r['gaships'];
				$players[$r['owner']][4] += $r['fighters'];
				$players[$r['owner']][5] += $r['cruisers'];
				$players[$r['owner']][6] += $r['bcruisers'];
			}

			array_push($players[$r['owner']][7], $r['id']);
			$fleets[$r['id']] = $r;
		}

		// If there is no defense at all, forget it
		// (that can also mean that the local defense fleet is on vacation)
		// Also skip if there are no attack fleets (could happen because of separate transactions)
		if ($defPower == 0 || $attPower == 0) {
			l::debug("Skipped battle on $pName, no fleets");
			return array();
		}
		l::debug("Starting battle on $pName (owner: $poFake); $pTurrets turrets, vacation=$vacation");

		// Compute the damage index, which is the proportion of damage inflicted by the biggest fleet
		$dRandom = $defPower;
		while ($dRandom > 1000) {
			$dRandom = $dRandom / 10;
		}
		$rdPower = $defPower + rand(0, round($dRandom * 0.05));
		l::debug("Defense: power= $defPower ; random= $dRandom; rPower= $rdPower");

		$aRandom = $attPower;
		while ($aRandom > 1000) {
			$aRandom = $aRandom / 10;
		}
		$raPower = $attPower + rand(0, round($aRandom * 0.05));
		l::debug("Attack: power= $attPower ; random= $aRandom; rPower= $raPower");

		$bigDef = ($rdPower > $raPower);
		$bigPower = $bigDef ? $rdPower : $raPower;
		$smallPower = $bigDef ? $raPower : $rdPower;
		$ratio = $bigPower / $smallPower;
		$damageIndex = ($ratio > 10 ? 1 : ((exp($ratio - 1) / (exp($ratio - 1) + 1))));
		$attDamage = round(($bigDef ? $damageIndex : (1 - $damageIndex)) * $attPower);
		$defDamage = round(($bigDef ? (1-$damageIndex) : $damageIndex) * $defPower);
		l::debug("Damage index: $damageIndex (attDamage: $attDamage ; defDamage: $defDamage)");

		// Handle heroic defense
		if (! $bigDef && $ratio >= 5 && rand(0, 10000) <= 200) {
			$heroicDefense = true;
			$addDamage = ceil($smallPower * rand(300, 400) / 100);
			if ($addDamage > $bigPower / 5) {
				$addDamage = ceil($bigPower / 5);
			}
			$attDamage += $addDamage;
			l::debug("Heroic defense! Damage increased by $addDamage (-> $attDamage)");
		} else {
			$heroicDefense = false;
		}

		// Compute the amount of damage to each player
		$defLosses = $attLosses = 0;
		$plist = array_keys($players);
		$turretLoss = $tPowerLoss = 0;
		foreach ($plist as $id) {
			l::debug(" -> Player $id");
			if	($players[$id][0])
				$losses = ($players[$id][1] / $attPower) * $attDamage;
			else
				$losses = ($players[$id][1] / $defPower) * $defDamage;
			l::debug("     * losses = $losses");

			$rules = $this->rules->call('get', ($id == 0) ? null : $id);
			if ($damageIndex < 1 || ($players[$id][0] && !$bigDef || !$players[$id][0] && $bigDef)) {
				$losses = ($losses / 100) * $rules['battle_losses'];
				l::debug("     * losses = $losses after adjustment ({$rules['battle_losses']}%)");
			} else {
				l::debug("     * losses not adjusted");
			}

			// Compute damage for each type of ship
			$probLoss = array(0, 0, 0, 0, 0); $oriFleet = array();
			if ($players[$id][1] > 0) {
				$sPowers = array(); $power = array();
				$tLoss = 0;
				$lossRatio = $losses / $players[$id][1];
				for ($i=0;$i<5;$i++) {
					$sPowers[$i] = ($rules[$rnames[$i] . "_power"] / 100)
						* $rules['effective_fleet_power'];
					$oriFleet[$i] = $players[$id][$i+2];
					$power[$i] = $sPowers[$i] * $players[$id][$i+2];
					$shipRatio = $power[$i] / $players[$id][1];
					$pLoss = $shipRatio * $losses;
					$probLoss[$i] = min($players[$id][$i+2], round($pLoss / $sPowers[$i]));
					$tLoss += $probLoss[$i] * $sPowers[$i];
				}

				$i = $n = 0;
				while ($tLoss < $losses && $n < 5) {
					if ($probLoss[$i] < $players[$id][$i+2]) {
						$probLoss[$i] ++;
						$tLoss += $sPowers[$i];
						$n = 0;
					} else {
						$n++;
					}
					$i = ($i + 1) % 5;
				}
			}
			l::debug("     * ship losses (T/G/F/C/B) = " . join(', ', $probLoss)
				. " out of " . join(', ', $oriFleet));

			// If there are turret losses, remove turrets
			if ($probLoss[0] > 0) {
				$turretLoss = $probLoss[0];
				$tPowerLoss = $this->planets->call('getPower', $pOwner, $turretLoss);
				$this->btQuery("UPDATE planet SET turrets=turrets-$turretLoss WHERE id=$lId");
				$tm = time();
				$this->btQuery("DELETE FROM turhist WHERE $tm-moment>86400");
				$this->btQuery("INSERT INTO turhist VALUES ($lId,$tm,-$turretLoss)");

				// Mark the planet's sale to be cancelled if that applies
				$q = $this->btQuery("SELECT id,player,finalized,sold_to FROM sale WHERE planet=$lId");
				if (($r = dbFetchArray($q)) && is_null($cSales)) {
					$cSales = $r;
				}
			}

			// Apply losses to the player's individual fleets
			$removed = array(0, 0, 0, 0);
			foreach	($players[$id][7] as $fid) {
				$rff = $tif = 0;
				$rem = array();
				for ($i=0;$i<4;$i++) {
					// Ships that must be destroyed
					$toRemove = $probLoss[$i+1] - $removed[$i];

					// Ships in the fleet
					$inFleet = $fleets[$fid][$rnames[$i+1]."s"];
					$tif += $inFleet;

					// Remove ships
					$fromFleet = min($toRemove, $inFleet);
					$removed[$i] += $fromFleet;
					$rff += $fromFleet;
					$rem[$i] = $fromFleet;
				}
				l::debug("     * fleet $fid losses (G/F/C/B) = " . join(', ', $rem));

				// Mark the fleet's sale to be cancelled if that applies
				if ($rff) {
					$q = $this->btQuery("SELECT id,player,finalized,sold_to FROM sale WHERE fleet=$fid");
					if ($r = dbFetchArray($q)) {
						if (is_null($r[2])) {
							$ga = 'cancel';
						} else {
							$ga = 'cancelTransfer';
						}
						$this->sales->call($ga, $r[1], $r[0]);

						// FIXME: send messages
						if (!is_null($cSales) && $cSales == $r[0]) {
							$cSales = null;
						}
					}
				}

				if ($rff == $tif) {
					// The whole fleet has been lost
					$this->btQuery("DELETE FROM fleet WHERE id=$fid");
				} elseif ($rff) {
					// Fleet has suffered some losses
					$qs = "UPDATE fleet SET ";
					$qsi = false;
					for ($i=0;$i<4;$i++) {
						if ($rem[$i] == 0) {
							continue;
						}
						if ($qsi) {
							$qs .= ",";
						} else {
							$qsi = true;
						}
						$qs .= $rnames[$i+1]."s=".$rnames[$i+1]."s-".$rem[$i];
					}
					$qs .= " WHERE id=$fid";
					$this->btQuery($qs);
				} else {
					// No losses, we're done
					break;
				}
			}

			// Add losses to the correct array
			if ($players[$id][0]) {
				for ($i=0;$i<4;$i++) {
					$attFleets[$i+4] += $probLoss[$i+1];
				}
			} else {
				for ($i=0;$i<4;$i++) {
					$defFleets[$i+4] += $probLoss[$i+1];
				}
			}

			// Store the player's losses
			$lostPower  = $this->fleets->call('getPower', ($id==0 ? null : $id), $probLoss[1], $probLoss[2], $probLoss[3], $probLoss[4]);
			$lostPower += $this->planets->call('getPower', ($id==0 ? null : $id), $probLoss[0]);
			array_shift($probLoss);
			$players[$id][8] = $probLoss;
			$players[$id][9] = $lostPower;
			if ($players[$id][0]) {
				$attLosses += $lostPower;
			} else {
				$defLosses += $lostPower;
			}
		}

		// Cancel the planet's sale if it suffered damage
		if (!is_null($cSales)) {
			if (is_null($cSales[2])) {
				$ga = 'cancel';
			} else {
				$ga = 'cancelTransfer';
			}
			$this->sales->call($ga, $cSales[1], $cSales[0]);
			// FIXME: send messages
		}

		// Give the players "inflicted damage" points
		$idrInc = array();
		foreach ($plist as $id) {
			if ($id == 0) {
				continue;
			}

			$q = $this->btQuery("SELECT hidden FROM player WHERE id = $id");
			list($hidden) = dbFetchArray($q);
			if ($hidden == 't') {
				continue;
			}

			if ($players[$id][0]) {
				$tPower = $attPower;
				$eLoss = $defLosses;
			} else {
				$tPower = $defPower;
				$eLoss = $attLosses;
			}
			$ii = round(($players[$id][1] / $tPower) * $eLoss);
			$n = $this->players->call('getName', $id);
			if (is_null($idrInc[$n])) {
				$idrInc[$n] = $ii;
			} else {
				$idrInc[$n] += $ii;
			}
		}

		// Send battle reports
		$tm = time();
		foreach	($plist as $id) {
			// Avoid the fake "neutral" player
			if ($id == 0) {
				continue;
			}
			$p = $players[$id];

			// Get friendly/hostile data
			if ($players[$id][0]) {
				$fPower  = $attPower;
				$ePower  = $defPower;
				$fLosses = $attLosses;
				$eLosses = $defLosses;
				$fFleets = $attFleets;
				$eFleets = $defFleets;
				$tMode   = ($pTurrets != 0 ? 3 : 0);
			} else {
				$fPower  = $defPower;
				$ePower  = $attPower;
				$fLosses = $defLosses;
				$eLosses = $attLosses;
				$fFleets = $defFleets;
				$eFleets = $attFleets;
				$tMode   = ($pTurrets != 0 ? ($pOwner == $id ? 1 : 2) : 0);
			}

			// Remove the player's own statistics from the list of friendlies
			$fPower -= $p[1]; $fLosses -= $p[9];
			for ($i=0;$i<4;$i++) {
				$fFleets[$i] -= $p[$i+3];
				$fFleets[$i+4] -= $p[8][$i];
			}

			// Send battle report
			$this->msgs->call('send', $id, "battle", array(
				"planet_id"	=> $lId,
				"planet"	=> $pName,
				"o_gaships"	=> $p[3],
				"o_fighters"	=> $p[4],
				"o_cruisers"	=> $p[5],
				"o_bcruisers"	=> $p[6],
				"o_power"	=> $p[1],
				"ol_gaships"	=> $p[8][0],
				"ol_fighters"	=> $p[8][1],
				"ol_cruisers"	=> $p[8][2],
				"ol_bcruisers"	=> $p[8][3],
				"ol_power"	=> $p[9],
				"a_gaships"	=> $fFleets[0],
				"a_fighters"	=> $fFleets[1],
				"a_cruisers"	=> $fFleets[2],
				"a_bcruisers"	=> $fFleets[3],
				"a_power"	=> $fPower,
				"al_gaships"	=> $fFleets[4],
				"al_fighters"	=> $fFleets[5],
				"al_cruisers"	=> $fFleets[6],
				"al_bcruisers"	=> $fFleets[7],
				"al_power"	=> $fLosses,
				"e_gaships"	=> $eFleets[0],
				"e_fighters"	=> $eFleets[1],
				"e_cruisers"	=> $eFleets[2],
				"e_bcruisers"	=> $eFleets[3],
				"e_power"	=> $ePower,
				"el_gaships"	=> $eFleets[4],
				"el_fighters"	=> $eFleets[5],
				"el_cruisers"	=> $eFleets[6],
				"el_bcruisers"	=> $eFleets[7],
				"el_power"	=> $eLosses,
				"turrets"	=> $pTurrets,
				"tpower	"	=> $turretPower,
				"l_turrets"	=> $turretLoss,
				"l_tpower"	=> $tPowerLoss,
				"tmode"		=> $tMode,
				"heroic_def"	=> $heroicDefense ? ($players[$id][0] ? -1 : 1) : 0
			));
		}

		// Update happiness and attack status
		$this->planets->call('updateHappiness', $lId);
		$this->planets->call('updateMilStatus', $lId);

		// If the planet was pending entrance in vacation mode and all enemy
		// fleet is dead, set it to vacation mode.
		if ($vacation == 'PEND') {
			$q = $this->btQuery("SELECT COUNT(*) FROM fleet WHERE location=$lId AND attacking");
			if ($q && dbCount($q) == 1) {
				list($c) = dbFetchArray($q);
				if ($c == 0) {
					$this->btQuery("UPDATE planet SET vacation='YES' WHERE id=$lId");
				}
			}
		}

		return $idrInc;
	}


	function btQuery($q) {
		$r = $this->db->query($q);
		l::trace("Result '$r' for query: $q");
		return $r;
	}
}

?>

<?php

//-------------------------------------------------------------------
// "Hour tick": owner change, production, stuck fleets, HS stand-by,
//	revolts, wormholes, sales
//-------------------------------------------------------------------

class beta5_ticks_hour_library {

	var $idrInc	= array();
	var $oChange	= array();

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->main	= $this->game->getLib();
		$this->bq	= $this->game->getLib('beta5/bq');
		$this->fleets	= $this->game->getLib('beta5/fleet');
		$this->msg	= $this->game->getLib('beta5/msg');
		$this->planets	= $this->game->getLib('beta5/planet');
		$this->players	= $this->game->getLib('beta5/player');
		$this->rankings	= $this->game->getLib('main/rankings');
		$this->rules	= $this->game->getLib('beta5/rules');
		$this->sales	= $this->game->getLib('beta5/sale');
		$this->standby	= $this->game->getLib('beta5/standby');
	}


	public function runTick() {
		// Planetary events
		l::debug("Handling planetary events");
		$handledSystems = 0;
		do {
			$increase = $this->db->safeTransaction(
				array($this, 'handleSystemGroup'), $handledSystems
			);
			$handledSystems += $increase;
		} while ($increase > 0);

		// Abandon planets
		$this->db->safeTransaction(array($this, "handleAbandon"));

		// Stuck fleets
		$this->db->safeTransaction(array($this, 'unstickFleets'));

		// Fleets waiting in hyperspace
		$this->db->safeTransaction(array($this, 'hyperspaceStandby'));

		// Fleet sales
		$this->db->safeTransaction(array($this, 'fleetSales'));

		// Update happiness
		$this->db->safeTransaction(array($this, 'updateHappiness'));

		// Increase inflicted damage points for WHSN'd fleets
		if (count($this->idrInc)) {
			$this->db->safeTransaction(array($this, 'updateIDR'));
		}

		// Check victory status on CTF games
		if ($this->game->params['victory'] == 2) {
			$this->db->safeTransaction(array($this, 'updateCTF'));
		}

		// Update rankings
		$this->db->safeTransaction(array($this->main, 'call'), 'updateRankings');
	}


	public function handleSystemGroup($startAt) {
		$q = $this->db->query("SELECT id,nebula FROM system ORDER BY id ASC LIMIT 5 OFFSET $startAt");
		$n = dbCount($q);
		while ($r = dbFetchArray($q)) {
			$this->handleSystem($r[0], $r[1]);
		}
		return $n;
	}

	public function unstickFleets() {
		l::debug("Unsticking fleets");
		$this->db->query("UPDATE fleet SET can_move = 'Y' WHERE can_move = 'H'");
	}

	public function hyperspaceStandby() {
		$q = $this->db->query("SELECT id,owner,waiting FROM fleet WHERE waiting IS NOT NULL");
		$fWait = array();
		while ($r = dbFetchArray($q)) {
			$fWait[$r[2]] = array($r[0],$r[1]);
		}
		if (!count($fWait)) {
			return;
		}

		l::debug("Handling hyperspace stand-by");
		$q = $this->db->query(
			"SELECT * FROM hs_wait WHERE id IN (" . join(',', array_keys($fWait)) . ")"
		);
		while ($r = dbFetchHash($q)) {
			$r['fleet'] = $fWait[$r['id']][0];
			$r['player'] = $fWait[$r['id']][1];
			$fWait[$r['id']] = $r;
		}

		$losses = array();
		$planets = array();
		foreach	($fWait as $fw) {
			if ($this->handleWait($fw, &$losses)) {
				array_push($planets, $fw['drop_point']);
			}
		}
		foreach ($planets as $planetID) {
			$this->planets->call('detectFleets', $planetID);
		}

		$tm = time();
		foreach ($losses as $pid => $locs) {
			foreach ($locs as $plid => $fl) {
				$pinf = $this->planets->call('byId', $plid);

				$hsLosses = array();
				foreach ($fl as $f) {
					array_push($hsLosses, array(
						"p_id"		=> $plid,
						"p_name"	=> $pinf['name'],
						"f_name"	=> $f[0],
						"gaships"	=> $f[1],
						"fighters"	=> $f[2],
						"cruisers"	=> $f[3],
						"bcruisers"	=> $f[4],
						"power"		=> $f[5]
					));
				}
				$this->msg->call('send', $pid, 'hsloss', $hsLosses);
			}
		}

		$this->fleets->call('sendMoveMessages');
	}


	public function fleetSales() {
		$q = $this->db->query("SELECT id FROM fleet WHERE sale IS NOT NULL");
		if (! dbCount($q)) {
			return;
		}

		l::debug("Handling fleet sales");
		while ($r = dbFetchArray($q)) {
			$f = $this->fleets->call('get', $r[0]);
			l::trace("FSALE: found fleet {$r[0]}");
			if (!is_null($f['sale_info']['sale']['planet'])) {
				l::trace("FSALE({$r[0]}): fleet is bundled with a planet, skipping");
				continue;
			}

			if ($f['sale'] > 0) {
				l::debug("FSALE({$f['id']}): fleet isn't ready yet ({$f['sale']}h remaining)");
				$this->db->query("UPDATE fleet SET sale=sale-1 WHERE id={$f['id']}");
				continue;
			}

			$q2 = $this->db->query(
				"SELECT id,sold_to FROM sale WHERE player={$f['owner']} AND fleet={$f['id']}"
				. " AND finalized IS NOT NULL ORDER BY finalized DESC LIMIT 1"
			);
			list($oid,$to) = dbFetchArray($q2);
			l::trace("FSALE({$f['id']}): offer #$oid, buyer #$to");

			// Read data required to send the messages
			$fps = $this->fleets->call('getPower', $f['owner'], $f['gaships'], $f['fighters'],
				$f['cruisers'], $f['bcruisers']);
			$fpb = $this->fleets->call('getPower', $to, $f['gaships'], $f['fighters'], $f['cruisers'],
				$f['bcruisers']);
			l::trace("FSALE({$f['id']}): fps= $fps, fpb= $fpb");

			// Send messages
			$this->msg->call('send', $f['owner'], 'sale', array(
				'f_name'	=> $f['name'],
				'f_power'	=> $fps,
				'pl_id'		=> $to,
				'pl_name'	=> $this->players->call('getName', $to),
				'is_sale'	=> 'TRUE'
			));
			$this->msg->call('send', $to, 'sale', array(
				'f_name'	=> $f['name'],
				'f_power'	=> $fpb,
				'pl_id'		=> $f['owner'],
				'pl_name'	=> $this->players->call('getName', $f['owner']),
				'is_sale'	=> 'FALSE'
			));
			l::trace("FSALE({$f['id']}): messages sent");

			// Transfer control
			$this->db->query("UPDATE fleet SET sale=NULL,owner=$to WHERE id={$f['id']}");
			$this->fleets->call('invCache', $f['id']);
			// FIXME: add history
			$this->db->query("DELETE FROM sale WHERE id=$oid");
			l::debug("FSALE({$f['id']}): control transfered");
		}
	}


	public function updateIDR() {
		l::debug("Updating IDR");
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

	public function updateCTF() {
		if (! $this->game->getLib()->call('isFinished')) {
			$this->game->getLib('beta5/ctf')->call('checkTargets');
		}
	}


	public function updateHappiness() {
		l::debug("Updating planetary happiness");

		$q = $this->db->query("SELECT id,owner FROM planet WHERE status=0");
		$destroy = array();
		while ($r = dbFetchArray($q)) {
			$nHap = $this->planets->call('updateHappiness', $r[0]);

			// Handle revolts
			if ($nHap < 20 && !in_array($r[0], $this->oChange)) {
				if (!is_null($r[1]) && is_null($destroy[$r[1]])) {
					$destroy[$r[1]] = array();
				}
				$pDestr = $this->planetRevolt($r[0], $nHap);
				if (!is_null($r[1])) {
					array_push($destroy[$r[1]], $pDestr);
				}
				$this->planets->call('updateHappiness', $r[0]);
			}
		}

		// Send damage reports for revolts
		$tm = time() + 1;
		foreach	($destroy as $player => $plist) {
			$report = array();
			foreach	($plist as $data) {
				array_push($report, array(
					"planet"	=> $data[0],
					"pname"		=> $data[1],
					"ifact"		=> $data[2],
					"mfact"		=> $data[3],
					"turrets"	=> $data[4]
				));
			}
			$this->msg->call('send', $player, 'revdmg', $report);
		}

	}


	private function buildFleets($plId, $ownId, $g, $f, $c, $b) {
		if ($g + $f + $c + $b == 0) {
			return;
		}

		$q = $this->db->query("SELECT id FROM fleet WHERE can_move='Y' AND LOWER(name)='no name' AND location=$plId AND owner=$ownId AND sale IS NULL LIMIT 1");
		if (dbCount($q)) {
			list($id) = dbFetchArray($q);
			$this->db->query("UPDATE fleet SET gaships=gaships+$g,fighters=fighters+$f,cruisers=cruisers+$c,bcruisers=bcruisers+$b WHERE id=$id");
		} else {
			$qs = "INSERT INTO fleet(owner,location,gaships,fighters,cruisers,bcruisers)VALUES($ownId,$plId,$g,$f,$c,$b)";
			$this->db->query($qs);
		}
	}


	private function planetBuildQueue($planet, $rules) {
		static	$types = array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');

		$units = $rules['mf_productivity'] * $rules['mf_productivity_factor'] * $planet['mfact'];
		$cl = $planet['corruption'] / 32000;
		if ($cl > .1) {
			$units = $units * (1.1 - $cl);
		}

		$vac = $this->players->call('isOnVacation', $planet['owner']);
		if ($vac) {
			$units = $units / 4;
		}

		$prot = $this->players->call('getProtectionLevel', $planet['owner']);
		if ($prot) {
			$units = $units * 1.15;
		}

		$units = round($units);

		$bq = $this->bq->call('get', $planet['id']);
		$prod = array(0, 0, 0, 0, 0);
		$move = 0;
		while (count($bq) && $units > 0) {
			if ($bq[0]['units'] <= $units) {
				$units -= $bq[0]['units'];
				$prod[$bq[0]['type']] += $bq[0]['quantity'];
				$this->db->query("DELETE FROM buildqueue WHERE planet=".$planet['id']." AND bq_order=$move");
				$move ++;
				array_shift($bq);
			} else {
				$units += $bq[0]['workunits'];
				$wuCost = $rules['workunits_'.$types[$bq[0]['type']]];
				$mod = $units % $wuCost;
				$nBuild = ($units - $mod) / $wuCost;
				$prod[$bq[0]['type']] += $nBuild;
				$nQt = $bq[0]['quantity'] - $nBuild;
				$this->db->query("UPDATE buildqueue SET quantity=$nQt,workunits=$mod WHERE planet=".$planet['id']." AND bq_order=$move");
				$units = 0;
			}
		}

		if ($move) {
			$this->bq->call('reorder', $planet['id']);
		}

		if ($prod[0] > 0) {
			$this->db->query("UPDATE planet SET turrets=turrets+".$prod[0]." WHERE id=".$planet['id']);
			$tm = time();
			$this->db->query("DELETE FROM turhist WHERE $tm-moment>86400");
			$this->db->query("INSERT INTO turhist VALUES({$planet['id']},$tm,$prod[0])");
		}

		$this->buildFleets($planet['id'], $planet['owner'], $prod[1], $prod[2], $prod[3], $prod[4]);
		$this->planets->call('updateMilStatus', $planet['id']);
	}

	private function planetSale(&$planet) {
		l::trace("SALE: handling planet {$planet['id']}");
		if ($planet['sale'] > 0) {
			l::debug("SALE({$planet['id']}): not ready for sale yet ({$planet['sale']}h remaining)");
			$this->db->query("UPDATE planet SET sale=sale-1 WHERE id=".$planet['id']);
			$q = $this->db->query(
				"SELECT fleet FROM sale WHERE player=".$planet['owner']." AND planet=".$planet['id']
				. " AND finalized IS NOT NULL ORDER BY finalized DESC LIMIT 1"
			);
			list($fleet) = dbFetchArray($q);
			if (!is_null($fleet)) {
				l::trace("SALE({$planet['id']}): fleet $fleet bundled");
				$this->db->query("UPDATE fleet SET sale=sale-1 WHERE id=$fleet");
			}
		} else {
			l::trace("SALE({$planet['id']}): running sale code");
			$q = $this->db->query(
				"SELECT id,sold_to,fleet FROM sale WHERE player=".$planet['owner']." AND planet=".$planet['id']
				. " AND finalized IS NOT NULL ORDER BY finalized DESC LIMIT 1"
			);
			if (!dbCount($q)) {
				l::warn("***** BUG: planet {$planet['id']} has sale field set to 0, but no "
					. "sales were found");
				$this->db->query("UPDATE planet SET sale=NULL WHERE id={$planet['id']}");
				return;
			}
			list($oid,$to,$fleet) = dbFetchArray($q);
			l::trace("SALE({$planet['id']}): offer #$oid, sold to player #$to (bundled fleet: "
				. (is_null($fleet) ? "none" : "#$fleet") . ")");

			// Read data required to send the messages
			$tn = addslashes($this->players->call('getName', $to));
			$fn = addslashes($this->players->call('getName', $planet['owner']));
			l::trace("SALE({$planet['id']}): original owner '$tn', new owner '$fn'");
			if (is_null($fleet)) {
				$fqs = $fqb = "NULL,NULL";
			} else {
				$q = $this->db->query("SELECT * FROM fleet WHERE id=$fleet");
				$fd = dbFetchHash($q);
				$fps = $this->fleets->call('getPower', $fd['owner'], $fd['gaships'], $fd['fighters'], $fd['cruisers'], $fd['bcruisers']);
				$fpb = $this->fleets->call('getPower', $to, $fd['gaships'], $fd['fighters'], $fd['cruisers'], $fd['bcruisers']);
				$fqs = "'" . addslashes($fd['name']) . "',$fps";
				$fqb = "'" . addslashes($fd['name']) . "',$fpb";
			}
			l::trace("SALE({$planet['id']}): fqs: $fqs");
			l::trace("SALE({$planet['id']}): fqb: $fqb");

			// Send messages
			// FIXME: message API
			$tm = time();
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES({$planet['owner']},$tm,'sale','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player={$planet['owner']} AND sent_on=$tm AND mtype='sale' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_sale VALUES($mid,{$planet['id']},'".addslashes($planet['name'])."',$fqs,$to,'$tn',TRUE)");
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($to,$tm,'sale','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$to AND sent_on=$tm AND mtype='sale' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_sale VALUES($mid,{$planet['id']},'".addslashes($planet['name'])."',$fqb,{$planet['owner']},'$fn',FALSE)");
			l::trace("SALE({$planet['id']}): messages sent");

			// Check for vacation mode
			$vac = $this->players->call('isOnVacation', $to);
			if ($vac) {
				$q = $this->db->query("SELECT COUNT(*) FROM fleet WHERE location={$planet['id']} AND attacking");
				list($c) = dbFetchArray();
				if ($c) {
					$vm = 'PEND';
				} else {
					$vm = 'YES';
				}
			} else {
				$vm = 'NO';
			}
			l::trace("SALE({$planet['id']}): vacation mode for target player: $vm");

			// Sell the planet
			$this->players->call('losePlanet', $planet['owner'], $planet['id']);
			$this->players->call('takePlanet', $to, $planet['id']);
			$nVal = $this->planets->call('ownerChange', $planet['id'], $to, $vm);
			if ($nVal > 0) {
				$planet['max_pop'] = $nVal;
			}
			$this->db->query("UPDATE player SET bh_unhappiness=bh_unhappiness+1 WHERE id={$planet['owner']} AND bh_unhappiness<15");
			if (!is_null($fleet)) {
				$this->db->query("UPDATE fleet SET sale=NULL,owner=$to WHERE id=$fleet");
			}
			$planet['owner'] = $to;
			// FIXME: add history
			$this->db->query("DELETE FROM sale WHERE id=$oid");

			l::debug("SALE({$planet['id']}) completed");
		}
	}

	private function whsnPreparation(&$planet) {
		if ($planet['bh_prep'] > 0) {
			// WHSN is not ready yet
			$this->db->query("UPDATE planet SET bh_prep=bh_prep-1 WHERE id=".$planet['id']);
			$this->db->query("UPDATE player SET bh_unhappiness=bh_unhappiness+1 WHERE id={$planet['owner']} AND bh_unhappiness<15");
			return false;
		}

		// Get fleet statistics to send messages
		$players = array(
			$planet['owner'] => array(
					"was_owner"	=> dbBool(true),
					"attacking"	=> false,
					"power"		=> 0,
					"fleets"	=> array(),
				)
		);
		$q = $this->db->query("SELECT * FROM fleet WHERE location=".$planet['id']);
		$aPower = $dPower = 0;
		while ($r = dbFetchHash($q)) {
			if (is_null($players[$r['owner']])) {
				$players[$r['owner']] = array(
					"was_owner"	=> dbBool(false),
					"attacking"	=> ($r['attacking'] == 't'),
					"power"		=> 0,
					"fleets"	=> array(),
				);
			}
			$f = array(
				"name"		=> $r['name'],
				"gaships"	=> $r['gaships'],
				"fighters"	=> $r['fighters'],
				"cruisers"	=> $r['cruisers'],
				"bcruisers"	=> $r['bcruisers'],
			);
			$power = $this->fleets->call('getPower', $r['owner'], $r['gaships'], $r['fighters'], $r['cruisers'], $r['bcruisers']);
			$players[$r['owner']]["power"] += ($f['power'] = $power);
			array_push($players[$r['owner']]['fleets'], $f);
			if ($players[$r['owner']]['attacking']) {
				$aPower += $power;
			} else {
				$dPower += $power;
			}

			// Cancel fleet sales
			$qfs = $this->db->query("SELECT id,player,finalized FROM sale WHERE fleet={$r['id']}");
			if (dbCount($qfs)) {
				list($saleId, $seller, $saleFin) = dbFetchArray($qfs);
				if (is_null($saleFin)) {
					$ga = "cancel";
				} else {
					$ga = "cancelTransfer";
				}
				$this->sales->call($ga, $seller, $saleId);
			}
		}

		// Send the messages
		// FIXME: message API
		$tm = time();
		foreach ($players as $pId => $data) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($pId,$tm,'whsn','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$pId AND sent_on=$tm AND mtype='whsn' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);

			if ($data['attacking']) {
				$pow = $aPower - $data['power'];
				$fPower = $pow; $ePower = $dPower;
			} else {
				$pow = $dPower - $data['power'];
				$fPower = $pow; $ePower = $aPower;
			}

			$this->db->query("INSERT INTO msg_whsn VALUES($mid,{$planet['id']},'"
				. addslashes($planet['name']) . "'," . $data['was_owner']
				. ",$fPower,$ePower)");
			foreach ($data['fleets'] as $f)
				$this->db->query("INSERT INTO whsn_fleet VALUES($mid,'" . addslashes($f['name'])
					. "',{$f['gaships']},{$f['fighters']},{$f['cruisers']},"
					. "{$f['bcruisers']},{$f['power']})");
		}

		// Check for IDR points
		$pow = $aPower - $dPower;
		if ($pow > 0) {
			$pn = $this->players->call('getName', $planet['owner']);
			if (is_null($this->idrInc[$pn])) {
				$this->idrInc[$pn] = $pow;
			} else {
				$this->idrInc[$pn] += $pow;
			}
		}

		// Destroy the planet and anything around it
		$this->players->call('losePlanet', $planet['owner'], $planet['id']);
		$this->bq->call('flush', $planet['id']);
		$this->db->query("DELETE FROM fleet WHERE location=".$planet['id']);
		do {
			$rn = strtoupper(substr(md5(uniqid(rand())), 0, 10));
			$q = $this->db->query("SELECT id FROM planet WHERE name='PR-[$rn]'");
		} while(dbCount($q));
		$this->db->query(
			"UPDATE planet "
				. "SET pop = 0, max_pop = 0, happiness = 0, mfact = 0, ifact = 0, beacon = 0, "
					. "built_probe = FALSE, probe_policy = NULL, bh_prep = NULL, owner = NULL, "
					. "sale = NULL, status = 1, bh_unhappiness = 0, vacation = 'NO', "
					. "name = 'PR-[$rn]', abandon = NULL "
				. "WHERE id = {$planet['id']}"
		);
		$this->db->query(
			"UPDATE planet "
				. "SET bh_unhappiness = bh_unhappiness + 4 "
				. "WHERE system={$planet['system']} AND id <> {$planet['id']} "
				  . "AND status = 0 AND bh_unhappiness < 12"
		);
		$this->db->query("DELETE FROM planet_abandon_time WHERE id = {$planet['id']}");

		// Raise the empire-wide unhappiness for the planet's owner
		$q = $this->db->query("SELECT bh_unhappiness FROM player WHERE id={$planet['owner']}");
		list($bhu) = dbFetchArray($q);
		$nuh = max($bhu + 4, 15);
		$this->db->query("UPDATE player SET bh_unhappiness=$nuh WHERE id={$planet['owner']}");

		return true;
	}


	private function planetTaken(&$planet) {
		if ($planet['turrets'] > 0) {
			return false;
		}

		// Check if the planet was in vacation mode before the attacking ships arrived
		// If it was, it can't be taken
		if ($planet['vacation'] == 'YES ') {
			return false;
		}

		// List fleets at location
		$q = $this->db->query("SELECT owner,gaships,attacking,time_spent FROM fleet WHERE location={$planet['id']}");
		$players = array();
		while ($r = dbFetchArray($q)) {
			// If there are defending fleets, forget it
			if ($r[2] == 'f') {
				if ($r[3] > 5) {
					return false;
				}
				continue;
			} elseif ($r[3] < 15) {
				continue;
			}

			// Count GA Ships per player
			$players[$r[0]] += $r[1];
		}

		// Get the highest GA efficiency
		$maxGAPower = $maxGAPlayer = -1;
		foreach	($players as $id => $gas) {
			$rules = $this->rules->call('get', $id);
			$gaPower = $gas * $rules['gaship_pop'];
			if ($gaPower > $maxGAPower) {
				$maxGAPower = $gaPower;
				$maxGAPlayer = $id;
			}
		}

		// Check whether the player can actually take the planet
		if ($maxGAPlayer == -1 || $maxGAPower < $planet['pop']) {
			return false;
		}

		// A player has taken control on the planet
		if (!is_null($planet['owner'])) {
			// If the planet had an owner, flush build queue and cancel sales
			$this->bq->call('flush', $planet['id']);
			$q = $this->db->query("SELECT id,finalized,sold_to FROM sale WHERE planet={$planet['id']}");
			if (dbCount($q)) {
				// Cancel sale
				list($sId,$fin,$to) = dbFetchArray($q);
				if (is_null($fin)) {
					$ga = 'cancel';
				} else {
					$ga = 'cancelTransfer';

					// Send message
					// FIXME ...
					$tm = time();
					$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($to,$tm,'plsc','INT',TRUE)");
					$q = $this->db->query("SELECT id FROM message WHERE player=$to AND sent_on=$tm AND mtype='plsc' ORDER BY id DESC LIMIT 1");
					list($mid) = dbFetchArray($q);
					$this->db->query("INSERT INTO msg_plsc VALUES($mid,{$planet['owner']},$maxGAPlayer,{$planet['id']},'"
						. addslashes($planet['name']) . "')");
				}
				$this->sales->call($ga, $planet['owner'], $sId);
			}

			$rqs = ",renamed=".time();
		} else {
			$rqs = "";
		}

		// Check for vacation mode
		$vm = $this->players->call('isOnVacation', $maxGAPlayer) ? 'YES' : 'NO';

		// Transfer control
		$this->players->call('losePlanet', $planet['owner'], $planet['id']);
		$this->players->call('takePlanet', $maxGAPlayer, $planet['id']);
		$nVal = $this->planets->call('ownerChange', $planet['id'], $maxGAPlayer, $vm);
		if ($nVal > 0) {
			$planet['max_pop'] = $nVal;
		}

		// Update a potential abandon log entry
		$tm = time();
		$this->db->query("UPDATE abandon_log SET retake_owner=$maxGAPlayer,retake_time=$tm WHERE planet={$planet['id']} AND retake_time IS NULL");

		// Get the list of the new owner's enemies
		$q = $this->db->query("SELECT enemy FROM enemy_player WHERE player=$maxGAPlayer");
		$enemies = array();
		while ($r = dbFetchArray($q)) {
			array_push($enemies, $r[0]);
		}
		$q = $this->db->query("SELECT p.id FROM player p,enemy_alliance e WHERE p.alliance=e.alliance AND p.a_status='IN' AND e.player=$maxGAPlayer");
		while ($r = dbFetchArray($q)) {
			array_push($enemies, $r[0]);
		}
		$enemies = array_unique($enemies);

		// Switch attacking fleets to defense unless they belong to an enemy
		$qs = "UPDATE fleet SET attacking=FALSE,can_move='Y' WHERE location={$planet['id']}";
		if (count($enemies)) {
			$qs .= " AND owner NOT IN (".join(',',$enemies).")";
		}
		$this->db->query($qs);

		// Send messages to the former owner and the new owner
		$pn = addslashes($planet['name']);
		$noMsg = array($maxGAPlayer);
		$mData = array(
			'p_id'		=> $planet['id'],
			'p_name'	=> $planet['name'],
			'owner'		=> $planet['owner'],
			'status'	=> 'TAKE'
		);
		$this->msg->call('send', $maxGAPlayer, 'ownerch', $mData);

		$mData['owner'] = $maxGAPlayer;
		if (!is_null($planet['owner'])) {
			$mData['status'] = 'LOSE';
			array_push($noMsg, $pid = $planet['owner']);
			$this->msg->call('send', $pid, 'ownerch', $mData);
		}

		// Get the list of other players and send them a message as well
		$q2 = $this->db->query("SELECT DISTINCT owner FROM fleet WHERE location={$planet['id']} AND owner NOT IN (" . join(',', $noMsg) . ")");
		$mData['status'] = 'VIEW';
		while ($r = dbFetchArray($q2)) {
			$this->msg->call('send', $r[0], 'ownerch', $mData);
		}

		return true;
	}


	private function handlePlanet($planet) {
		// WHSN preparation
		if (!is_null($planet['bh_prep']) && $this->whsnPreparation($planet)) {
			return false;
		}

		// Check for owner change
		if ($this->planetTaken($planet)) {
			$this->planets->call('updateMilStatus', $planet['id']);
			return true;
		}

		// Planet sale
		if (!is_null($planet['sale'])) {
			$this->planetSale($planet);
		}

		// No build queues or revolts on neutral planets
		if (is_null($planet['owner'])) {
			return false;
		}

		// Load owner rules
		$rules = $this->rules->call('get', $planet['owner']);

		// Build queue
		$this->planetBuildQueue($planet, $rules);

		return false;
	}


	private function handleSystem($sid, $neb) {
		if ($neb) {
			return;
		}

		l::trace("Handling system #$sid");
		$q = $this->db->query("SELECT * FROM planet WHERE system=$sid");
		while ($planet = dbFetchHash($q)) {
			if ($neb || $planet['status'] == 1) {
				continue;
			}
			if ($this->handlePlanet($planet)) {
				array_push($this->oChange, $planet['id']);
			}
		}
	}


	private function handleWait($fw, &$losses) {
		$canDestroy = $this->standby->call('canDestroy', $fw['drop_point'], $fw['player']);

		// If we can destroy fleets, find out whether we must do so
		if ($canDestroy) {
			$prob = $this->standby->call('getLossProb', $fw['time_spent']);
			l::debug("HSSB({$fw['fleet']}): at planet #{$fw['drop_point']}, loss probability = "
				. "$prob (time spent: {$fw['time_spent']}h)");
			if (rand(1,100) <= $prob) {
				l::debug("HSSB({$fw['fleet']}): applying fleet losses");

				// Destroy a part of the fleet
				$amount = rand(5,round(10+$prob/10));
				$fleet = $this->fleets->call('get', $fw['fleet']);
				$rules = $this->rules->call('get', $fw['player']);
				$fcLoss = ceil(($fleet['cruisers'] / 100) * $amount);
				$fbLoss = ceil(($fleet['bcruisers'] / 100) * $amount);
				l::trace("HSSB({$fw['fleet']}): cruisers lost: $fcLoss; battle cruisers lost: $fbLoss");

				// Compute the amount of transported ships to be destroyed
				$gaSpace = $fleet['gaships'] * $rules['gaship_space'];
				$fSpace = $fleet['fighters'] * $rules['fighter_space'];
				$tSpace = $gaSpace + $fSpace;
				if ($tSpace > 0) {
					$haul = $fleet['cruisers'] * $rules['cruiser_haul']
						+ $fleet['bcruisers'] * $rules['bcruiser_haul'];
					$haulUsed = $tSpace / $haul;
					$lHaul = $fcLoss * $rules['cruiser_haul'] + $fbLoss * $rules['bcruiser_haul'];
					$lSpace = $haulUsed * $lHaul;
					$gaLSpace = ($lSpace / $tSpace) * $gaSpace;
					$fLSpace = ($lSpace / $tSpace) * $fSpace;
					$fgLoss = min($fleet['gaships'], ceil($gaLSpace / $rules['gaship_space']));
					$ffLoss = min($fleet['fighters'], ceil($fLSpace / $rules['fighter_space']));
				} else {
					$fgLoss = $ffLoss = 0;
				}
				l::trace("HSSB({$fw['fleet']}): fighters lost: $ffLoss; GA ships lost: $fgLoss");

				// Add details to the $losses array
				if (!is_array($losses[$fleet['owner']])) {
					$losses[$fleet['owner']] = array();
				}
				if (!is_array($losses[$fleet['owner']][$fw['drop_point']])) {
					$losses[$fleet['owner']][$fw['drop_point']] = array();
				}
				array_push($losses[$fleet['owner']][$fw['drop_point']], array(
					addslashes($fleet['name']), $fgLoss, $ffLoss, $fcLoss, $fbLoss,
					$this->fleets->call('getPower', $fleet['owner'], $fgLoss, $ffLoss,
						$fcLoss, $fbLoss)
				));

				if ($fcLoss == $fleet['cruisers'] && $fbLoss == $fleet['bcruisers']
						&& $ffLoss == $fleet['fighters'] && $fgLoss == $fleet['gaships']) {
					$this->fleets->call('disband',$fw['fleet'],true);
					l::trace("HSSB({$fw['fleet']}): fleet disbanded");
				} elseif ($fcLoss + $fbLoss + $ffLoss + $fgLoss > 0) {
					$this->db->query(
						"UPDATE fleet SET gaships = gaships - $fgLoss, "
							       . "fighters = fighters - $ffLoss, "
							       . "cruisers = cruisers - $fcLoss, "
							       . "bcruisers = bcruisers - $fbLoss "
						. "WHERE id = {$fw['fleet']}"
					);
				}
			}
		}

		if ($fw['time_left'] == 1) {
			// Fleet arrival
			$this->fleets->call('arrival', $fw['fleet'], $fw['drop_point'], $fw['origin']);
			// Destroy wait entry
			$this->db->query("DELETE FROM hs_wait WHERE id={$fw['id']}");
		} else {
			// Update wait entry
			$this->db->query(
				"UPDATE hs_wait SET time_left = time_left - 1, time_spent = time_spent + 1 "
					. "WHERE id={$fw['id']}"
			);
		}

		return !($fw['time_left'] == 1);
	}


	private function planetRevolt($id, $hap) {
		$q = $this->db->query("SELECT name,ifact,mfact,turrets FROM planet WHERE id=$id");
		list($n, $if, $mf, $tr) = dbFetchArray($q);

		$pdestr = 30 - $hap;
		$pdestr += rand(-floor($pdestr/2),floor($pdestr/2));
		$pdestr /= 150;
		$dInd = round($if * $pdestr);
		$dMil = round($mf * $pdestr);
		$dTur = round($tr * $pdestr);

		if ($dInd + $dMil + $dTur != 0) {
			$this->db->query(
				"UPDATE planet SET ifact = ifact - $dInd, mfact = mfact - $dMil, "
					        . "turrets = turrets - $dTur "
					. "WHERE id = $id"
			);
		}

		return array($id, $n, $dInd, $dMil, $dTur);
	}


	public function handleAbandon() {
		l::debug("Handling planets being abandonned");

		// Decrease time to abandon
		$this->db->query("UPDATE planet SET abandon = abandon - 1 WHERE abandon IS NOT NULL");

		// Get the list of planets to abandon at this tick
		$q = $this->db->query("SELECT id, owner FROM planet WHERE abandon = 0");
		$oList = array();
		while ($r = dbFetchArray($q)) {
			if (is_null($r[1])) {
				l::warn("****** BUG: planet {$r[0]} is being abandoned and has no owner");
				continue;
			}
			$this->abandonPlanet($r[0], $r[1]);
			if (!is_array($oList[$r[1]])) {
				$oList[$r[1]] = array();
			}
			array_push($oList[$r[1]], $r[0]);
		}

		foreach	($oList as $pid => $aPl) {
			// Send the message
			$message = array();
			foreach ($aPl as $plId) {
				$planet = $this->planets->call('byId', $plId);
				array_push($message, array("p_id" => $plId, "p_name" => $planet['name']));
			}
			$this->msg->call('send', $pid, 'abandon', $message);

			// Update other planets' happiness
			$this->players->call('updateHappiness', $pid);
		}
	}


	private function abandonPlanet($pid, $oid) {
		// Insert log entry
		$this->db->query(
			"INSERT INTO abandon_log(planet,abandon_time,former_owner) "
				. "VALUES ($pid," . time() . ",$oid)"
		);

		// Make the planet neutral
		$this->planets->call('ownerChange', $pid);
		$this->players->call('losePlanet', $oid, $pid);
	}
}

?>

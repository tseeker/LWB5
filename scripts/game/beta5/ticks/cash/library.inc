<?php

//------------------------------------------------------
// "Cash tick": Income computations & fleet destruction
//------------------------------------------------------

class beta5_ticks_cash_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->fleets	= $this->game->getLib('beta5/fleet');
		$this->msgs	= $this->game->getLib('beta5/msg');
		$this->planets	= $this->game->getLib('beta5/planet');
		$this->players	= $this->game->getLib('beta5/player');
		$this->rules	= $this->game->getLib('beta5/rules');
	}


	public function runTick() {
		// Update time required to abandon planets
		$this->db->safeTransaction(array($this, "handleAbandon"));

		// Handle the players' income
		$players = $this->db->safeTransaction(array($this, "getPlayers"));
		foreach ($players as $player) {
			$this->db->safeTransaction(array($this, "playerIncome"), $player);
		}

		// Handle CTF victory
		$this->db->safeTransaction(array($this, "checkCTF"));
	}


	public function checkCTF() {
		// Check victory status on CTF games
		if ($this->game->params['victory'] == 2 && ! $this->game->getLib()->call('isFinished')) {
			$this->game->getLib('beta5/ctf')->call('checkTargets');
		}
	}


	public function getPlayers() {
		$q = $this->db->query(
			"SELECT id FROM player "
				. "WHERE quit IS NULL "
				   . "OR UNIX_TIMESTAMP(NOW()) - quit < 86400"
		);

		$players = array();
		while ($r = dbFetchArray($q)) {
			array_push($players, $r[0]);
		}

		return $players;
	}


	public function playerIncome($id) {
		$q = $this->db->query(
			"SELECT p.cash, (a.status = 'VAC') FROM player p,account a "
				. "WHERE p.id = $id AND a.id = p.userid "
				. "FOR UPDATE OF p"
		);
		list($cash, $vac) = dbFetchArray($q);

		$protected = $this->players->call('getProtectionLevel', $id);

		$div = ($vac == 't' ? 4 : 1);
		$mult = ($protected ? 1.15 : 1);
		$income = round($this->computeIncome($id) * $mult / $div);
		$upkeep = round($this->computeUpkeep($id) / $div);
		$profit = ceil($income - $upkeep) / 2;

		$cash += $profit;
		if ($cash < 0) {
			$this->debt($id, $income, $upkeep, $cash);
			$cash = 0;
		}
		$this->db->query("UPDATE player SET cash=$cash WHERE id=$id");
	}


	private function computeIncome($player) {
		$income = 0;
		$ppl = $this->players->call('getPlanets', $player);

		foreach ($ppl as $id => $name) {
			$info = $this->planets->call('byId', $id);
			$m = $this->planets->call('getIncome', $player, $info['pop'], $info['happiness'],
				$info['ifact'], $info['mfact'], $info['turrets'], $info['corruption']);
			$income += $m[0];
		}

		return $income;
	}


	private function computeUpkeep($player) {
		$pfl = $this->players->call('getFleets', $player);
		$upkeep = 0;

		foreach ($pfl as $id => $name) {
			$info = $this->fleets->call('get', $id);
			$u = $this->fleets->call('getUpkeep', $player, $info['gaships'], $info['fighters'],
				$info['cruisers'], $info['bcruisers']);
			$upkeep += $u;
		}

		return	$upkeep;
	}


	private function killFleets($id, $ratio) {
		$fl = array_keys($this->players->call('getFleets', $id));
		$rules = $this->rules->call('get', $id);

		$kills = array(0, 0, 0, 0);
		foreach	($fl as $fid) {
			$f = $this->fleets->call('get', $fid);

			if (is_null($f['waiting']) && (is_null($f['moving'])
					|| !is_null($f['moving']) && $f['move']['hyperspace'] == 'f')) {

				// Fleet is idle or not in HyperSpace
				$kg = ceil($ratio * $f['gaships']);
				$kf = ceil($ratio * $f['fighters']);
				$kc = ceil($ratio * $f['cruisers']);
				$kb = ceil($ratio * $f['bcruisers']);
			} else {
				// Fleet is in Hyperspace
				$kc = ceil($f['cruisers'] * $ratio);
				$kb = ceil($f['bcruisers'] * $ratio);

				// Compute the amount of transported ships to be destroyed
				// FIXME: computeHyperspaceDamage() ?
				$gaSpace = $f['gaships'] * $rules['gaship_space'];
				$fSpace = $f['fighters'] * $rules['fighter_space'];
				$tSpace = $gaSpace + $fSpace;
				if ($tSpace > 0) {
					$haul = $f['cruisers'] * $rules['cruiser_haul']
						+ $f['bcruisers'] * $rules['bcruiser_haul'];
					$haulUsed = $tSpace / $haul;
					$lHaul = $kc * $rules['cruiser_haul'] + $kb * $rules['bcruiser_haul'];
					$lSpace = $haulUsed * $lHaul;
					$gaLSpace = ($lSpace / $tSpace) * $gaSpace;
					$fLSpace = ($lSpace / $tSpace) * $fSpace;
					$kg = min($f['gaships'], ceil($gaLSpace / $rules['gaship_space']));
					$kf = min($f['fighters'], ceil($fLSpace / $rules['fighter_space']));
				} else {
					$kg = $kf = 0;
				}
			}

			if ($kc == $f['cruisers'] && $kb == $f['bcruisers'] && $kf == $f['fighters']
					&& $kg == $f['gaships']) {

				$this->fleets->call('disband', $fid, true);
			} elseif ($kc + $kb + $kf + $kg > 0) {
				$this->db->query(
					"UPDATE fleet SET gaships=gaships - $kg, fighters = fighters - $kf, "
							. "cruisers=cruisers - $kc,bcruisers = bcruisers - $kb "
						. "WHERE id=$fid"
				);
				$this->fleets->call('invCache', $fid);
			}
			$kills[0] += $kg; $kills[1] += $kf; $kills[2] += $kc; $kills[3] += $kb;
		}

		// No kills?
		if ($kills[0] + $kills[1] + $kills[2] + $kills[3] == 0) {
			return;
		}

		// Send the message
		$this->msgs->call('send', $id, 'kfleet', array(
			"gaships"	=> $kills[0],
			"fighters"	=> $kills[1],
			"cruisers"	=> $kills[2],
			"bcruisers"	=> $kills[3],
		));

		$this->players->call('updateHappiness', $id);
	}


	private function debt($id, $income, $upkeep, $cash) {
		if ($upkeep > 0) {
			$ratio = min(-$cash * 2 / $upkeep, 1) / 5;
			$this->killFleets($id, $ratio);
			return;
		}

		$rules = $this->rules->call('get', $id);
		$ppl = $this->players->call('getPlanets', $id);
		$pinf = array();
		foreach ($ppl as $pid => $name) {
			$info = $this->planets->call('byId', $pid);
			$ic = $this->planets->call('getIncome', $id, $info['pop'], $info['ifact'],
				$info['mfact'], $info['turrets'], $info['corruption']);
			$info['income'] = $ic[0];
			if ($info['income'] >= 0) {
				$income -= $info['income'];
				continue;
			} else {
				$income -= $ic[1] + $ic[2];
				$info['income'] -= $ic[1] + $ic[2];
			}
			$pinf[$pid] = $info;
		}

		$kill = array(0, 0);
		foreach	($pinf as $pid => $p) {
			$ratio = $p['income'] / $income;
			$tCost = $p['turrets'] * $rules['turret_cost'];
			$mCost = $p['mfact'] * $rules['factory_cost'];
			$tRatio = (rand(10,20)/100) * $ratio * $tCost / (-$p['income']);
			$mRatio = (rand(10,20)/100) * $ratio * $mCost / (-$p['income']);

			$tLoss = ceil($p['turrets'] * $tRatio);
			$mLoss = ceil($p['mfact'] * $mRatio);
			if ($tLoss + $mLoss == 0) {
				continue;
			}

			$this->db->query("UPDATE planet SET turrets=turrets-$tLoss,mfact=mfact-$mLoss WHERE id=$pid");
			$kill[0] += $tLoss; $kill[1] += $mLoss;
			$this->planets->call('updateHappiness', $pid);
		}

		if ($kill[0] + $kill[1] == 0) {
			return;
		}

		$this->msgs->call('send', $id, 'kimpr', array("turrets" => $kill[0], "factories" => $kill[1]));
	}


	public function handleAbandon() {
		$this->db->query(
			"UPDATE planet_abandon_time "
				. "SET time_required = time_required + 1 "
				. "WHERE time_required < 24"
		);
	}
}


?>

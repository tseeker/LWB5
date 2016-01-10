<?php

//------------------------------------------------------
// "Day tick": Research, population increase, rankings,
//   protection levels
//------------------------------------------------------

class beta5_ticks_day_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->main	= $this->game->getLib();
		$this->planets	= $this->game->getLib('beta5/planet');
		$this->players	= $this->game->getLib('beta5/player');
		$this->msgs	= $this->game->getLib('beta5/msgs');
		$this->fleets	= $this->game->getLib('beta5/fleet');
		$this->rankings	= $this->game->getLib('main/rankings');
		$this->rules	= $this->game->getLib('beta5/rules');
		$this->tech	= $this->game->getLib('beta5/tech');
	}


	public function runTick() {
		// Cache the whole research tree, we'll need that later
		$this->tech->call('getTree');

		// Increase neutral population
		l::debug("Increasing neutral population");
		$this->db->safeTransaction(array($this, 'increasePopulation'), array(null, false));

		// Increase population and research
		$players = $this->db->safeTransaction(array($this, 'getPlayers'));
		$this->playerPopulation($players);
		$this->playerResearch($players);

		// Mass-update status and timers
		$this->db->safeTransaction(array($this, 'statusUpdates'));

		// Dry run of the rankings computation
		list($players, $playersById) = $this->db->safeTransaction(
			array($this->main, 'call'), array('updateRankings', true)
		);

		// Restore neutral planets and decrease protection levels
		if ((int) $this->game->params['victory'] == 0) {
			$this->db->safeTransaction(array($this, 'restoreNeutrals'));
			$this->db->safeTransaction(array($this, 'decreaseProtection'));
		}

		// Update the data warehouse
		$this->db->safeTransaction(array($this, 'updateWarehouse'), array($players, $playersById));
	}


	public function restoreNeutrals() {
		$q = $this->db->query("SELECT p.id FROM planet p, system s "
			. "WHERE p.owner IS NULL AND p.status = 0 AND s.id = p.system "
			  . "AND s.nebula = 0"
		);

		while ($r = dbFetchArray($q)) {
			$this->planets->call('restoreNeutral', $r[0]);
		}
	}


	public function decreaseProtection() {
		$q = $this->db->query(
			"SELECT s.id, s.prot, y.id FROM system s, player y "
				. "WHERE s.prot > 0 AND s.nebula = 0 "
				  . "AND y.id = (SELECT DISTINCT p.owner FROM planet p "
				  			. "WHERE p.system = s.id AND owner IS NOT NULL) "
				. "FOR UPDATE OF s, y");
		$removeProtection = array();
		while ($r = dbFetchArray($q)) {
			if ($r[1] == 1) {
				$removeProtection[$r[0]] = $r[2];
			}
		}

		$this->db->query("UPDATE system SET prot = prot - 1 WHERE prot > 1 AND nebula = 0");
		foreach ($removeProtection as $system => $player) {
			$this->players->call('breakProtection', $player, 'EXP');
		}
	}


	public function getPlayers() {
		$q = $this->db->query(
			"SELECT p.id, (a.status <> 'STD') FROM player p, account a "
				. "WHERE (p.quit IS NULL OR UNIX_TIMESTAMP(NOW()) - p.quit < 86400) "
			 	  . "AND a.id = p.userid"
		);

		$players = array();
		while ($r = dbFetchArray($q)) {
			$players[$r[0]] = ($r[1] == 't');
		}

		return $players;
	}


	private function playerResearch($players) {
		l::debug("Increasing player research");
		foreach ($players as $playerID => $vacation) {
			$this->db->safeTransaction(array($this, 'research'), array($playerID, $vacation));
		}
	}


	private function playerPopulation($players) {
		l::debug("Increasing player population");
		foreach ($players as $playerID => $vacation) {
			l::trace("Increasing population for player #$playerID");
			$this->db->safeTransaction(array($this, 'increasePopulation'), array($playerID, $vacation));
		}
	}


	public function statusUpdates() {
		// Laws: reduce delays to enact/revoke laws
		$this->db->query("UPDATE research_player SET in_effect=in_effect-1 WHERE in_effect>1");
		$this->db->query("UPDATE research_player SET in_effect=in_effect+1 WHERE in_effect<0");

		// Remove research assistance
		$this->db->query("UPDATE player SET res_assistance=NULL WHERE res_assistance IS NOT NULL");

		// Restrictions on new players
		$this->db->query("UPDATE player SET restrain=restrain-1 WHERE restrain >= 1");

		// Decrease unhappiness because of Wormhole Super Novas and sales
		$this->db->query("UPDATE player SET bh_unhappiness=bh_unhappiness-1 WHERE bh_unhappiness>0");
		$this->db->query("UPDATE planet SET bh_unhappiness=bh_unhappiness-1 WHERE bh_unhappiness>0 AND status=0");

		// Probe building
		$this->db->query("UPDATE planet SET built_probe=FALSE WHERE built_probe");
	}


	// Advance a player's research status.
	public function research($id, $onVacation) {
		$q = $this->db->query("SELECT COUNT(*) FROM research_player WHERE player=$id");
		list($c) = dbFetchArray($q);
		if (!$c) {
			if ($this->lib->game->params['partialtechs'] == 1) {
				$this->tech->call('createTree', $id);
			} else {
				$q = $this->db->query("SELECT id FROM research");
				while ($r = dbFetchArray($q)) {
					$this->db->query("INSERT INTO research_player(player,research,possible) "
						. "VALUES($id,{$r[0]},TRUE)");
				}
			}
		}

		// Get the whole tech tree and the player's research points
		$rp = $this->tech->call('getPoints',$id);
		list($res, $rLeaf) = $this->tech->call('getTree', $id);

		// Reduce by half if we're giving assistance
		$q = $this->db->query("SELECT id FROM player WHERE res_assistance=$id");
		if (dbCount($q)) {
			$rp = round($rp / 2);
		}

		// Are we receiving assistance?
		$q = $this->db->query("SELECT research,res_assistance FROM player WHERE id=$id");
		list($rb, $ra) = dbFetchArray($q);
		if (!is_null($ra)) {
			$rp += round(min($this->tech->call('getPoints', $ra), $rp) / 2);
		}

		// If the player's on vacation, he gets a fourth of the RPs
		if ($onVacation) {
			$rp = round($rp / 4);
		}

		// If the player is under protection, he gets a +25% bonus
		if ($this->players->call('getProtectionLevel', $id)) {
			$rp = round($rp * 1.25);
		}

		// Divide research points between categories
		$rbl = explode('!',$rb);
		$rbp = array();
		for ($i=$s=0;$i<3;$i++) {
			$rbp[$i] = floor($rbl[$i] * $rp / 100);
			$s += $rbp[$i];
		}
		for ($i=0;$s<$rb;$i=($i+1)%3) {
			$rbp[$i] ++; $s ++;
		}
		$possible = array(array(), array(), array());

		// Get current research topics
		$q = $this->db->query("SELECT p.research,p.points FROM research_player p,research r "
			. "WHERE p.research=r.id AND p.points>0 AND p.points<r.points AND p.player=$id");
		$npc = array(0, 0, 0);
		while ($r = dbFetchArray($q)) {
			$rp = array(
				'id'	=> $r[0],
				'pts'	=> $r[1],
				'max'	=> $res[$r[0]]['points']
			);
			$cat = $res[$r[0]]['category'];
			array_push($possible[$cat], $rp);
			$npc[$cat] ++;
		}

		// Get pending research topics
		$q = $this->db->query("SELECT research FROM research_player WHERE player=$id AND points=0 AND possible");
		$pending = array(array(),array(),array());
		while ($r = dbFetchArray($q)) {
			array_push($pending[$res[$r[0]]['category']], $r[0]);
		}

		// Get completed research
		$q = $this->db->query("SELECT p.research FROM research_player p,research r "
			. "WHERE p.research=r.id AND p.points=r.points AND (r.is_law OR p.in_effect <> 0) AND p.player=$id");
		$pRes = array();
		while ($r = dbFetchArray($q)) {
			$pRes[$r[0]] = 1;
		}

		// Find topics which have their dependencies satisfied
		for ($i=0;$i<3;$i++) {
			foreach	($pending[$i] as $p) {
				$ok = true;
				foreach	($res[$p]['depends_on'] as $dep) {
					$ok = $ok && $pRes[$dep];
				}
				if ($ok) {
					array_push($possible[$i], array(
						'id'	=> $p,
						'pts'	=> 0,
						'max'	=> $res[$p]['points']
					));
				}
			}
		}

		// Find points left for each category
		$left = array(); $nrec = 0; $points = 0;
		for ($i=0;$i<3;$i++) {
			$max = 0;
			foreach	($possible[$i] as $rt) {
				$max += $rt['max'] - $rt['pts'];
			}
			$left[$i] = max(0, $rbp[$i] - $max);
			if ($left[$i] == 0) {
				$nrec ++;
			} else {
				$points += $left[$i];
				$rbp[$i] -= $left[$i];
			}
		}

		// Reassign extra points if possible
		if ($points) {
			for ($i=0;$i<3;$i++) {
				if ($left[$i] == 0) {
					$rbp[$i] += floor($points / $nrec);
				}
			}
		}

		// Increment research points for existing research
		for ($i=0;$i<3;$i++) {
			while ($possible[$i][0]['pts'] && $npc[$i]) {
				$inc = min(floor($rbp[$i] / $npc[$i]), $possible[$i][0]['max'] - $possible[$i][0]['pts']);
				l::trace("Adding $inc points to research #{$possible[$i][0]['id']} for player #$id");
				$this->db->query(
					"UPDATE research_player SET points = points + $inc "
						. "WHERE research = {$possible[$i][0]['id']} AND player = $id"
				);
				array_shift($possible[$i]);
				$rbp[$i] -= $inc;
				$npc[$i] --;
			}
		}

		// Generate probabilities for new research: the lower a tech's required points,
		// the higher its probabillity
		$probabilities = array(array(), array(), array());
		for ($i=0;$i<3;$i++) {
			if ($rbp[$i] == 0) {
				continue;
			}

			$sum = 0;
			foreach ($possible[$i] as $idx => $t) {
				$sum += $t['max'];
			}

			foreach ($possible[$i] as $idx => $t) {
				$raw = $sum / $t['max'];
				$n = ceil($raw * $raw);
				for ($j=0;$j<$n;$j++) {
					array_push($probabilities[$i], $idx);
				}
			}
		}

		// Start new research
		$doneSomething = true;
		$addedTechs = array(array(), array(), array());
		for ($i=0;$i%3||$doneSomething;$i=($i+1)%3) {
			if ($i%3==0) {
				$doneSomething = false;
			}
			if (!$rbp[$i] || count($possible[$i]) == count($addedTechs[$i])) {
				continue;
			}

			$doneSomething = true;
			do {
				$ridx = $probabilities[$i][rand(0, count($probabilities[$i]) - 1)];
			} while (in_array($ridx, $addedTechs[$i]));
			array_push($addedTechs[$i], $ridx);

			$r = $possible[$i][$ridx];
			$inc = min($rbp[$i], $r['max']);
			$this->db->query(
				"UPDATE research_player SET points = points + $inc "
					. "WHERE research = {$r['id']} AND player = $id"
			);
			l::trace("Starting research {$r['id']} with $inc points for player $id");
			$rbp[$i] -= $inc;
		}
	}


	public function increasePopulation($id, $onVacation = false) {
		$rules = $this->rules->call('get', $id);
		if (!is_null($id) && $this->players->call('getProtectionLevel', $id)) {
			$rules['pop_growth_factor'] = min($rules['pop_growth_factor'] + 1, 5);
		}
		$pow = 1.45 - (0.05 * $rules['pop_growth_factor']);
		$vacFact = $onVacation ? 4 : 1;

		$q = $this->db->query("SELECT id,pop,max_pop,happiness,corruption,mfact+ifact FROM planet "
			. "WHERE owner" . (is_null($id) ? " IS NULL AND status=0" : "=$id") . " FOR UPDATE");
		while ($r = dbFetchArray($q)) {
			$hapVal = ($r[3] - 50) / 12;
			$hapFct = exp($hapVal) / ((exp($hapVal)+1)*9);

			// Population growth for players that are on vacation is 1/4th of normal
			$p = round($r[1] + pow($hapFct, $pow) * $r[1] * ($r[2] - $r[1]) / ($vacFact * $r[2]));
			$growthRatio = $r[1] / $p;

			// Increase corruption
			$cDec = 5 + 3 * $r[3] / 10;
			if (!is_null($id)) {
				$ncor = max(0, $r[4] - $cDec);
				$x = ($r[1] - 2000) / 48000;
				$optFact = ($r[1] / 30) - 754 * $x * $x;
				$mCorInc = $r[5] / 3;
				$nf = ($r[5] > $optFact * 3) ? ($optFact * 3) : $r[5];
				$x = abs($nf / $optFact - 1);
				$v = 10 + $x * $x * $x * 50;

				$ncor = min(32000, $ncor + $v);
			} else {
				$ncor = max(0, $r[4] - $cDec * 3);
			}
			$ncor = round($ncor * $growthRatio * $growthRatio * $growthRatio);

			$this->db->query("UPDATE planet SET pop=$p,corruption=$ncor WHERE id={$r[0]}");
			$this->planets->call('updateHappiness', $r[0]);
		}
	}


	public function updateWarehouse(&$players, &$playersById) {
		// Get missing data for the warehouse:
		$this->at = time();

		// -> IDR:
		$o = $this->rankings->call('getAll', $this->rankings->call('getType', 'p_idr'));
		foreach ($o as $r) {
			$players[$r['id']]['d_rank'] = $r['points'];
		}

		// -> Alliance tag:
		$q = $this->db->query("SELECT p.id,a.tag FROM alliance a,player p "
				. "WHERE (p.quit IS NULL OR UNIX_TIMESTAMP(NOW())-p.quit<86400)"
				. " AND p.a_status='IN' AND p.alliance IS NOT NULL AND NOT p.hidden"
				. " AND a.id=p.alliance");
		while ($r = dbFetchArray($q)) {
			$players[$playersById[$r[0]]]['a_tag'] = $r[1];
		}

		// -> Tech lists
		$lsts = array();
		$q = $this->db->query("SELECT p.id,r.id FROM research r,research_player j,player p "
				. "WHERE (p.quit IS NULL OR UNIX_TIMESTAMP(NOW())-p.quit<86400) AND NOT p.hidden"
				. " AND j.player=p.id AND j.research=r.id AND j.points=r.points");
		while ($r = dbFetchArray($q)) {
			if (!is_array($lsts[$r[0]])) {
				$lsts[$r[0]] = array();
			}
			array_push($lsts[$r[0]], $r[1]);
		}
		foreach ($lsts as $pid => $tl) {
			$players[$playersById[$pid]]['tech_list'] = join(',', $tl);
		}

		// -> Planet lists and details
		$q = $this->db->query("SELECT p.id,r.id,r.name,r.pop,r.max_pop,r.ifact,r.mfact,r.turrets,"
					. "r.happiness,r.beacon,r.abandon,r.corruption "
				. "FROM planet r,player p "
				. "WHERE (p.quit IS NULL OR UNIX_TIMESTAMP(NOW())-p.quit<86400) AND NOT p.hidden"
				. " AND r.owner=p.id");
		$plPlanets = array();
		while ($r = dbFetchArray($q)) {
			if (!is_array($plPlanets[$r[0]])) {
				$plPlanets[$r[0]] = array();
			}
			array_push($plPlanets[$r[0]], array(
				"planet"	=> $r[1],
				"name"		=> $r[2],
				"pop"		=> $r[3],
				"max_pop"	=> $r[4],
				"ifact"		=> $r[5],
				"mfact"		=> $r[6],
				"turrets"	=> $r[7],
				"happiness"	=> $r[8],
				"beacon"	=> $r[9],
				"abandon"	=> $r[10],
				"corruption"	=> $r[11]
			));
		}


		// Update the warehouse
		foreach ($players as $name => $data) {
			$pid = $data['player'];
			if (is_null($pid)) {
				l::warn("****** BUG: null player ID in the warehouse update loop");
				continue;
			}
			foreach ($data as $k => $v) {
				$data[$k] = "'" . addslashes($v) . "'";
			}
			if (is_null( $data['tech_list'] )) {
				$data['tech_list'] = "''";
			}

			$qs = "INSERT INTO warehouse(" . join(',', array_keys($data))
				. ") VALUES (" . join(',', array_values($data)) . ")";
			$this->db->query($qs);

			$q = $this->db->query("SELECT id FROM warehouse WHERE tick_at={$this->at} AND player=$pid");
			list($id) = dbFetchArray($q);
			if (!$id || is_null($plPlanets[$pid])) {
				continue;
			}

			foreach ($plPlanets[$pid] as $pdata) {
				$pdata['id'] = $id;

				foreach ($pdata as $k => $v) {
					$pdata[$k] = "'" . addslashes($v == '' ? '0' : $v) . "'";
				}

				$qs = "INSERT INTO wh_planet(" . join(',', array_keys($pdata))
					. ") VALUES (" . join(',', array_values($pdata)) . ")";
				$this->db->query($qs);
			}
		}
		$this->db->query("DELETE FROM warehouse WHERE UNIX_TIMESTAMP(NOW())-tick_at>86400*60");
	}
}


?>

<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/library/updateRankings.inc
//
// This function computes the rankings for a Beta 5 game instance.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_updateRankings {

	private $lib;
	private $db;
	private $game;

	private $fleets;
	private $planets;
	private $players;
	private $rankings;

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->fleets	= $lib->game->getLib('beta5/fleet');
		$this->planets	= $this->game->getLib('beta5/planet');
		$this->players	= $this->game->getLib('beta5/player');
		$this->rankings	= $lib->game->getLib('main/rankings');
	}


	public function run($dryRun = false) {
		// Compute and update player rankings
		list($players, $playersById, $genRankings) = $this->computePlayerRankings($dryRun);

		// Round and alliance rankings
		$this->computeRoundRankings($players, $dryRun);
		$this->computeAllianceRankings($genRankings, $dryRun);

		return array($players, $playersById);
	}


	private function computePlayerRankings($dryRun) {
		// Compute player rankings
		$this->at = time();
		$q = $this->db->query("SELECT id FROM player "
			. "WHERE (quit IS NULL OR UNIX_TIMESTAMP(NOW()) - quit < 86400) AND NOT hidden");
		$genR = $civR = $milR = $finR = array();
		$genRankings = array();
		$players = array();
		$playersById = array();
		while ($r = dbFetchArray($q)) {

			// Get player name
			$pn = $this->players->call('getName',$r[0]);
			$players[$pn] = array(
				"player"	=> $r[0],
				"tick_at"	=> $this->at,
			);
			$playersById[$r[0]] = $pn;

			// Get planet data
			$q2 = $this->db->query(
				"SELECT SUM(pop), SUM(ifact), SUM(mfact), SUM(turrets), "
				     . "AVG(happiness), SUM(beacon), AVG(corruption) "
				. "FROM planet WHERE owner={$r[0]}"
			);
			$pld = dbFetchArray($q2);

			// Get fleet data
			$q2 = $this->db->query(
				"SELECT SUM(gaships), SUM(fighters), SUM(cruisers), SUM(bcruisers) "
				. "FROM fleet WHERE owner={$r[0]}");
			$fld = dbFetchArray($q2);
			$players[$pn]['gaships'] = (int) $fld[0];
			$players[$pn]['fighters'] = (int) $fld[1];
			$players[$pn]['cruisers'] = (int) $fld[2];
			$players[$pn]['bcruisers'] = (int) $fld[3];

			// Financial ranking
			$q2 = $this->db->query("SELECT cash FROM player WHERE id={$r[0]}");
			list($cash) = dbFetchArray($q2);
			$players[$pn]['cash'] = $cash;

			$q2 = $this->db->query(
				"SELECT pop, ifact, mfact, turrets, corruption, happiness "
				. "FROM planet WHERE owner={$r[0]}"
			);
			$income = 0;
			while ($r2 = dbFetchArray($q2)) {
				$ir = $this->planets->call('getIncome', $r[0], $r2[0], $r2[5],
					$r2[1], $r2[2], $r2[3], $r2[4]);
				$income += $ir[0];
			}
			$upkeep = $this->fleets->call('getUpkeep', $r[0], $fld[0], $fld[1], $fld[2], $fld[3]);
			$profit = max(0, $income[0] - $upkeep);
			$fr = round($cash / 2000);
			$fr += round($profit / 2) + round($income / 1.5);
			$fr += round($pld[1] * 201);
			if (!is_array($finR[$fr])) {
				$finR[$fr] = array();
			}
			array_push($finR[$fr], $pn);
			$players[$pn]['f_rank'] = $fr;

			// Military ranking
			$fpower = $this->fleets->call('getPower', $r[0], $fld[0], $fld[1], $fld[2], $fld[3]);
			$players[$pn]['fleet'] = $fpower;
			$mr = $pld[3] * 8 + $fpower * 7 + $pld[2] * 40 + $pld[5] * 100;
			if (!is_array($milR[$mr])) {
				$milR[$mr] = array();
			}
			array_push($milR[$mr], $pn);
			$players[$pn]['m_rank'] = $mr;

			// Civilian ranking
			$q2 = $this->db->query("SELECT SUM(points) FROM research_player WHERE player={$r[0]}");
			list($rpoints) = dbFetchArray($q2);
			$players[$pn]['tech_points'] = $rpoints;
			$cr = round(pow(1 + $pld[4] / 100, 4) * $pld[0] * 3);
			$cr -= round($cr * $pld[6] / 48000);
			$cr += round(sqrt($rpoints*1000) * 3);
			if (!is_array($civR[$cr])) {
				$civR[$cr] = array();
			}
			array_push($civR[$cr], $pn);
			$players[$pn]['c_rank'] = $cr;

			// General ranking
			$gr = $cr + $mr + $fr;
			if (!is_array($genR[$gr])) {
				$genR[$gr] = array();
			}
			array_push($genR[$gr], $pn);
			$genRankings[$r[0]] = $gr;
			$players[$pn]['g_rank'] = $gr;
		}

		// Update player rankings if this is not a dry run
		if (! $dryRun) {
			$rankings = array($finR, $milR, $civR, $genR);
			$rTypes = array('p_financial', 'p_military', 'p_civ', 'p_general');
			for ($i = 0; $i < count($rTypes); $i ++) {
				$rt = $this->rankings->call('getType', $rTypes[$i]);
				$this->rankings->call('update', $rt, $rankings[$i]);
			}
		}

		return array($players, $playersById, $genRankings);
	}


	private function computeRoundRankings(&$players, $dryRun) {
		// Get the general top 15
		$rt = $this->rankings->call('getType', 'p_general');
		$rr = $this->rankings->call('getAll', $rt, 15);
		$top15 = array();
		foreach ($rr as $r) {
			$top15[$r['id']] = array($r['ranking'], $r['points']);
		}

		// Generate the new values
		$rt = $this->rankings->call('getType', 'p_round');
		$o = $this->rankings->call('getAll', $rt);
		$round = array();
		foreach ($o as $r) {
			$round[$r['id']] = $r['points'];
		}
		foreach	($top15 as $n => $r) {
			$round[$n] += round((16-$r[0]) * $r[1] / 24000);
		}

		// Update the rankings
		$rndR = array();
		foreach	($round as $n => $p) {
			if (!is_array($rndR[$p])) {
				$rndR[$p] = array();
			}
			array_push($rndR[$p], $n);
			$players[$n]['o_rank'] = $p;
		}
		if (! $dryRun) {
			$this->rankings->call('update', $rt, $rndR);
		}
	}


	private function computeAllianceRankings(&$genRankings, $dryRun) {
		// Sum up the values
		$q = $this->db->query(
			"SELECT p.id, a.tag FROM player p,alliance a "
				. "WHERE p.alliance=a.id AND p.a_status='IN' "
				  . "AND (p.quit IS NULL OR UNIX_TIMESTAMP(NOW()) - p.quit < 86400)"
		);
		$alliances = array();
		while ($r = dbFetchArray($q)) {
			if (is_null($alliances[$r[1]])) {
				$alliances[$r[1]] = $genRankings[$r[0]];
			} else {
				$alliances[$r[1]] += $genRankings[$r[0]];
			}
		}

		// If there are victory conditions, add that to the alliances' score
		if ($this->lib->game->params['victory'] == 1) {
			$q = $this->db->query("SELECT id,tag FROM alliance");
			$aLib = $this->lib->game->getLib('beta5/alliance');
			while ($r = dbFetchArray($q)) {
				$vVal = (100 + $aLib->call('updateVictory', $r[0])) / 100;
				$alliances[$r[1]] = round($alliances[$r[1]] * $vVal);
			}
		} elseif ($this->lib->game->params['victory'] == 2) {
			$q = $this->db->query("SELECT id,tag FROM alliance");
			$aLib = $this->lib->game->getLib('beta5/ctf');
			while ($r = dbFetchArray($q)) {
				$vVal = (100 + $aLib->call('getTeamPoints', $r[0])) / 100;
				$alliances[$r[1]] = round($alliances[$r[1]] * $vVal);
			}
		}

		// Update the rankings
		$allR = array();
		foreach	($alliances as $n => $p) {
			if (!is_array($allR[$p])) {
				$allR[$p] = array();
			}
			array_push($allR[$p], $n);
		}
		if (! $dryRun) {
			$rt = $this->rankings->call('getType', 'a_general');
			$this->rankings->call('update', $rt, $allR);
		}
	}
}


?>

<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
		"func"	=> array(
			"getCash", "getCashDetails", "transferFunds"
		),
		"init"	=> "makeMoneyTooltips();\nx_getCash(displayCash); x_getCashDetails(displayPage);"
	);

	function	getCash() {
		$pi = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
		return	$pi['cash'];
	}

	function	getCashDetails() {
		$pr = gameAction('isPlayerRestrained', $_SESSION[game::sessName()]['player']);
		$str = "$pr";
		$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
		$str .= "#" . (gameAction('isOnVacation', $_SESSION[game::sessName()]['player']) ? 1 : 0);
		$income = $upkeep = 0;

		$ppl = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$str .= "#" . count($ppl);
		$strp = "";
		foreach ($ppl as $id => $name)
		{
			$info = gameAction('getPlanetById', $id);
			if	($strp != "")
				$strp .= "\n";
			$strp .= "$name\n$id#";
			$m = gameAction('getPlanetIncome', $info['owner'], $info['pop'], $info['happiness'], $info['ifact'], $info['mfact'], $info['turrets'], $info['corruption']);
			$income += $m[0];
			$strp .= join('#', $m);
			$strp .= '#' . $info['ifact'];
		}

		$pfl = gameAction('getPlayerFleets', $_SESSION[game::sessName()]['player']);
		$str .= "#" . count($pfl);
		$strf = "";
		foreach ($pfl as $id => $name)
		{
			$info = gameAction('getFleet', $id);
			if	($strf != "")
				$strf .= "\n";
			$strf .= "$name\n";
			if	(is_null($info['move']) && is_null($info['wait']))
			{
				$pinf = gameAction('getPlanetById', $info['location']);
				$strf .= $pinf['name'] . "\n$id#0#0";
			}
			elseif	(is_null($info['move']))
			{
				$pinf = gameAction('getPlanetById', $info['wait']['drop_point']);
				$strf .= $pinf['name'] . "\n$id#0#" . $info['wait']['time_left'];
			}
			else
			{
				$pinf = gameAction('getPlanetById', $info['move']['m_to']);
				$strf .= $pinf['name'] . "\n$id#" . $info['move']['time_to_arrival'] . "#";
				$strf .= (is_null($info['wait']) ? 0 : $info['wait']['time_left']);
			}
			$u = gameAction('getFleetUpkeep', $_SESSION[game::sessName()]['player'],
				$info['gaships'], $info['fighters'], $info['cruisers'], $info['bcruisers']);
			$upkeep += $u;
			$strf .= "#$u";
		}

		$profit = $income - $upkeep;
		$str .= "#$income#$upkeep#$profit\n$strp\n$strf";
		return	$str;
	}

	public function transferFunds($target, $amount) {
		if ((int)$amount != $amount || $amount <= 0) {
			return 1;
		}

		if ($target == "") {
			return 2;
		}

		$sid = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');
		$tid = $pLib->call('getPlayerId', $target);
		if (is_null($tid)) {
			return 3;
		}
		if ($sid == $tid) {
			return 4;
		}

		if ($pLib->call('isRestrained', $tid)) {
			return 5;
		}
		if ($pLib->call('isRestrained', $sid)) {
			return 6;
		}

		$sourcePLevel = $pLib->call('getProtectionLevel', $sid);
		if ($sourcePLevel) {
			return 7;
		}
		$targetPLevel = $pLib->call('getProtectionLevel', $tid);
		if ($targetPLevel) {
			return 8;
		}

		$p = gameAction('getPlayerInfo', $sid);
		if ($p['cash'] < $amount) {
			return 9;
		}

		$vac = $this->game->getLib('beta5/player');
		if ($vac->call('isOnVacation', $sid) || $vac->call('isOnVacation', $tid)) {
			return 10;
		}

		gameAction('transferFunds', $sid, $tid, $amount);
		return 0;
	}

	function	handle($input)
	{
		$this->output = "money";
	}
}

?>

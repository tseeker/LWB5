<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
		"func"	=> array(
				"getInitVals", "getPlanetList", "setMode",
				"setCumulative", "getMilitaryLevel",
				"flushQueues", "addToQueues", "factoryAction",
				"deleteItems", "moveItems", "replaceItems"
			),
		"init"	=> "makePlanetsTooltips();\nx_getInitVals(initGetMode);"
	);

	function getPlanetList() {
		$str = "";
		$l = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		foreach	($l as $id => $name) {
			$info = gameAction('getPlanetById', $id);
			if	($str != "")
				$str .= "\n";
			$str .= "$id#" . $info["x"] . "#" . $info["y"] . "#" . ($info['orbit'] + 1);
			$str .= "#" . $info["pop"] . '#' . $info['happiness'];
			$str .= "#" . $info["ifact"] . '#' . $info['turrets'];
			$str .= "#" . $info["mfact"] . '#';
			$m = gameAction('getPlanetIncome', $info['owner'], $info['pop'], $info['happiness'], $info['ifact'], $info['mfact'], $info['turrets'], $info['corruption']);
			$str .= $m[0] . "#" . round($info['corruption'] / 320);
			$str .= "\n$name\n";

			$bq = gameAction('getBuildQueue', $id);
			$bqs = '';
			foreach ($bq as $bqi)
			{
				if	($bqs != "")
					$bqs .= "#";
				$bqs .= $bqi["type"] . "#" . $bqi['quantity'] . "#" . $bqi['time'] . "#" . $bqi['ctime'];
			}
			$str .= $bqs;
		}

		return	$str;
	}

	function	getInitVals() {
		$vac = gameAction('isOnVacation', $_SESSION[game::sessName()]['player']);

		if (is_null($_SESSION[game::sessName()]['quick_builder']) || $vac) {
			$_SESSION[game::sessName()]['quick_builder'] = false;
		}
		if (is_null($_SESSION[game::sessName()]['plist_cumulative'])) {
			$_SESSION[game::sessName()]['plist_cumulative'] = true;
		}
		return ($_SESSION[game::sessName()]['quick_builder'] ? "1" : "0") . '#' . ($_SESSION[game::sessName()]['plist_cumulative'] ? "1" : "0")
			. "#" . ($vac ? 1 : 0);
	}

	function setMode($val) {
		$vac = gameAction('isOnVacation', $_SESSION[game::sessName()]['player']);
		$_SESSION[game::sessName()]['quick_builder'] = ($val == "1" && !$vac);
	}

	function setCumulative($val) {
		$_SESSION[game::sessName()]['plist_cumulative'] = ($val == "1");
	}

	function getMilitaryLevel() {
		$r = gameAction('loadPlayerRules', $_SESSION[game::sessName()]['player']);
		if	(is_null($r))
			return	"0";
		return	$r['military_level'];
	}

	function flushQueues($planets) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return 13;
		}

		$pl = explode('#', $planets);
		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		foreach	($pl as $pid) {
			if (!in_array($pid, $ids)) {
				continue;
			}
			gameAction('flushBuildQueue', $pid);
		}
		return	"OK";
	}

	function addToQueues($planets, $nb, $type) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return 13;
		}

		if ((int)$nb != $nb || $nb <= 0) {
			return	2;
		}

		$r = gameAction('loadPlayerRules', $_SESSION[game::sessName()]['player']);
		if ($type < 0 || $type > $r['military_level'] + 2) {
			return	-1;
		}

		$pl = explode('#', $planets);
		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		foreach	($pl as $pid) {
			if (!in_array($pid, $ids)) {
				return	-1;
			}
		}

		$types = array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');
		$cost = $r['build_cost_'.$types[$type]] * $nb * count($pl);
		$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
		if ($pinf['cash'] < $cost) {
			return	4;
		}

		foreach	($pl as $pid) {
			gameAction('addToBuildQueue', $pid, $nb, $type);
		}

		return	"OK";
	}

	function factoryAction($planets, $act, $nb, $type) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return 13;
		}

		if ((int)$nb != $nb || $nb <= 0) {
			return 3;
		}

		$t = ($type == '1') ? "m" : "i";
		$r = gameAction('loadPlayerRules', $_SESSION[game::sessName()]['player']);

		$pl = explode('#', $planets);
		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		foreach	($pl as $pid) {
			if (!in_array($pid, $ids)) {
				return	-1;
			}
		}

		if ($act != "1") {
			// Check cash first
			$cost = $nb * $r[$t . 'f_cost'] * count($pl) ;
			$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
			if ($pinf['cash'] < $cost) {
				return 4;
			}

			// Now check other planet details
			$planets = $this->game->getLib('beta5/planet');
			foreach	($pl as $pid) {
				$err = $planets->call('checkBuildFactories', $pid, $nb, $t);
				if ($err) {
					return $err + 13;
				}
			}
		} else {
			foreach	($pl as $pid) {
				$err = gameAction('checkDestroyFactories', $pid, $nb, $t);
				if ($err) {
					return	$err + 4;
				}
			}
		}

		$ga = ($act == "1") ? "destroyFactories" : "buildFactories";
		foreach	($pl as $pid) {
			gameAction($ga, $pid, $nb, $t);
		}

		return "OK";
	}

	function deleteItems($items) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return 13;
		}

		$lst = explode('#', $items);
		$items = array();
		foreach	($lst as $i) {
			array_push($items, explode('-', $i));
		}
		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);

		$pll = array();
		foreach	($items as $i)
		{
			list($pl, $it) = $i;
			if	(!(in_array($pl, $ids) && preg_match('/^[0-9]+$/', $it)))
				continue;
			if	(!in_array($pl, $pll))
				array_push($pll, $pl);
			gameAction('deleteBuildQueueItem', $pl, $it);
		}
		foreach	($pll as $pid)
			gameAction('reorderBuildQueue', $pid);

		return	"OK";
	}

	function replaceItems($items, $nb, $type) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return 13;
		}

		if	((int)$nb != $nb || $nb <= 0)
			return	2;
		$r = gameAction('loadPlayerRules', $_SESSION[game::sessName()]['player']);
		if	($type < 0 || $type > $r['military_level'] + 2)
			return	-1;

		$lst = explode('#', $items);
		$rList = array();
		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		foreach	($lst as $li)
		{
			list($pl,$it) = explode('-', $li);
			if	(!is_array($rList[$pl]))
			{
				if	(!in_array($pl, $ids))
					return	-1;
				$rList[$pl] = array();
			}
			if	(!preg_match('/^[0-9]+$/', $it))
				return	-1;
			array_push($rList[$pl], $it);
		}

		$types = array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');
		$icost = $r['build_cost_'.$types[$type]] * $nb;

		$ids = array_keys($rList);
		$sum = 0;
		foreach	($ids as $pl)
			$sum += gameAction('getReplacementCost', $pl, $rList[$pl], $icost);

		$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
		if	($pinf['cash'] < $sum)
			return	4;

		foreach	($ids as $pl)
			gameAction('replaceItems', $pl, $rList[$pl], $nb, $type);

		return	"OK";
	}

	function moveItems($items, $dir) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return 13;
		}

		$lst = explode('#', $items);
		$rList = array();
		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		foreach	($lst as $li)
		{
			list($pl,$it) = explode('-', $li);
			if	(!is_array($rList[$pl]))
			{
				if	(!in_array($pl, $ids))
					return	-1;
				$rList[$pl] = array();
			}
			if	(!preg_match('/^[0-9]+$/', $it))
				return	-1;
			array_push($rList[$pl], $it);
		}

		$ids = array_keys($rList);
		foreach	($ids as $pl)
		{
			sort($rList[$pl]);
			if	($dir != '1')
			{
				$bql = gameAction('getQueueLength', $pl);
				if	($rList[$pl][0] >= $bql - 1)
					return	12;
				$rList[$pl] = array_reverse($rList[$pl]);
			}
			elseif	($rList[$pl][0] == 0)
				return	11;
		}

		$ga = ($dir == '1') ? "Up" : "Down";
		foreach	($ids as $pl)
		{
			foreach	($rList[$pl] as $it)
				gameAction('moveItem' . $ga, $pl, $it);
		}

		return	"OK";
	}

	function handle($input) {
		$this->output = "planets";
	}
}

?>

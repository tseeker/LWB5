<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
		"func"	=> array(
				"getPlanetList", "getPlanetData", "addToQueue",
				"factoryAction", "replaceItems", "flushQueue",
				"moveItems", "deleteItems", "rename",
				"planetSale", "cancelSale", "abandon",
				"cancelAbandon", "blowItUp", "cancelDestruction",
				"getSellableFleets", "destroyTurrets"
			),
		"init"	=> "initPlanetPage();"
	);

	function getPlanetList() {
		$l = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$str = count($l);
		foreach	($l as $id => $name)
			$str .= "\n$id#$name";

		return	$str;
	}

	function getPlanetData($id) {
		// Get planet data
		$id = (int)$id;
		$info = gameAction('getPlanetById', $id);
		if (is_null($info)) {
			return	"";
		}
		$pid = $_SESSION[game::sessName()]['player'];

		// Build result array
		$result = array(
			"{$info['status']}#$id#{$info['x']}#{$info['y']}#" . ($info['orbit']+1)
			. "#" . utf8entities($info['name'])
		);

		// Get probe report
		$probeData = null;	// FIXME

		// Build fleets summary
		$ownFleets = array_keys(gameAction('getFleetsAt', $id, $pid));
		$allFleets = array_keys(gameAction('getFleetsAt', $id));
		$ownSum = 0;
		if	(count($ownFleets) || $info['owner'] == $pid)
		{
			$attSum = 0;
			$defSum = 0;
			$ownMode = ($info['owner'] == $pid ? 1 : null);
			foreach	($allFleets as $fid)
			{
				$finf = gameAction('getFleet', $fid);
				if	($finf['owner'] == $pid)
				{
					if	(is_null($ownMode))
						$ownMode = ($finf['attacking'] == 't') ? 2 : 1;
					$ownSum += gameAction('getFleetPower', $finf['owner'], 0, $finf['gaships'], $finf['fighters'], $finf['cruisers'], $finf['bcruisers']);
				}
				elseif	($finf['attacking'] == 't')
					$attSum += gameAction('getFleetPower', $finf['owner'], 0, $finf['gaships'], $finf['fighters'], $finf['cruisers'], $finf['bcruisers']);
				else
					$defSum += gameAction('getFleetPower', $finf['owner'], 0, $finf['gaships'], $finf['fighters'], $finf['cruisers'], $finf['bcruisers']);
			}
			if	(is_null($ownMode))
				$ownMode = 0;
			array_push($result, "$ownMode#$ownSum#$attSum#$defSum");
		}
		elseif	(!is_null($probeData))
		{
			// FIXME: get fleets data from probe report
		}
		else
			array_push($result, "0###");

		// If the planet has been destroyed or is a nebula, we're done
		if	($info['status'] != 0)
			return	join("\n", $result);

		// Get owner and player data
		$isOwn = ($info['owner'] == $pid);
		$owner = is_null($info['owner']) ? null : gameAction('getPlayerInfo', $info['owner']);
		$player = $isOwn ? $owner : gameAction('getPlayerInfo', $pid);
		$protected = is_null($owner) ? false : (
			$this->game->getLib('beta5/player')->call('getProtectionLevel', $info['owner']) > 0
		);
		$plPriv = gameAction('getAlliancePrivileges', $pid);
		$vacation = ($isOwn && $info['vacation'] != 'NO  ')
			|| (!$isOwn && $info['vacation'] == 'YES ');

		// Build planet summary:
		// - isOwner
		$s = ($isOwn ? 1 : 0) . "#" . ($vacation ? 1 : 0) . "#" . ($protected ? 1 : 0) . "#";

		// - turrets
		if	($isOwn || $ownSum || ($player['aid'] == $owner['aid'] && $plPriv['list_access'] == 3))
			$s .= $info['turrets'];
		elseif	(!is_null($probeData))
			$s .= "";		//FIXME: use probe report
		$s .= "#";
		// - population
		if	($isOwn || $ownSum)
			$s .= $info['pop'];
		elseif	(!is_null($probeData))
			$s .= "";		//FIXME: use probe report
		else
			$s .= "";
		$s .= "#";
		// - factories
		if	(!$isOwn && $player['aid'] == $owner['aid'] && $plPriv['list_access'] == 3)
			$s .= ((int)$info['ifact'] + (int)$info['mfact']);
		elseif	(!is_null($probeData))	// FIXME: don't include it if the probe got the exact amounts
			$s .= "";		// FIXME: use probe report
		$s .= "#";
		if	($isOwn)
			$s .= $info['ifact'] . "#" . $info['mfact'];
		elseif	(!is_null($probeData))	// FIXME: only include it if the probe got the exact amounts
			$s .= "#";		// FIXME: use probe report
		else
			$s .= "#";
		$s .= "#";
		// - happiness
		if	($isOwn)
			$s .= $info['happiness'];
		elseif	(!is_null($probeData))
			$s .= "";		// FIXME: use probe report
		$s .= "#";
		// - corruption
		if	($isOwn)
			$s .= round($info['corruption'] / 320);
		elseif	(!is_null($probeData))
			$s .= "";		// FIXME: use probe report
		$s .= "#";
		// - profit
		if ($isOwn) {
			$m = gameAction('getPlanetIncome', $info['owner'], $info['pop'], $info['happiness'], $info['ifact'], $info['mfact'], $info['turrets'], $info['corruption']);
			$s .= $m[0];
		}
		$s .= "#";
		// - alliance tag
		$s .= $owner['alliance'];
		array_push($result, $s);

		// We're done if we're not the owner
		if	(!$isOwn)
			return	join("\n", $result);
		$rules = gameAction('loadPlayerRules', $pid);

		// Current action
		if	(!is_null($info['bh_prep']))
			$s = "2#".($info['bh_prep'] + 1);
		elseif	(!is_null($info['abandon']))
			$s = "3#".($info['abandon']);
		else
		{
			$offer = gameAction('getPlanetSale', $id);
			if	(is_null($offer))
			{
				$restr = gameAction('isPlayerRestrained', $pid);
				$np = gameAction('getRealPlanetCount', $pid);
				$canSell = ($restr == 0) && ($np > 1);
				$q = dbQuery("SELECT bh_unhappiness FROM player WHERE id=$pid");
				list($bhu) = dbFetchArray($q);
				$q = dbQuery("SELECT COUNT(*) FROM planet WHERE owner=$pid AND bh_prep IS NOT NULL");
				list($bhp) = dbFetchArray($q);
				$canBlow = ($np > 1) && ($rules['planet_destruction'] > 0) && ($bhu < 20) && ($bhp < 4);
				if ($canBlow && $this->game->params['victory'] == 2) {
					// Prevent planets from being destroyed in CTF games
					$ctfLib = $this->game->getLib('beta5/ctf');
					$canBlow = !($ctfLib->call('isTarget', $info['system']));
				}

				$s = "0#" . $info['rename'] . "#" . ($canSell ? 1 : 0) . "#" . ($canBlow ? 1 : 0) . "#" . ($np > 1 ? 1 : 0);
			}
			else
			{
				$s  = "1#" . (is_null($offer['finalized']) ? 0 : ($info['sale'] + 1));
				$s .= "#" . $offer['sold_to'] . "#";
				if	(!is_null($offer['sold_to']))
					$s .= utf8entities(gameAction('getPlayerName', $offer['sold_to']));
			}
		}
		array_push($result, $s);

		// Capabilities and prices
		$s  = $rules['military_level'] . "#" . $rules['if_cost'] . "#" . $rules['mf_cost'] . "#" .  $rules['bh_cost'] . "#";
		$s .= $rules['build_cost_turret'] . "#" . $rules['build_cost_gaship'] . "#" . $rules['build_cost_fighter'];
		$s .= "#" . $rules['build_cost_cruiser'] . "#" . $rules['build_cost_bcruiser'];
		array_push($result, $s);

		// Build queue
		$bq = gameAction('getBuildQueue', $id);
		foreach ($bq as $bqi)
			array_push($result, $bqi["type"] . "#" . $bqi['quantity'] . "#" . $bqi['time'] . "#" . $bqi['ctime']);

		return	join("\n", $result);
	}

	function factoryAction($pid, $act, $nb, $type) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		if ((int)$nb != $nb || $nb <= 0) {
			return "ERR#3";
		}

		$t = ($type == '1') ? "m" : "i";

		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		if (!in_array($pid, $ids)) {
			return "ERR#-1";
		}

		if ($act != "1") {
			$planets = $this->game->getLib('beta5/planet');
			$err = $planets->call('checkBuildFactories', $pid, $nb, $t);
			if ($err) {
				return "ERR#" . ($err + 23);
			}
		} else {
			$err = gameAction('checkDestroyFactories', $pid, $nb, $t);
			if ($err) {
				return "ERR#" . ($err + 7);
			}
		}

		gameAction(($act == "1") ? "destroyFactories" : "buildFactories", $pid, $nb, $t);
		return	$this->getPlanetData($pid);
	}

	function addToQueue($pid, $nb, $type) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		if ((int)$nb != $nb || $nb <= 0) {
			return	"ERR#2";
		}

		$r = gameAction('loadPlayerRules', $_SESSION[game::sessName()]['player']);
		$type = (int)$type;
		if	($type < 0 || $type > $r['military_level'] + 2)
			return	"ERR#1";

		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		if	(!in_array($pid, $ids))
			return	"ERR#-1";

		$types = array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');
		$cost = $r['build_cost_'.$types[$type]] * $nb;
		$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
		if	($pinf['cash'] < $cost)
			return	"ERR#0";
		gameAction('addToBuildQueue', $pid, $nb, $type);
		return	$this->getPlanetData($pid);
	}

	function replaceItems($pid, $items, $nb, $type) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		if	((int)$nb != $nb || $nb <= 0)
			return	"ERR#2";
		$r = gameAction('loadPlayerRules', $_SESSION[game::sessName()]['player']);
		if	($type < 0 || $type > $r['military_level'] + 2)
			return	"ERR#1";

		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		if	(!in_array($pid, $ids))
			return	"ERR#-1";

		$lst = explode('#', $items);
		if	(!count($lst))
			return	"ERR#5";
		foreach	($lst as $it)
			if	(!preg_match('/^[0-9]+$/', $it))
				return	"ERR#-2";

		$types = array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');
		$icost = $r['build_cost_'.$types[$type]] * $nb;
		$sum = gameAction('getReplacementCost', $pid, $lst, $icost);
		$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
		if	($pinf['cash'] < $sum)
			return	"ERR#0";
		gameAction('replaceItems', $pid, $lst, $nb, $type);
		return	$this->getPlanetData($pid);
	}

	function moveItems($pid, $items, $dir) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		if	(!in_array($pid, $ids))
			return	"ERR#-1";

		$lst = explode('#', $items);
		if	(!count($lst))
			return	5;
		foreach	($lst as $li)
			if	(!preg_match('/^[0-9]+$/', $li))
				return	"ERR#-2";

		sort($lst);
		if	($dir != '1')
		{
			$bql = gameAction('getQueueLength', $pid);
			if	($lst[0] >= $bql - 1)
				return	"ERR#8";
			$lst = array_reverse($lst);
		}
		elseif	($lst[0] == 0)
			return	"ERR#7";

		$ga = ($dir == '1') ? "Up" : "Down";
		foreach	($lst as $it)
			gameAction('moveItem' . $ga, $pid, $it);

		return	$this->getPlanetData($pid);
	}

	function deleteItems($pid, $sel) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		if	(!in_array($pid, $ids))
			return	"ERR#-1";

		$items = explode('#', $sel);
		if	(!count($items))
			return	"ERR#5";
		foreach	($items as $i)
		{
			if	(!preg_match('/^[0-9]+$/', $i))
				continue;
			gameAction('deleteBuildQueueItem', $pid, $i);
		}
		gameAction('reorderBuildQueue', $pid);

		return	$this->getPlanetData($pid);
	}

	function flushQueue($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$plp = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$ids = array_keys($plp);
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		gameAction('flushBuildQueue', $pid);
		return	$this->getPlanetData($pid);
	}

	function rename($pid, $name) {
		$player = $_SESSION[game::sessName()]['player'];
		$playerLib = $this->game->getLib('beta5/player');

		if ($playerLib->call('isOnVacation', $player)) {
			return "ERR#200";
		}

		$plp = $playerLib->call('getPlanets', $player);
		$ids = array_keys($plp);
		if (!in_array($pid, $ids)) {
			return "ERR#-1";
		}

		$n = trim($name);
		$err = $this->game->getLib()->call('checkPlanetName', $n);
		if ($err) {
			if ($err == 6) {
				return "ERR#38";
			}
			return "ERR#". ($err + 12);
		}

		$planetLib = $this->game->getLib('beta5/planet');
		$salesLib = $this->game->getLib('beta5/sale');

		$info = $planetLib->call('byId', $pid);
		$offer = $salesLib->call('getPlanetSale', $pid);
		if (is_null($info['abandon']) && is_null($info['bh_prep']) && $info['rename'] && is_null($offer)) {
			$planetLib->call('rename', $pid, $n);
		}
		return $this->getPlanetData($pid);
	}

	function planetSale($pid, $mode, $player, $price, $expires, $fleets) {
		$cPlayer = $_SESSION[game::sessName()]['player'];
		if (gameAction('isOnVacation', $cPlayer)) {
			return "ERR#200";
		} elseif (gameAction('getProtectionLevel', $cPlayer)) {
			return "ERR#201";
		}

		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$info = gameAction('getPlanetById', $pid);
		$offer = gameAction('getPlanetSale', $pid);
		if	(!(is_null($info['abandon']) && is_null($info['bh_prep']) && is_null($offer)))
			return	"ERR#-1";
		$restr = gameAction('isPlayerRestrained', $cPlayer);
		$np = gameAction('getRealPlanetCount', $cPlayer);
		if	($restr > 0 || $np <= 1)
			return	"ERR#-1";

		$mode = (int)$mode;
		if	($mode < 0 || $mode > 3)
			return	"ERR#-1";

		if	($mode < 2)
		{
			$player = trim($player);
			if	($player == '')
				return	"ERR#1";
			$tPid = gameAction('getPlayer', $player);
			if	(is_null($tPid))
				return	"ERR#4";
			elseif	($tPid == $cPlayer) {
				return	"ERR#6";
			} elseif (gameAction('getProtectionLevel', $tPid)) {
				return "ERR#5";
			}
		}
		else
			$tPid = null;

		if	($mode != 0)
		{
			$price = (int)$price;
			if	($price <= 0)
				return	"ERR#2";
		}
		else
			$price = 0;

		$expOk = array(0, 6, 12, 24, 48, 72, 96, 120);
		$expires = (int)$expires;
		if	(!in_array($expires,$expOk))
			return	"ERR#-1";
		elseif	($mode == 3 && $expires == 0)
			return	"ERR#3";

		// Check fleets
		if	($fleets != "")
		{
			$sFleets = split('#', $fleets);
			$flist = array_keys(gameAction('getFleetsAt', $pid, $cPlayer));
			foreach	($sFleets as $fid)
			{
				if	(!in_array($fid, $flist))
					return	"ERR#7";
				$f = gameAction('getFleet', $fid);
				if	($f['can_move'] != 'Y' || !is_null($fleet['sale_info']) || !is_null($fleet['sale']))
					return	"ERR#8";
			}
		}
		else
			$sFleets = array();

		// Merge fleets
		if	(count($sFleets) > 1)
		{
			$tmp = gameAction('mergeFleets', $sFleets, array($cPlayer), '');
			$fleet = $tmp[0];
		}
		elseif	(count($sFleets))
			$fleet = $sFleets[0];
		else
			$fleet = null;

		gameAction('newSale', $cPlayer, ($mode>1), ($mode == 3), $expires, $price, $tPid, $pid, $fleet);

		return	$this->getPlanetData($pid);
	}

	function abandon($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$info = gameAction('getPlanetById', $pid);
		$offer = gameAction('getPlanetSale', $pid);
		if	(!(is_null($info['abandon']) && is_null($info['bh_prep']) && is_null($offer)))
			return	"ERR#-1";
		$np = gameAction('getRealPlanetCount', $cPlayer);
		if	($np <= 1)
			return	"ERR#-1";

		gameAction('setPlanetAbandon', $pid);
		return	$this->getPlanetData($pid);
	}

	function cancelAbandon($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$info = gameAction('getPlanetById', $pid);
		if	(is_null($info['abandon']))
			return	"ERR#-1";

		gameAction('setPlanetAbandon', $pid, false);
		return	$this->getPlanetData($pid);
	}

	function cancelSale($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$offer = gameAction('getPlanetSale', $pid);
		if	(is_null($offer))
			return	"ERR#-1";

		if	(is_null($offer['finalized']))
			gameAction('cancelSale', $cPlayer, $offer['id']);
		elseif	(!gameAction('cancelTransfer', $cPlayer, $offer['id']))
			return	"ERR#18";
		return	$this->getPlanetData($pid);
	}

	function blowItUp($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if (!in_array($pid, $ids)) {
			return	"ERR#-1";
		}

		$info = gameAction('getPlanetById', $pid);
		$offer = gameAction('getPlanetSale', $pid);
		if (!(is_null($info['abandon']) && is_null($info['bh_prep']) && is_null($offer))) {
			return	"ERR#-1";
		}

		$rules = gameAction('loadPlayerRules', $cPlayer);
		$np = gameAction('getRealPlanetCount', $cPlayer);

		$q = dbQuery("SELECT bh_unhappiness FROM player WHERE id=$cPlayer");
		list($bhu) = dbFetchArray($q);
		$q = dbQuery("SELECT COUNT(*) FROM planet WHERE owner=$pid AND bh_prep IS NOT NULL");
		list($bhp) = dbFetchArray($q);

		$canBlow = ($np > 1) && ($rules['planet_destruction'] > 0) && ($bhu < 20) && ($bhp < 4);
		if ($canBlow && $this->game->params['victory'] == 2) {
			// Prevent planets from being destroyed in CTF games
			$ctfLib = $this->game->getLib('beta5/ctf');
			$canBlow = !($ctfLib->call('isTarget', $info['system']));
		}

		if (! $canBlow) {
			return "ERR#-1";
		}

		gameAction('setPlanetBoom', $pid);
		return	$this->getPlanetData($pid);
	}

	function cancelDestruction($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$info = gameAction('getPlanetById', $pid);
		if	(is_null($info['bh_prep']))
			return	"ERR#-1";

		gameAction('setPlanetBoom', $pid, false);
		return	$this->getPlanetData($pid);
	}

	function	getSellableFleets($pid)
	{
		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$fleets = array_keys(gameAction('getFleetsAt', $pid, $cPlayer));
		$rv = array();
		foreach	($fleets as $fid)
		{
			$f = gameAction('getFleet', $fid);
			if	($f['can_move'] != 'Y' || !is_null($fleet['sale_info']) || !is_null($fleet['sale']))
				continue;
			$s  = "$fid#" . $f['gaships'] . "#" . $f['fighters'] . "#" . $f['cruisers'] . "#" . $f['bcruisers'] . "#";
			$s .= gameAction('getFleetPower', $cPlayer, 0, $f['gaships'], $f['fighters'], $f['cruisers'], $f['bcruisers']) . "#";
			$s .= utf8entities($f['name']);
			array_push($rv, $s);
		}
		return	join("\n", $rv);
	}

	function destroyTurrets($pid, $turrets) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$ids = array_keys(gameAction('getPlayerPlanets', $cPlayer));
		if	(!in_array($pid, $ids))
			return	"ERR#-1";
		$turrets = (int)$turrets;
		if	($turrets <= 0)
			return	"ERR#19";
		$err = gameAction('destroyTurrets', $pid, $turrets);
		if	($err > 0)
			return	"ERR#" . ($err + 19);
		return	$this->getPlanetData($pid);
	}

	function	handle($input)
	{
		if (!preg_match('/^[0-9]+$/', $input['id'])) {
			logText("****** BUG? Planet not found: '{$input['id']}' (from {$_SERVER['HTTP_REFERER']})", LOG_DEBUG);
			$this->output = "planetnf";
			return;
		}
		$info = gameAction('getPlanetById', $input['id']);
		if	(is_null($info))
		{
			$this->output = "planetnf";
			return;
		}

		$this->data = array(
			'id'	=> $input['id'],
			'plist'	=> $this->getPlanetList(),
			'pdata'	=> $this->getPlanetData($input['id'])
		);
		$this->output = "planet";
	}
}

?>

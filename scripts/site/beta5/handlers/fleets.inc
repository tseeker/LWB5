<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			'init'	=> 'initFleets();',
			'func'	=> array(
					'getFleetsList','setFilter', 'renameFleets',
					'mergeFleets', 'switchStatus', 'manualSplit',
					'autoSplit', 'disbandFleets', 'setOrders',
					'getLocation', 'getMapData', 'moveMapToId',
					'moveMapToName', 'getTrajectory', 'getFleetTrajectory',
					'sellFleets', 'cancelSales', 'getSaleDetails'
				)
		);

	//--------------------------------------------------------------------------------------------
	// FLEETS LIST FUNCTIONS 

	function getOfflineAllies($player)
	{
		$list = gameAction('checkPlayerAllies', $player);
		array_push($list, $player);
		$res = array();
		foreach	($list as $pid)
		{
			$str = "$pid#" . ($pid == $player ? "1" : "0") . "#";
			$rules = gameAction('loadPlayerRules', $pid);
			$str .= $rules['fighter_space'] . "#" . $rules['gaship_space'] . "#";
			$str .= $rules['cruiser_haul'] . "#" . $rules['bcruiser_haul'] . "#";
			$str .= $rules['gaship_pop'] . "#" . ($pid == $player ? gameAction('isPlayerRestrained', $player) : 0);
			array_push($res, $str);
		}
		return	array($list, $res);
	}

	function	getLocations($plist, $dest)
	{
		$allLocations = array(); $allOwned = array(); $allyHasPlanets = array();

		foreach	($plist as $cPlayer)
		{
			$locations = gameAction('getFleetLocations', $cPlayer);
			foreach	($locations as $id)
				if	(!in_array($id, $allLocations))
					array_push($allLocations, $id);

			$ownPlanets = array_keys(gameAction('getPlayerPlanets', $cPlayer));
			foreach	($ownPlanets as $id)
			{
				if	(!in_array($id, $allLocations))
					array_push($allLocations, $id);
				if	(!in_array($id, $allOwned))
					array_push($allOwned, $id);
			}
			if (count($ownPlanets)) {
				$allyHasPlanets[$cPlayer] = true;
			}

			// FIXME: planets being probed should be accounted for as well
		}

		if	(!is_null($dest) && !in_array($dest['id'], $allLocations))
			array_push($allLocations, $dest['id']);

		return	array($allLocations, $allOwned, $allyHasPlanets);
	}

	function	getRawFleets($locations, $players)
	{
		$fleets = array();

		// Get all fleets at these locations
		foreach	($locations as $plId)
		{
			$locFleets = array_keys(gameAction('getFleetsAt', $plId));
			foreach	($locFleets as $id)
				$fleets[$id] = gameAction('getFleet', $id);
		}

		// Get moving fleets and fleets standing by in HS
		$targets = array(); $allyHasFleets = array();
		foreach	($players as $cPlayer)
		{
			$ownFleets = array_keys(gameAction('getPlayerFleets', $cPlayer));
			foreach	($ownFleets as $id)
			{
				if	(is_array($fleets[$id]))
					continue;

				$fleets[$id] = gameAction('getFleet', $id);
				if	(!is_null($fleets[$id]['move']))
				{
					$to = $fleets[$id]['move']['m_to'];
					$from = $fleets[$id]['move']['m_from'];
					$cur = $fleets[$id]['move']['cur'] = gameAction('getObjectLocation', $fleets[$id]['move']['id']);
				}
				else
				{
					$to = $fleets[$id]['wait']['drop_point'];
					$from = $fleets[$id]['wait']['origin'];
					$cur = null;
				}

				if	(!in_array($to, $locations))
					array_push($targets, $to);
				if	(!in_array($from, $locations))
					array_push($targets, $from);
				if	(!(is_null($cur) || in_array($cur, $locations)))
					array_push($targets, $cur);
			}
			if (count($ownFleets)) {
				$allyHasFleets[$cPlayer] = true;
			}
		}

		return	array($fleets, array_unique($targets), $allyHasFleets);
	}

	function	genFleetLines($cPlayer, $fleets, $allies)
	{
		$players = array(); $lines = array();
		foreach	($fleets as $id => $data)
		{
			if	(!in_array($data['owner'], $players))
				array_push($players, $data['owner']);

			if	(in_array($data['owner'], $allies))
			{
				$rs = "$id";
				if	($data['owner'] == $cPlayer)
					$fUpkeep = gameAction('getFleetUpkeep', $cPlayer, $data['gaships'], $data['fighters'], $data['cruisers'], $data['bcruisers']);
				else
					$fUpkeep = "";
			}
			else
			{
				$rs = "";
				$fUpkeep = "";
			}

			if	(is_null($data['location']))
				$mode = is_null($data['move']) ? 3 : 2;
			elseif	(is_null($data['sale_info']))
				$mode = ($data['attacking'] == 't' ? 1 : 0);
			else
				$mode = is_null($data['sale']) ? 4 : 5;

			$rs .= "#" . $data['owner'] . '#' . $data['gaships'] . "#" . $data['fighters'] . "#" . $data['cruisers'] . "#" . $data['bcruisers'] . "#"
				. ($fPower = gameAction('getFleetPower', $data['owner'], 0, $data['gaships'], $data['fighters'], $data['cruisers'], $data['bcruisers']))
				. "#$mode#$fUpkeep#" . utf8entities($data['name']);

			array_push($lines, $rs);

			// Orbiting fleets
			if	($mode < 2)
			{
				array_push($lines, $data['location'] . "#" . $data['can_move']);
				continue;
			}

			// Moving fleets
			if	($mode == 2)
			{
				$rs  = $data['move']['m_to'] . "#" . $data['move']['m_from'] . "#";
				$rs .= $data['move']['cur'] . "#" . (is_null($data['move']['wait_order']) ? ($data['attacking'] == 't' ? 1 : 0) : 2);
				$rs .= "#" . ($data['move']['time_left'] + $data['move']['changed']) . "#" . ($data['move']['hyperspace'] == 't' ? 1 : 0) . "#" . $data['move']['changed'];
				array_push($lines, $rs);

				if	(!is_null($data['move']['wait_order']))
				{
					$q = dbQuery("SELECT * FROM hs_wait WHERE id=".$data['move']['wait_order']);
					$data['wait'] = dbFetchHash($q);
					$data['wait']['origin'] = $data['move']['m_from'];
					$mode = 3;
				}
				else
					continue;
			}

			// Fleets standing by / Moving fleets with stand-by orders
			if	($mode == 3)
			{
				$rs  = $data['wait']['drop_point'] . "#" . $data['wait']['origin'] . "#";
				$rs .= $data['wait']['time_left'] . "#" . $data['wait']['time_spent'] . "#";

				if	(gameAction('waitCanDestroy', $data['wait']['drop_point'], $data['owner']))
				{
					$minLeft = $maxLeft = $fPower;
					for	($i=0;$i<$data['wait']['time_left'];$i++)
					{
						$t = $data['wait']['time_spent'] + $i;
						$prob = gameAction('waitGetLossProb', $t);
						if	($prob == 0)
							continue;
						$minLeft -= floor(5 * $minLeft * $prob / 10000);
						$maxLeft -= floor(round(10+$prob/10) * $maxLeft * $prob / 10000);
					}
					$minProb = round((($fPower - $minLeft) / $fPower) * 100);
					$maxProb = round((($fPower - $maxLeft) / $fPower) * 100);
					$rs .= "$minProb#$maxProb";
				}
				else
					$rs .= "0#0";

				$rs .= "#" . ($data['attacking'] == 't' ? 1 : 0);
				array_push($lines, $rs);
				continue;
			}

			// Fleets on sale
			if	($mode == 4)
			{
				$rs = $data['location'] . "#";
				if	($data['owner'] == $cPlayer)
					$rs .= $data['sale_info']['sale']['id'] . "#" . (is_null($data['sale_info']['sale']['planet']) ? 0 : 1);
				else
					$rs .= "#";
			}
			// Sold fleets waiting for owner change
			else
			{
				$rs = $data['location'] . "#";
				if	($data['owner'] == $cPlayer)
					$rs .= $data['sale_info']['sale']['id'] . "#" . $data['sale_info']['sale']['sold_to'] . "#"
						. $data['sale'] . "#" . (is_null($data['sale_info']['sale']['planet']) ? 0 : 1);
				else
					$rs .= "###";
			}
			array_push($lines, $rs);
		}

		return	array($lines, $players);
	}

	function	genPlanetLines($allPlanets, $allAllies, $locations)
	{
		$tags = array(); $result = array();
		foreach	($allPlanets as $id) {
			$pl = gameAction('getPlanetById', $id);
			array_push($result, "$id#" . $pl['status'] . "#" . $pl['x'] . "#" . $pl['y'] . "#" . ($pl['orbit'] + 1) . "#" . utf8entities($pl['name']));
			if	($pl['status'] != 0)
				continue;

			if (!is_null($pl['owner'])) {
				$owner = in_array($pl['owner'], $allAllies) ? $pl['owner'] : '';
				if (is_null($tags[$pl['owner']])) {
					$i = gameAction('getPlayerInfo', $pl['owner']);
					$tag = $tags[$pl['owner']] = utf8entities($i['alliance']);
				} else {
					$tag = $tags[$pl['owner']];
				}
			} else {
				$owner = $tag = "";
			}

			if (in_array($id, $locations)) {
				$turrets = $pl['turrets'];
				$tPower = gameAction('getFleetPower', $pl['owner'], $turrets, 0, 0, 0, 0);
				$pop = $pl['pop'];
				$vacation = ($pl['vacation'] == 'YES ') ? 1 : 0;
				$protection = (!is_null($pl['owner'])
					&& (gameAction('getProtectionLevel', $pl['owner']) > 0)) ? 1 : 0;
			} else {
				$turrets = $pop = $tPower = $vacation = $protection = "";
			}

			array_push($result, "$owner#$turrets#$tPower#$pop#$vacation#$protection#$tag");
		}
		return	$result;
	}

	function	getFleetsList()
	{
		// Get related data
		list($allAllies, $allyData) = $this->getOfflineAllies($_SESSION[game::sessName()]['player']);
		list($locations, $owned, $allyHasPlanets) = $this->getLocations($allAllies, $dest);
		list($fleets, $targets, $allyHasFleets) = $this->getRawFleets($locations, $allAllies);
		list($fleetLines, $players) = $this->genFleetLines($_SESSION[game::sessName()]['player'], $fleets, $allAllies);
		$planetLines = $this->genPlanetLines(array_merge($targets, $locations), $allAllies, $locations);
		$result = array();

		// nAllies # nFleets # nPlanets
		array_push($result, count($allAllies) . "#" . count($fleets) . "#" . (count($locations) + count($targets)));

		// listLocations # listMode # fDispMode # allyMode
		$lMode = $this->sessData('listMode'); $lLoc = $this->sessData('listLocations');
		$fMode = $this->sessData('fDispMode'); $aMode = $this->sessData('alliesMode');
		array_push($result, "$lLoc#$lMode#$fMode#$aMode");

		// sType # sText
		$sType = $this->sessData('sType'); $sText = $this->sessData('sText');
		array_push($result, "$sType#$sText");

		// Allies list
		$result = array_merge($result, $allyData);

		// Fleets list
		$result = array_merge($result, $fleetLines);

		// Planets list
		$result = array_merge($result, $planetLines);

		// Players
		foreach	($allAllies as $id) {
			if (!in_array($id, $players) && ($allyHasPlanets[$id] || $allyHasFleets[$id])) {
				array_push($players, $id);
			}
		}
		foreach	($players as $id)
			array_push($result, "$id#" . utf8entities(gameAction('getPlayerName', $id)));

		return	join("\n", $result);
	}

	//--------------------------------------------------------------------------------------------
	// FLEET ACTIONS

	function renameFleets($fleets, $name) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Check the new name
		$name = preg_replace('/\s+/', ' ', trim($name));
		if	($name == "")
			return	"ERR#0";
		elseif	(strlen($name) < 3)
			return	"ERR#1";
		elseif	(strlen($name) > 64)
			return	"ERR#2";

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Get all fleets
		$afl = array();
		foreach	($players as $pid)
			$afl = array_merge($afl, array_keys(gameAction('getPlayerFleets', $pid)));

		// Check fleet IDs
		$fids = explode('#', $fleets);
		foreach	($fids as $fid)
			if	(!in_array($fid, $afl))
				return	"ERR#3";

		foreach	($fids as $fid)
			gameAction('renameFleet', $fid, $name);

		return	$this->getFleetsList();
	}

	function mergeFleets($fleets, $name) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Check the new name
		$name = preg_replace('/\s+/', ' ', trim($name));
		if	(strlen($name) < 3 && $name != '')
			return	"ERR#0";
		elseif	(strlen($name) > 64)
			return	"ERR#1";

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Check fleet IDs
		$fids = explode('#', $fleets);
		$afl = array();
		foreach	($fids as $fid)
			array_push($afl, (int)$fid);

		// Do it
		gameAction('mergeFleets', array_unique($afl), $players, $name);

		return	$this->getFleetsList();
	}

	// Switch fleet status, making sure fleets at the same location are in the same
	// mode. NOTE, we assume that fleets at a given location for a player are already
	// in the same mode.
	function switchStatus($fleets) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Check fleet IDs
		$fids = explode('#', $fleets);
		$afl = array();
		foreach	($fids as $fid)
			array_push($afl, (int)$fid);

		// Check for problems and additional fleets to switch
		$inspected = array();
		$locations = array();
		foreach	($afl as $fid)
		{
			// Get fleet and check ownership
			$f = gameAction('getFleet', $fid);
			if	(!($f && in_array($f['owner'], $players)))
			{
				logText("beta5/fleets/switchStatus: fleet $fid not owned by any allied player (" . join(', ', $players) . ")", LOG_NOTICE);
				return	"ERR#0";
			}

			// Get owner's other fleets at the same location
			if	(!is_null($f['location']))
			{
				$location = $f['location'];
				$key = $f['owner'] . ":" . $location;
				if (!in_array($key, $inspected)) {
					if (!in_array($location, $locations))
						array_push($locations, $location);

					$fl = array_keys(gameAction('getFleetsAt', $location, $f['owner']));
					foreach	($fl as $nid)
						if	(!in_array($nid, $afl))
							array_push($afl, $nid);
					array_push($inspected, $key);
				}
			}
			elseif	(is_null($f['moving']))
				$location = $f['wait']['drop_point'];
			else
				$location = $f['move']['m_to'];

			// Make sure a player's not attacking himself
			$po = gameAction('getPlanetOwner', $location);
			if	($po == $f['owner'])
			{
				logText("beta5/fleets/switchStatus: fleet $fid can't be switched (planet owner == fleet owner for $location)", LOG_NOTICE);
				return	"ERR#1";
			}

			// If the player's switching to def, make sure he can
			if	(!is_null($po) && $po != $f['owner'] && !is_null($f['location']))
			{
				// Is the fleet owner an enemy of the planet owner?
				$isEnemy = gameAction('isPlayerEnemy', $po, $f['owner']);
				if	(!$isEnemy)
				{
					// Get fleet owner data
					$foi = gameAction('getPlayerInfo', $f['owner']);
					// Check for enemy alliance
					$isEnemy = (!is_null($foi['aid']) && gameAction('isAllianceEnemy', $po, $foi['aid']));
				}

				if	($isEnemy)
				{
					logText("beta5/fleets/switchStatus: fleet $fid can't be switched (planet owner has fleet owner on enemy list at $location)", LOG_NOTICE);
					return	"ERR#2";
				}
			}

			// If we are in a round game, and if the player is marked for destruction by the
			// Peacekeepers in that system, prevent him from switching
			if ($this->game->params['victory'] == 0 && !is_null($f['location'])) {
				if (is_null($this->protection)) {
					$this->protection = $this->game->getLib('beta5/prot');
				}
				if ($this->protection->call('isPlayerMarked', $f['owner'], $f['location'])) {
					return "ERR#3";
				}
			}
		}

		// Switch fleets status
		foreach	($afl as $fid)
			gameAction('switchFleetStatus', $fid);

		// Update the attacks cache
		foreach ($locations as $pid)
			gameAction('updateAttackStatus', $pid);

		return	$this->getFleetsList();
	}

	function manualSplit($fleet, $name, $amount, $sg, $sf, $sc, $sb) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Check the new name
		$name = preg_replace('/\s+/', ' ', trim($name));
		if	(strlen($name) < 3 && $name != '')
			return	"ERR#10";
		elseif	(strlen($name) > 60)
			return	"ERR#11";

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Get all fleets
		$f = gameAction('getFleet', (int)$fleet);
		if	(!($f && in_array($f['owner'], $players)))
			return	"ERR#0";

		// Check parameters
		$amount = (int)$amount;
		$sg = (int)$sg; $sf = (int)$sf; $sc = (int)$sc; $sb = (int)$sb;
		if	($amount <= 0 || $amout > 10 || $sg < 0 || $sf < 0 || $sc < 0 || $sb < 0 || $sg+$sf+$sc+$sb <= 0)
			return	"ERR#1";

		$ret = gameAction('splitFleetManually', $fleet, $name, $amount, $sg, $sf, $sc, $sb);
		if	($ret)
			return	"ERR#" . ($ret + 1);

		return	$this->getFleetsList();
	}

	function autoSplit($fleet, $name, $amount) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Check the new name
		$name = preg_replace('/\s+/', ' ', trim($name));
		if	(strlen($name) < 3 && $name != '')
			return	"ERR#10";
		elseif	(strlen($name) > 60)
			return	"ERR#11";

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Get all fleets
		$f = gameAction('getFleet', (int)$fleet);
		if	(!($f && in_array($f['owner'], $players)))
			return	"ERR#0";

		// Check parameters
		$amount = (int)$amount;
		if	($amount <= 0 || $amout > 10)
			return	"ERR#1";

		$ret = gameAction('splitFleetAuto', $fleet, $name, $amount + 1);
		if	($ret)
			return	"ERR#" . ($ret + 4);

		return	$this->getFleetsList();
	}

	function disbandFleets($fleets) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Get current player's fleets
		$afl = array_keys(gameAction('getPlayerFleets', $_SESSION[game::sessName()]['player']));

		// Check fleet IDs
		$fids = explode('#', $fleets);
		foreach	($fids as $fid)
			if	(!in_array($fid, $afl))
				return	"ERR#0";

		// Disband fleets
		foreach	($fids as $fid)
		{
			$e = gameAction('disbandFleet', $fid);
			if	($e != 0)
				return	"ERR#$e";
		}

		return	$this->getFleetsList();
	}

	function setOrders($fleets, $moveTo, $waitTime, $status) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Get all fleets
		$afl = array();
		foreach	($players as $pid)
			$afl = array_merge($afl, array_keys(gameAction('getPlayerFleets', $pid)));

		// Check fleet IDs
		$fids = explode('#', $fleets);
		foreach	($fids as $fid)
			if	(!in_array($fid, $afl))
				return	"ERR#0";

		// Check destination
		$moveTo = ($moveTo > 0 ? (int)$moveTo : null);
		if	(!is_null($moveTo))
		{
			$d = gameAction('getPlanetById', $moveTo);
			if	(is_null($d))
				return	"ERR#1";
		}

		// Set other parameters
		$waitTime = ($waitTime > 0 ? (int)$waitTime : null);
		$status = ($status == '1');

		logText("beta5/fleets/setOrders: move to '$moveTo', wait time '$waitTime', status '".($status?"Att":"Def")."', fleets: " . join(', ', $fids), LOG_DEBUG);

		// Checks the validity of the orders
		foreach	($fids as $fid)
		{
			$fleet = gameAction('getFleet', $fid);
			if	(is_null($fleet))		// Should never happen
			{
				logText("beta5/fleets/setOrders: fleet $fid has disappeared", LOG_WARNING);
				return	"ERR#2";
			}
			elseif	(!is_null($fleet['sale_info']))
			{
				logText("beta5/fleets/setOrders: fleet $fid is for sale", LOG_NOTICE);
				return	"ERR#3";
			}
			elseif	($fleet['can_move'] != 'Y')
			{
				logText("beta5/fleets/setOrders: fleet $fid is unavailable", LOG_NOTICE);
				return	"ERR#4";
			}
		}

		// Execute orders
		$fLib = $this->game->getLib('beta5/fleet');
		foreach	($fids as $fid) {
			$fLib->call('setOrders', $fid, $moveTo, $waitTime, $status);
		}
		$fLib->call('sendMoveMessages');

		return	$this->getFleetsList();
	}

	function sellFleets($data) {
		$cPlayer = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');

		if ($pLib->call('isOnVacation', $cPlayer)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $cPlayer)) {
			return "ERR#201";
		}

		if	(gameAction('isPlayerRestrained', $cPlayer) > 0)
			return	"ERR#-1";
		$pProt = gameAction('getProtectionLevel', $cPlayer);

		// Extract data from the received string
		$tmp = explode('*', $data);
		$sales = array();
		foreach	($tmp as $str)
		{
			$tmp2 = explode('#', $str);
			if	(count($tmp2) != 6)
				return	"ERR#-1";
			$fleets = array_unique(explode('!', array_pop($tmp2)));
			if	(!count($fleets))
				return	"ERR#-1";
			array_push($tmp2, $fleets);
			array_push($sales, $tmp2);
		}

		// Check each sale
		$remove = array();
		$expOk = array(0, 6, 12, 24, 48, 72, 96, 120);
		for	($i=0;$i<count($sales);$i++)
		{
			$sales[$i][0] = (int)$sales[$i][0]; $sales[$i][1] = (int)$sales[$i][1];
			$sales[$i][2] = (int)$sales[$i][2]; $sales[$i][3] = (int)$sales[$i][3];

			// Check mode
			if	($sales[$i][1] < 0 || $sales[$i][1] > 3)
				return	"ERR#-1";

			// Check target player
			if	($sales[$i][1] < 2)
			{
				$p = gameAction('getPlayer', $sales[$i][4]);
				if	(is_null($p))
					return	"ERR#0#".$sales[$i][0];
				else if	($p == $cPlayer)
					return	"ERR#6#".$sales[$i][0];
				$pl = gameAction('getProtectionLevel', $p);
				if	($pl != $pProt)
					return	"ERR#1#".$sales[$i][0];
				if	(gameAction('isPlayerRestrained', $p) > 0)
					return	"ERR#7#".$sales[$i][0];
				$sales[$i][4] = $p;
			}
			else
				$sales[$i][4] = null;

			// Check price
			if	($sales[$i][1] != 0 && $sales[$i][3] <= 0)
				return	"ERR#2#".$sales[$i][0];

			// Check expiration
			if	(!in_array($sales[$i][2],$expOk))
				return	"ERR#-1";
			elseif	($mode == 3 && $sales[$i][2] == 0)
				return	"ERR#3#".$sales[$i][0];

			// Check fleets
			foreach	($sales[$i][5] as $fid)
			{
				$f = gameAction('getFleet', (int)$fid);
				if	(is_null($f) || $f['owner'] != $cPlayer || $f['attacking'] == 't'
						|| is_null($f['location']) || $f['location'] != $sales[$i][0]
						|| !is_null($f['sale']) || !is_null($f['sale_info']))
				{
					if	(!is_array($remove[$sales[$i][0]]))
						$remove[$sales[$i][0]] = array();
					array_push($remove[$sales[$i][0]], $fid);
				}
			}
		}

		// If some fleets have to be removed from the list, notify the page
		if	(count($remove))
		{
			$rv = array('ERR', '4');
			foreach	($remove as $sid => $fl)
			{
				array_push($rv, $sid);
				array_push($rv, join('!', $fl));
			}
			return	join('#', $rv);
		}

		// Generate sales
		foreach	($sales as $sale)
		{
			// Merge fleets
			if	(count($sale[5]) > 1)
			{
				$fidl = gameAction('mergeFleets', $sale[5], array($cPlayer), '');
				$fid = $fidl[0];
			}
			else
				$fid = $sale[5][0];
			gameAction('newSale', $cPlayer, ($sale[1]>1), ($sale[1]==3), $sale[2], $sale[3], $sale[4], null, $fid);
		}

		return	$this->getFleetsList();
	}

	function cancelSales($list) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$cPlayer = $_SESSION[game::sessName()]['player'];
		$pinf = gameAction('getPlayerInfo', $cPlayer);

		$offers = array();
		$ids = explode("#", $list);
		$sum = 0;
		foreach	($ids as $sid)
		{
			$offer = gameAction('getFleetSale', (int)$sid);
			if	(is_null($offer) || $offer['player'] != $cPlayer)
				return	"ERR#-1";
			array_push($offers, $offer);
			if	(!is_null($offer['finalized']))
				$sum += $offer['price'];
		}

		if	($sum > $pinf['cash'])
			return	"ERR#0";

		foreach	($offers as $offer)
			if	(is_null($offer['finalized']))
				gameAction('cancelSale', $cPlayer, $offer['id']);
			else	
				gameAction('cancelTransfer', $cPlayer, $offer['id']);

		return	$this->getFleetsList();
	}

	function getSaleDetails($sId) {
		$cPlayer = $_SESSION[game::sessName()]['player'];

		$offer = gameAction('getFleetSale', (int)$sId);
		if	(is_null($offer) || $offer['player'] != $cPlayer)
			return	"";

		$rs = "$sId#{$offer['fleet']}#{$offer['expires']}#{$offer['finalized']}#{$offer['sold_to']}#{$offer['tx_time']}#";
		if	($offer['public'])
		{
			if	($offer['is_auction'])
				$rs .= "2#{$offer['price']}#{$offer['max_bid']}#{$offer['bidder']}#{$offer['last_bid']}";
			else
				$rs .= "1#{$offer['price']}";
		}
		else
			$rs .= "0#{$offer['price']}";

		if	(!is_null($offer['sold_to']))
			$rs .= "\n" . gameAction('getPlayerName', $offer['sold_to']);
		elseif	(!is_null($offer['bidder']))
			$rs .= "\n" . gameAction('getPlayerName', $offer['bidder']);

		return	$rs;
	}

	//--------------------------------------------------------------------------------------------
	// HANDLER AND HELPER FUNCTIONS

	function setFilter($name, $value) {
		$okv = array('listMode','listLocations','fDispMode','alliesMode','sType','sText');
		if	(!in_array($name, $okv))
			return;
		$v = &$this->sessData($name);
		$v = $value;
	}

	function	getLocation($id)
	{
		$id = (int)$id;
		$p = gameAction('getPlanetById', $id);
		if	(is_null($p))
			return "";
		return	join("\n", $this->genPlanetLines(array($id),array(),array()));
	}

	function getMapData($sx, $sy) {
		$sx = (int)$sx; $sy = (int)$sy;
		$sys = gameAction('getSystemAt', $sx, $sy);
		if (is_null($sys)) {
			return	"$sx#$sy#-1";
		}

		switch ($this->game->params['victory']) {
		    case 0:
			$sys['prot'] = ($sys['prot'] > 0) ? 1 : 0;
			break;
		    case 1:
			$sys['prot'] = 0;
			break;
		    case 2:
			$sys['prot'] = input::$game->getLib('beta5/ctf')->call('isTarget', $sys['id']) ? 1 : 0;
			break;
		}

		$rv = array("$sx#$sy#{$sys['nebula']}#{$sys['prot']}");
		$plist = gameAction('getSystemPlanets', $sys['id']);

		$player = $_SESSION[game::sessName()]['player'];
		$pinf = gameAction('getPlayerInfo', $player);

		foreach	($plist as $p)
		{
			$str = $p['id'] . "#" . $p['status'] . "#";
			if	($p['status'] == 0)
			{
				if	($p['owner'] == $player)
					$str .= "2";
				elseif	(!is_null($pinf['alliance']) && $p['tag'] == $pinf['alliance'])
					$str .= "1";
				else
					$str .= "0";
			}
			else
				$str .= '0';
			$str .= "#" . utf8entities($p['name']);
			array_push($rv, $str);
		}
		return	join("\n", $rv);
	}

	function	moveMapToId($id)
	{
		$id = (int)$id;
		$p = gameAction('getPlanetById', $id);
		if	(is_null($p))
			return	"ERR";
		return	$this->getMapData($p['x'], $p['y']);
	}

	function	moveMapToName($name)
	{
		$p = gameAction('getPlanetByName', $name);
		if	(is_null($p))
			return	"ERR";
		return	$this->getMapData($p['x'], $p['y']);
	}

	function	getTrajectory($key)
	{
		list($from, $hc, $owner, $to) = explode(';', $key);

		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);
		if	(!in_array($owner, $players))
			return	"";

		// Test destination and origin
		$p = gameAction('getPlanetById', $from);
		if	(is_null($p))
			return	"";
		if	($from == $to)
			return	$key;
		$p = gameAction('getPlanetById', $to);
		if	(is_null($p))
			return	"";
		$hc = ($hc == "1");

		// Get player capabilities
		$rules = gameAction('loadPlayerRules', $owner);

		// Get trajectory
		$t = gameAction('getTrajectory', $from, $to, $rules['capital_ship_speed'], $hc);
		if	(!$t)
			return	"";

		// Generate return string
		$rv = array($key);
		foreach	($t['list'] as $wp)
		{
			$p = gameAction('getPlanetById', $wp['pid']);
			array_push($rv, $p['x'] . "#" . $p['y'] . "#" . ($p['orbit'] + 1) . "#" . $wp['time'] . "#" . utf8entities($p['name']));
		}

		return	join("\n", $rv);
	}

	function	getFleetTrajectory($fid)
	{
		// Get players
		$player = $_SESSION[game::sessName()]['player'];
		$players = gameAction('checkPlayerAllies', $player);
		array_push($players, $player);

		// Get all fleets
		$f = gameAction('getFleet', (int)$fid);
		if	(!($f && in_array($f['owner'], $players)))
			return	"ERR#0";
		elseif	(is_null($f['move']))
			return	"ERR#1";

		// Start generating output
		$s = "$fid#".$f['move']['m_from']."#".$f['move']['m_to']."#".$f['move']['changed']."#"
			.$f['move']['time_left']."#".($f['move']['hyperspace'] == 't' ? 1 : 0)."#".(is_null($f['wait'])?"0":$f['wait']['time_left']);
		$rv = array($s);

		// Get trajectory
		$t = gameAction('getObjectTrajectory', $f['moving']);
		foreach	($t as $wp)
		{
			$s = $wp['id'] . "#" . $wp['eta'] . "#" . $wp['x'] . "," . $wp['y'] . "#" . $wp['orbit'] . "#" . $wp['opacity']
				. "#" . utf8entities($wp['name']);
			array_push($rv, $s);
		}

		return	join("\n", $rv);
	}

	function	&sessData($name)
	{
		if	(!is_array($_SESSION[game::sessName()]['fleets']))
			$_SESSION[game::sessName()]['fleets'] = array(
				'listMode'	=> 0,
				'listLocations'	=> 0,
				'fDispMode'	=> 0,
				'alliesMode'	=> 0,
				'sType'		=> 0,
				'sText'		=> ''
			);
		return	$_SESSION[game::sessName()]['fleets'][$name];
	}

	function	handle($input)
	{
		// FIXME: should check for request argument ("Send Fleets" planet command)
		if	($input['sto'] != '')
			$dest = gameAction('getPlanetById', (int)$input['sto']);
		else
			$dest = null;
		$prefix = is_null($dest) ? '-' : $dest['id'];
		$this->data = "$prefix " . $this->getFleetsList();
		$this->output = "fleets";
	}
}

?>

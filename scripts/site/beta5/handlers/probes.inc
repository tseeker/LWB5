<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$bcnCost = 50000;
	var	$bcnPow = 3;
	var	$ajax = array(
			"func"	=> array(
					'getPageData',
//					'setEmpirePolicy', 'togglePlanetPolicy', 'setPlanetPolicy',
					'upgradeBeacon'
				),
			"init"	=> "initPage();"
		);


	//------------------------------------------------------------------------------
	// Probes policy control

	function	setEmpirePolicy($element, $value)
	{
		$element = (int)$element; $value = (int)$value;
		if	($element < 0 || $value < 0 || $element > 3 || $value > 2)
			return	"ERR#1";

		// Get empire policy
		$player = $_SESSION[game::sessName()]['player'];
		$ePolicy = gameAction('getPlayerPolicy', $player);
		if	(is_null($ePolicy))
			return	"ERR#0";

		// Set new value
		$ePolicy{$element} = $value;
		gameAction('setPlayerPolicy', $player, $ePolicy);

		return	"1\n$ePolicy";
	}

	function	togglePlanetPolicy($planet)
	{
		// Get planet information
		$player = $_SESSION[game::sessName()]['player'];
		$planet = (int)$planet;
		$pinf = gameAction('getPlanetById', $planet);
		if	(is_null($pinf))
			return	"ERR#1";
		elseif	($pinf['owner'] != $player)
			return	"ERR#2";

		// Get planet policy
		$pPolicy = gameAction('getPlanetPolicy', $planet);
		if	(is_null($pPolicy))
		{
			$ePolicy = gameAction('getPlayerPolicy', $player);
			if	(is_null($ePolicy))
				return	"ERR#0";
			$pPolicy = $ePolicy;
		}
		else
			$pPolicy = null;
		gameAction('setPlanetPolicy', $planet, $pPolicy);

		return	"2\n$planet#$pPolicy";
	}

	function	setPlanetPolicy($planet, $element, $value)
	{
		// Check parameters
		$element = (int)$element; $value = (int)$value;
		if	($element < 0 || $value < 0 || $element > 3 || $value > 2)
			return	"ERR#1";

		// Get planet information
		$player = $_SESSION[game::sessName()]['player'];
		$planet = (int)$planet;
		$pinf = gameAction('getPlanetById', $planet);
		if	(is_null($pinf))
			return	"ERR#1";
		elseif	($pinf['owner'] != $player)
			return	"ERR#2";

		// Get planet policy
		$pPolicy = gameAction('getPlanetPolicy', $planet);
		if	(is_null($pPolicy))
			return	"ERR#1";
		$pPolicy{$element} = $value;
		gameAction('setPlanetPolicy', $planet, $pPolicy);

		return	"2\n$planet#$pPolicy";
	}


	//------------------------------------------------------------------------------
	// Beacon management functions

	function	getPlanetBeaconData($pid, $maxTech)
	{
		// Get the planet's data
		$pinf = gameAction('getPlanetById', $pid);
		$pinf['orbit'] ++;
		$mainData = "$pid#{$pinf['x']}#{$pinf['y']}#{$pinf['orbit']}#{$pinf['beacon']}#";

		// Compute the price to upgrade if that applies
		if	($pinf['beacon'] < $maxTech)
			$price = $this->bcnCost * pow($this->bcnPow, $pinf['beacon']);
		else
			$price = "";
		$mainData .= "$price#" . ($pinf['built_probe'] == 't' ? 1 : 0) . "#";

		// If the beacon can spot fleets standing by in Hyperspace, check for them
		$reqLevel = $this->game->params['fakebeacons'] ? 1 : 2;
		$hsFleets = array();
		if ($pinf['beacon'] >= $reqLevel) {
			$hsFleetsRaw = $this->game->getLib('beta5/planet')->call('getDetectedFleets', $pid);
			foreach ($hsFleetsRaw as $fleet) {
				$str = "{$fleet['i_level']}#{$fleet['fl_size']}#{$fleet['fl_owner']}#{$fleet['owner']}";
				array_push($hsFleets, $str);
			}
		}

		// Generate the return value
		$mainData .= count($hsFleets) . "#" . count($mvFleets) . "#" . utf8entities($pinf['name']);
		$rv = array($mainData);
		if (count($hsFleets)) {
			$rv = array_merge($rv, $hsFleets);
		}

		return	$rv;
	}

	function upgradeBeacon($pid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$player = $_SESSION[game::sessName()]['player'];
		$pid = (int)$pid;

		// Check the planet's existance, ownership and beacon/probe status
		$pinf = gameAction('getPlanetById', $pid);
		if	(is_null($pinf) || $pinf['built_probe'] == 't')
			return	"ERR#0";
		else if	($pinf['owner'] != $player)
			return	"ERR#1";

		// Check the player's tech level
		$rules = gameAction('loadPlayerRules', $player);
		if	($pinf['beacon'] == $rules['hs_beacon_level'])
			return	"ERR#0";

		// Check the player's cash
		$plinf = gameAction('getPlayerInfo', $player);
		$price = $this->bcnCost * pow($this->bcnPow, $pinf['beacon']);
		if	($plinf['cash'] < $price)
			return	"ERR#2";

		// Upgrade the beacon
		dbQuery("UPDATE planet SET beacon=beacon+1,built_probe=".dbBool(1)." WHERE id=$pid");
		dbQuery("UPDATE player SET cash=cash-$price WHERE id=$player");

		// Generate the return value
		$rv = $this->getPlanetBeaconData($pid, $rules['hs_beacon_level']);
		array_unshift($rv, 1);
		return	join("\n", $rv);
	}


	//------------------------------------------------------------------------------
	// Complete data generation for sub-pages

	function	getPageData($name)
	{
		$page = $this->setPage($name);
		switch	($page)
		{
			//case "policy": return $this->getPolicyData();
			case "beacons": return $this->getBeaconsData();
			//case "build": return $this->getBuildData();
			//case "data": return $this->getProbesData();
		}
	}

	function	getPolicyData()
	{
		$player = $_SESSION[game::sessName()]['player'];

		// Get empire policy
		$ePolicy = gameAction('getPlayerPolicy', $player);
		if	(is_null($ePolicy))
			return	"ERR#0";

		// Get the player's planet-specific policies
		$pPolicies = gameAction('getPlayerPlanets', $player);
		foreach	($pPolicies as $id => $junk)
		{
			$pinf = gameAction('getPlanetById', $id);
			$pPolicies[$id] = array(
				$pinf['x'], $pinf['y'], $pinf['orbit'] + 1,
				gameAction('getPlanetPolicy', $id),
				utf8entities($pinf['name'])
			);
		}

		// Generate output
		$rv = array(0, count($pPolicies) . "#" . $ePolicy);
		if	(count($pPolicies))
		{
			foreach	($pPolicies as $id => $pPolicy)
				array_push($rv, "$id#" . join("#", $pPolicy));
		}

		return	join("\n", $rv);
	}

	function	getBeaconsData()
	{
		$player = $_SESSION[game::sessName()]['player'];

		// Get the player's beacon technology level
		$rules = gameAction('loadPlayerRules', $player);
		$beaconTech = $rules['hs_beacon_level'];

		// Get the data regarding all of the player's planets
		$pl = gameAction('getPlayerPlanets', $player);
		$rv = array(0, "$beaconTech#" . count($pl));
		foreach	($pl as $id => $name) {
			$rv = array_merge($rv, $this->getPlanetBeaconData($id, $beaconTech));
		}

		return	join("\n", $rv);
	}

	//------------------------------------------------------------------------------
	// Sub-pages and session

	function	setPage($name)
	{
		//$okPages = array('policy', 'beacons', 'build', 'data');
		$okPages = array('beacons');
		if	(!in_array($name, $okPages))
			$name = $okPages[0];
		return	($_SESSION[game::sessName()]['probe_page'] = $name);
	}

	function	getPage()
	{
		if	(is_null($_SESSION[game::sessName()]['probe_page']))
			$_SESSION[game::sessName()]['probe_page'] = 'beacons';
		return	$_SESSION[game::sessName()]['probe_page'];
	}

	function	handle($input)
	{
		$this->data = $this->getPage();
		$this->output = "probes";
	}
}

?>

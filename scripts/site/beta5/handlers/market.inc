<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			'func'	=> array(
					'getPublicOffers', 'moveMap', 'cancelSale', 'placeBid',
					'buyPublic', 'getPrivateOffers', 'buyPrivate', 'declinePrivate',
					'getSentOffers'
				),
			'init'	=> 'initMarket();'
		);

	//--------------------
	// Public offers

	function	findName($n)
	{
		$n = trim($n);
		$a = gameAction('getPlanetByName', $n);
		if	(is_null($a))
			return	null;
		$o = ($a['owner'] == $_SESSION[game::sessName()]['player']) ? 0 : 1;
		$i = $o ? $a['name'] : $a['id'];
		return	array($o + 1, $i, $a['x'], $a['y']);
	}

	function	getPlayerPlanets()
	{
		$pl = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		$as = array(count($pl));
		foreach	($pl as $id => $name)
			array_push($as, "$id#$name");
		return	$as;
	}

	function getPublicOffers() {
		$page = &$this->sessData('page'); $page = 0;
		$player = $_SESSION[game::sessName()]['player'];
		$pinfo = gameAction('getPlayerInfo', $player);
		$result = array();

		// Get map configuration
		$map = &$this->sessData('po_map');
		if (!is_array($map)) {
			$pname = gameAction('getFirstPlanet', $player);
			list($type,$param,$x,$y) = $this->findName($pname);
			$map = array(
				'type'	=> $type,
				'x'	=> $x,
				'y'	=> $y,
				'param'	=> $param,
				'dist'	=> 1
			);
		} elseif ($map['type'] == 1) {
			$pi = gameAction('getPlanetOwner', $map['param']);
			if ($pi['owner'] != $player) {
				$pi['type'] = 2;
				$pi['param'] = $pi['name'];
			}
		}

		// Add map data to the output array
		array_push($result, $map['type']."#".$map['x']."#".$map['y']."#".$map['dist'].($map['type'] == 0 ? '' : ('#'.utf8entities($map['param']))));
		$result = array_merge($result, $this->getPlayerPlanets());

		// Get selected system data
		$sys = gameAction('getSystemAt', $map['x'], $map['y']);
		if (is_null($sys)) {
			array_push($result, "-1#0");
		} else {
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

			$pLev = gameAction('getProtectionLevel', $player);
			array_push($result, $sys['nebula']."#".$sys['prot']);
			$plist = gameAction('getSystemPlanets', $sys['id']);
			foreach	($plist as $pi) {
				$s  = $pi['id']."#".$pi['status']."#".($pi['owner'] == $player ? '1' : '0') . '#';
				$s .= ($pinfo['aid'] && $pi['owner'] != $player && $pinfo['alliance'] == $pi['tag']) ? '1' : '0';
				array_push($result, $s . "#" . utf8entities($pi['name']));
			}
		}

		// Look for sales
		$pIds = gameAction('getPlanetsAround', $map['x'], $map['y'], $map['dist']);
		if	(count($pIds))
		{
			$sales = gameAction('getSales', $pIds);
			$pSales = $fSales = array();
			for	($i=0;$i<count($sales);$i++)
				if	(is_null($sales[$i]['planet']))
					array_push($fSales, $i);
				else
					array_push($pSales, $i);
			array_push($result, count($pSales)."#".count($fSales));
			$sellers = array();

			// Planet sales, with or without fleets
			for	($i=0;$i<count($pSales);$i++)
			{
				$o = $sales[$pSales[$i]];
				$p = $o['planet'];
				if	(!in_array($o['player'],$sellers))
					array_push($sellers, $o['player']);

				$s  = $o['id']."#".$o['expires']."#".$p['id']."#".$o['player']."#".$p['x']."#".$p['y']."#".($p['orbit']+1)."#".$o['price']."#";
				$s .= ($o['auction']=='t'?1:0)."#".$p['pop']."#".$p['turrets']."#".$p['fact']."#".(is_null($o['fleet'])?0:1)."#".utf8entities($p['name']);
				array_push($result, $s);
				if	(is_null($o['fleet']))
					continue;

				$f = $o['fleet'];
				array_push($result, $f['id']."#".$f['sg']."#".$f['sf']."#".$f['sc']."#".$f['sb']);
			}

			// Fleet sales
			for	($i=0;$i<count($fSales);$i++)
			{
				$o = $sales[$fSales[$i]];
				$f = $o['fleet'];
				if	(!in_array($o['player'],$sellers))
					array_push($sellers, $o['player']);

				$s  = $o['id']."#".$o['expires']."#".$f['id']."#".$o['player']."#".$f['x']."#".$f['y']."#".($f['orbit']+1)."#".$f['pid']."#".$o['price']."#";
				$s .= ($o['auction']=='t'?1:0)."#".$f['sg']."#".$f['sf']."#".$f['sc']."#".$f['sb']."#".utf8entities($f['pname']);
				array_push($result, $s);
			}

			// Current owners
			foreach	($sellers as $sid)
			{
				$pinf = gameAction('getPlayerInfo', $sid, true);
				array_push($result, "$sid#" . ($player==$sid?1:0) . "#" . $pinf['quit'] . "#" . utf8entities($pinf['name']));
			}
		}
		else
			// No sales
			array_push($result, "0#0");

		return	join("\n", $result);
	}

	function moveMap($type,$parm,$dist) {
		$map = &$this->sessData('po_map');

		$type = (int)$type;
		if ($type < 0 || $type > 2) {
			$type = 0;
		}
		$dist = (int)$dist;
		if ($dist < 1 || $dist > 7) {
			$dist = 1;
		}
		$map['dist'] = $dist;

		switch	($type) :
		case 0:
			list($x,$y) = split('#',$parm);
			$map['x'] = (int)$x;
			$map['y'] = (int)$y;
			$map['type'] = 0;
			break;
		case 1:
			$a = gameAction('getPlanetById', (int)$parm);
			if (!is_null($a)) {
				$map['x'] = $a['x'];
				$map['y'] = $a['y'];
				$map['param'] = (int)$parm;
				$map['type'] = 1;
			}
			break;
		case 2:
			$a = $this->findName($parm);
			if (!is_null($a)) {
				list($map['type'],$map['param'],$map['x'],$map['y']) = $a;
			}
			break;
		endswitch;

		return	$this->getPublicOffers();
	}

	function cancelSale($id) {
		$player = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');

		if ($pLib->call('isOnVacation', $player)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $player)) {
			return "ERR#201";
		}

		gameAction('cancelSale', $_SESSION[game::sessName()]['player'], (int)$id);
		return	$this->getMarketData();
	}

	function placeBid($id, $bid) {
		$player = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');

		if ($pLib->call('isOnVacation', $player)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $player)) {
			return "ERR#201";
		}

		$r = gameAction('placeBid', $_SESSION[game::sessName()]['player'], (int)$id, (int)$bid);
		if	($r != '')
			return	"ERR#$r";
		return	$this->getPublicOffers();
	}

	function buyPublic($offer) {
		$player = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');

		if ($pLib->call('isOnVacation', $player)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $player)) {
			return "ERR#201";
		}

		$r = gameAction('buy', $_SESSION[game::sessName()]['player'], (int)$offer);
		if	($r != '')
			return	"ERR#$r";
		return	$this->getPublicOffers();
	}


	//--------------------
	// Private offers

	function getPrivateOffers() {
		$page = &$this->sessData('page'); $page = 1;
		$player = $_SESSION[game::sessName()]['player'];
		$pinfo = gameAction('getPlayerInfo', $player);
		$result = array();

		// Get offers and history
		$offers = gameAction('getDirectSales', $player);
		$history = gameAction('getSalesHistoryTo', $player);
		$sellers = array();
		array_push($result, count($offers) . "#" . count($history));

		// Generate offers list
		foreach	($offers as $o)
		{
			$f = $o['fleet'];
			$p = $o['planet'];
			if	(!in_array($o['player'], $sellers))
				array_push($sellers, $o['player']);

			$cs = is_null($p) ? $f : $p;
			$rs = array($o['id'], $o['player'], $o['price'], $cs['x'], $cs['y'], $cs['orbit']+1);
			array_push($rs, is_null($p) ? '' : $p['id']);
			array_push($rs, is_null($f) ? '' : $f['id']);
			array_push($rs, $o['started']); array_push($rs, $o['expires']);
			array_push($rs, utf8entities(is_null($p) ? $f['pname'] : $p['name']));
			array_push($result, join('#', $rs));

			if	(!is_null($f))
			{
				$rs = array($f['sg'], $f['sf'], $f['sc'], $f['sb']);
				array_push($result, join('#', $rs));
			}

			if	(!is_null($p))
			{
				$rs = array($p['pop'], $p['turrets'], $p['fact']);
				array_push($result, join('#', $rs));
			}
		}

		// Generate history list
		foreach	($history as $o)
		{
			if	(!in_array($o['from_player'], $sellers))
				array_push($sellers, $o['from_player']);
			$price = is_null($o['sell_price']) ? $o['price'] : $o['sell_price'];
			$hf = ($o['is_planet'] == 'f') || ($o['f_gaships'] + $o['f_fighters'] + $o['f_cruisers'] + $o['f_bcruisers'] != 0);
			$pinf = gameAction('getPlanetById', $o['p_id']);
			$rs = array(
				$o['from_player'], $price, $pinf['x'], $pinf['y'], $pinf['orbit']+1,
				($o['is_planet'] == 't' ? 1 : 0), $hf?1:0,
				$o['started'], $o['end_mode'], $o['ended'], utf8entities($o['p_name'])
			);
			array_push($result, join("#", $rs));

			if	($hf)
				array_push($result, $o['f_gaships']."#".$o['f_fighters']."#".$o['f_cruisers']."#".$o['f_bcruisers']);
			if	($o['is_planet'] == 't')
				array_push($result, $o['p_pop']."#".$o['p_turrets']."#".$o['p_factories']);
		}

		// Player names and status
		foreach	($sellers as $sid)
		{
			$pinf = gameAction('getPlayerInfo', $sid, true);
			array_push($result, "$sid#" . $pinf['quit'] . "#" . utf8entities($pinf['name']));
		}

		return	join("\n", $result);
	}

	function buyPrivate($offer) {
		$player = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');

		if ($pLib->call('isOnVacation', $player)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $player)) {
			return "ERR#201";
		}

		$r = gameAction('buy', $_SESSION[game::sessName()]['player'], (int)$offer);
		if	($r != '')
			return	"ERR#$r";
		return	$this->getPrivateOffers();
	}

	function declinePrivate($offer) {
		$player = $_SESSION[game::sessName()]['player'];
		$pLib = $this->game->getLib('beta5/player');

		if ($pLib->call('isOnVacation', $player)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $player)) {
			return "ERR#201";
		}

		if (!gameAction('isDirectOffer', $_SESSION[game::sessName()]['player'], (int)$offer)) {
			return	"ERR#0";
		}
		gameAction('declinePrivate', (int)$offer);
		return	$this->getPrivateOffers();
	}


	//--------------------
	// Sent offers

	function getSentOffers() {
		$page = &$this->sessData('page'); $page = 2;
		$player = $_SESSION[game::sessName()]['player'];
		$pinfo = gameAction('getPlayerInfo', $player);
		$result = array();

		// Get offers and history
		$offers = gameAction('getSentOffers', $player);
		$history = gameAction('getSalesHistoryFrom', $player);
		$buyers = array();
		array_push($result, count($offers) . "#" . count($history));

		// Generate offers list
		foreach	($offers as $o) {
			$f = $o['fleet'];
			$p = $o['planet'];
			if	($o['to_player'] != '' && !in_array($o['to_player'], $buyers))
				array_push($buyers, $o['to_player']);

			$cs = is_null($p) ? $f : $p;
			$rs = array($o['id'], $o['mode'], $o['price'], $o['to_player'], $cs['x'], $cs['y'], $cs['orbit'] + 1, is_null($p) ? 0 : 1, is_null($f) ? 0 : 1);
			array_push($rs, $o['started']); array_push($rs, $o['expires']);
			array_push($rs, utf8entities(is_null($p) ? $f['pname'] : $p['name']));
			array_push($result, join('#', $rs));

			if (!is_null($f)) {
				$rs = array($f['sg'], $f['sf'], $f['sc'], $f['sb']);
				array_push($result, join('#', $rs));
			}

			if (!is_null($p)) {
				$rs = array($p['pop'], $p['turrets'], $p['fact']);
				array_push($result, join('#', $rs));
			}
		}

		// Generate history list
		foreach	($history as $o) {
			if	($o['to_player'] != '' && !in_array($o['to_player'], $buyers))
				array_push($buyers, $o['to_player']);
			$price = is_null($o['sell_price']) ? $o['price'] : $o['sell_price'];
			$hf = ($o['is_planet'] == 'f') || ($o['f_gaships'] + $o['f_fighters'] + $o['f_cruisers'] + $o['f_bcruisers'] != 0);
			$pinf = gameAction('getPlanetById', $o['p_id']);
			$rs = array(
				$pinf['x'], $pinf['y'], $pinf['orbit'] + 1, $o['mode'], ($o['is_planet'] == 't' ? 1 : 0),
				$hf?1:0, $o['to_player'], $price, 
				$o['started'], $o['end_mode'], $o['ended'], utf8entities($o['p_name'])
			);
			array_push($result, join("#", $rs));

			if	($hf)
				array_push($result, $o['f_gaships']."#".$o['f_fighters']."#".$o['f_cruisers']."#".$o['f_bcruisers']);
			if	($o['is_planet'] == 't')
				array_push($result, $o['p_pop']."#".$o['p_turrets']."#".$o['p_factories']);
		}

		// Player names and status
		foreach	($buyers as $sid) {
			$pinf = gameAction('getPlayerInfo', $sid, true);
			array_push($result, "$sid#" . $pinf['quit'] . "#" . utf8entities($pinf['name']));
		}

		return	join("\n", $result);
	}


	//--------------------
	// Standard output

	function	&sessData($name)
	{
		if	(!is_array($_SESSION[game::sessName()]['market']))
			$_SESSION[game::sessName()]['market'] = array('page' => 0);
		if	(!isset($_SESSION[game::sessName()]['market'][$name]))
			$_SESSION[game::sessName()]['market'][$name] = null;
		return	$_SESSION[game::sessName()]['market'][$name];
	}

	function	getMarketData()
	{
		switch	($this->sessData('page')) :
			case 0: $s = $this->getPublicOffers(); break;
			case 1: $s = $this->getPrivateOffers(); break;
			case 2: $s = $this->getSentOffers(); break;
			default: $s = ""; break;
		endswitch;
		return	$s;
	}

	function	handle($input)
	{
		$page = &$this->sessData('page');
		switch	($input['p']) :
			case 'p': $page = 0; break;
			case 'r': $page = 1; break;
			case 's': $page = 2; break;
		endswitch;

		$r = gameAction('isPlayerRestrained', $_SESSION[game::sessName()]['player']);
		if	($r > 0)
			$this->data = $r;
		else
			$this->data = array(
				"page"		=> $page,
				"pdata"		=> $this->getMarketData()
			);
		$this->output = "market";
	}
}

?>

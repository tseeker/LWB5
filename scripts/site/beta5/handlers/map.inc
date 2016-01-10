<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
		"func"	=> array(
				"getMapParams", "updateData", "findName", "getPlayerPlanets"
			),
		"method"=> array(
			"updateData"	=> "POST",
		),
		"init"	=> "makeMapTooltips();\nx_getMapParams(mapInit);"
	);

	function	getMapParams()
	{
		$s = $_SESSION[game::sessName()]['map'] . "\n" . $this->findName($_SESSION[game::sessName()]['map_ctr']);
		$s2 = $this->getPlayerPlanets();
		if	($s2 != "")
			$s .= "\n$s2";
		return	$s;
	}

	function	getPlayerPlanets()
	{
		$as = array();
		$pl = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		foreach	($pl as $id => $name)
			array_push($as, "$id#$name");
		return	join("\n", $as);
	}

	function	findName($n)
	{
		$n = trim($n);
		$a = gameAction('getPlanetByName', $n);
		if	(is_null($a))
			return	"ERR";
		$o = ($a['owner'] == $_SESSION[game::sessName()]['player']) ? 0 : 1;
		$i = $o ? $a['name'] : $a['id'];
		return	$a['x']."#".$a['y']."#$o#$i";
	}

	function	updateData($rlist)
	{
		if	($rlist == "")
			return	"";

		$pid = $_SESSION[game::sessName()]['player'];
		$pinf = gameAction('getPlayerInfo', $pid);
		$tag = $pinf['aid'] ? $pinf['alliance'] : "";

		$l = explode('#', $rlist);
		$rs = array();
		foreach	($l as $rdata)
		{
			list($xt,$yt,$md5) = explode(',', $rdata);
			$x = (int)$xt; $y = (int)$yt;

			$sys = gameAction('getSystemAt', $x, $y);
			if (is_null($sys)) {
				if ($md5 != "-") {
					array_push($rs, "#$x#$y");
				}
				continue;
			}

			if ($sys['nebula'] > 0) {
				$mmd5 = md5(serialize($sys));
				if	($md5 == $mmd5)
					continue;
				array_push($rs, "{$sys['id']}#$x#$y#$mmd5#{$sys['nebula']}#0");
				$zones = gameAction('getSystemPlanets', $sys['id']);
				for ($i = 0; $i < 6; $i++) {
					array_push($rs, "{$zones[$i]['id']}#" . ($zones[$i]['status'] - 1)
						. "#{$zones[$i]['name']}");
				}
				continue;
			}

			switch (input::$game->params['victory']) {
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

			$sys['planets'] = gameAction('getSystemPlanets', $sys['id']);
			for ($i = 0; $i < 6; $i ++) {
				if	($sys['planets'][$i]['owner'] == $pid)
					$r = 2;
				elseif	($tag != "" && $sys['planets'][$i]['tag'] == $tag)
					$r = 1;
				else
					$r = 0;
				$sys['planets'][$i]['relation'] = $r;
			}

			$mmd5 = md5(serialize($sys));
			if ($md5 == $mmd5) {
				continue;
			}

			array_push($rs, "{$sys['id']}#$x#$y#$mmd5#{$sys['nebula']}#{$sys['prot']}");
			for ($i = 0; $i < 6; $i ++) {
				$p = $sys['planets'][$i];
				$s = $p['id']."#".$p['status']."#".$p['relation']."#".$p['tag'];
				array_push($rs, $s);
				array_push($rs, $p['name']);
			}
		}

		return	join("\n", $rs);
	}

	function	handle($input)
	{
		$ctrMap = null;
		switch	($input['menu']):
			case	'p':
				$_SESSION[game::sessName()]['map'] = 0;
				if	($input['ctr'] != '' && !is_null($mc = gameAction('getPlanetById', (int)$input['ctr'])))
					$ctrMap = $mc['name'];
			break;
			case	'a': $_SESSION[game::sessName()]['map'] = 1; break;
			case	'l': $_SESSION[game::sessName()]['map'] = 2; break;
		endswitch;
		if	(is_null($_SESSION[game::sessName()]['map']))
			$_SESSION[game::sessName()]['map'] = 0;

		if	(is_null($ctrMap))
			$_SESSION[game::sessName()]['map_ctr'] = gameAction('getFirstPlanet', $_SESSION[game::sessName()]['player']);
		else
			$_SESSION[game::sessName()]['map_ctr'] = $ctrMap;

		$this->output = "map";
	}
}

?>

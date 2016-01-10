<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			'func'	=> array(
				'setPage', 'getPlayerData',
				'getGeneralRankings', 'getDetailedRankings',
				'getAllianceRankings', 'getRoundRankings',
				'getDamageRankings'
			),
			'init'	=> "makeRanksTooltips();\ninitPage();"
		);

	function getRanking($type,$name) {
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', $type);
		$r = $this->rkLib->call('get', $rt, $name);
		if (!$r) {
			return	array('','');
		}
		return	$r;
	}

	function	getPlayerData($md5)
	{
		$pname = gameAction('getPlayerName', $_SESSION[game::sessName()]['player']);
		$gr = $this->getRanking('p_general', $pname);
		$cr = $this->getRanking('p_civ', $pname);
		$mr = $this->getRanking('p_military', $pname);
		$fr = $this->getRanking('p_financial', $pname);
		$or = $this->getRanking('p_round', $pname);
		$ir = $this->getRanking('p_idr', $pname);
		$a = array(
			$gr['points'], $gr['ranking'], $cr['points'], $cr['ranking'],
			$mr['points'], $mr['ranking'], $fr['points'], $fr['ranking'],
			$or['points'], $or['ranking'], $ir['points'], $ir['ranking']
		);
		$nmd5 = md5(serialize($a));
		if	($md5 == $nmd5)
			return	"KEEP";
		array_unshift($a, $nmd5);
		return	join('#', $a);
	}

	function	setPage($page)
	{
		$pok = array('Summary','General','Details','Alliance','Round', 'Damage');
		if	(in_array($page,$pok))
			$_SESSION[game::sessName()]['rkpage'] = $page;
		return	$_SESSION[game::sessName()]['rkpage'];
	}

	function	getGeneralRankings($param, $md5) {
		// Listing configuration
		$conf = array(
			"perPage"	=> array(5, 10, 15, 20, 25),
			"sortable"	=> array(
						"player"	=> SORT_STRING,
						"rank"		=> SORT_NUMERIC,
					),
			"searchModes"	=> array("player"),
			"output"	=> array(
						"isMe", "rank", "points", "player_real"
					)
		);

		// Get the data
		$players = array();
		$myName = gameAction('getPlayerName', $_SESSION[game::sessName()]['player']);
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', "p_general");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r)
			array_push($players, array(
				"player"	=> strtolower($r['id']),
				"player_real"	=> $r['id'],
				"rank"		=> $r['ranking'],
				"points"	=> $r['points'],
				"isMe"		=> ($r['id'] == $myName) ? 1 : 0
			));

		return	gameAction('generateListing', $players, $conf, $param, $md5);
	}

	function	getDetailedRankings($param, $md5) {
		// Listing configuration
		$conf = array(
			"perPage"	=> array(5, 10, 15, 20, 25),
			"sortable"	=> array(
						"player"	=> SORT_STRING,
						"civRank"	=> SORT_NUMERIC,
						"milRank"	=> SORT_NUMERIC,
						"finRank"	=> SORT_NUMERIC,
					),
			"searchModes"	=> array("player"),
			"output"	=> array(
						"isMe", "civRank", "civPoints",
						"milRank", "milPoints", "finRank",
						"finPoints", "player_real"
					)
		);

		// Get the data
		$players = array();
		$myName = gameAction('getPlayerName', $_SESSION[game::sessName()]['player']);
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', "p_civ");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r)
			$players[$r['id']] = array(
				"player"	=> strtolower($r['id']),
				"player_real"	=> $r['id'],
				"civRank"	=> $r['ranking'],
				"civPoints"	=> $r['points'],
				"isMe"		=> ($r['id'] == $myName) ? 1 : 0
			);

		$rt = $this->rkLib->call('getType', "p_military");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r) {
			$players[$r['id']]['milRank'] = $r['ranking'];
			$players[$r['id']]['milPoints'] = $r['points'];
		}

		$rt = $this->rkLib->call('getType', "p_financial");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r) {
			$players[$r['id']]['finRank'] = $r['ranking'];
			$players[$r['id']]['finPoints'] = $r['points'];
		}

		$data = array();
		foreach ($players as $id => $d)
			array_push($data, $d);

		return	gameAction('generateListing', $data, $conf, $param, $md5);
	}

	function	getAllianceRankings($param, $md5) {
		// Listing configuration
		$conf = array(
			"perPage"	=> array(5, 10, 15, 20, 25),
			"sortable"	=> array(
						"alliance"	=> SORT_STRING,
						"rank"		=> SORT_NUMERIC,
					),
			"searchModes"	=> array("alliance"),
			"output"	=> array(
						"rank", "points", "alliance_real"
					)
		);

		// Get the data
		$data = array();
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', "a_general");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r)
			array_push($data, array(
				"alliance"	=> strtolower($r['id']),
				"alliance_real"	=> $r['id'],
				"rank"		=> $r['ranking'],
				"points"	=> $r['points']
			));

		return	gameAction('generateListing', $data, $conf, $param, $md5);
	}

	function	getRoundRankings($param, $md5) {
		// Listing configuration
		$conf = array(
			"perPage"	=> array(5, 10, 15, 20, 25),
			"sortable"	=> array(
						"player"	=> SORT_STRING,
						"rank"		=> SORT_NUMERIC,
					),
			"searchModes"	=> array("player"),
			"output"	=> array(
						"isMe", "rank", "points", "player_real"
					)
		);

		// Get the data
		$players = array();
		$myName = gameAction('getPlayerName', $_SESSION[game::sessName()]['player']);
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', "p_round");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r)
			array_push($players, array(
				"player"	=> strtolower($r['id']),
				"player_real"	=> $r['id'],
				"rank"		=> $r['ranking'],
				"points"	=> $r['points'],
				"isMe"		=> ($r['id'] == $myName) ? 1 : 0
			));

		return	gameAction('generateListing', $players, $conf, $param, $md5);
	}

	function	getDamageRankings($param, $md5) {
		// Listing configuration
		$conf = array(
			"perPage"	=> array(5, 10, 15, 20, 25),
			"sortable"	=> array(
						"player"	=> SORT_STRING,
						"rank"		=> SORT_NUMERIC,
					),
			"searchModes"	=> array("player"),
			"output"	=> array(
						"isMe", "rank", "points", "player_real"
					)
		);

		// Get the data
		$players = array();
		$myName = gameAction('getPlayerName', $_SESSION[game::sessName()]['player']);
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', "p_idr");
		$rl = $this->rkLib->call('getAll', $rt);
		foreach	($rl as $r)
			array_push($players, array(
				"player"	=> strtolower($r['id']),
				"player_real"	=> $r['id'],
				"rank"		=> $r['ranking'],
				"points"	=> $r['points'],
				"isMe"		=> ($r['id'] == $myName) ? 1 : 0
			));

		return	gameAction('generateListing', $players, $conf, $param, $md5);
	}

	function	handle($input)
	{
		switch	($input['p']):
			case	's': $_SESSION[game::sessName()]['rkpage'] = 'Summary'; break;
			case	'g': $_SESSION[game::sessName()]['rkpage'] = 'General'; break;
			case	'd': $_SESSION[game::sessName()]['rkpage'] = 'Details'; break;
			case	'a': $_SESSION[game::sessName()]['rkpage'] = 'Alliance'; break;
			case	'o': $_SESSION[game::sessName()]['rkpage'] = 'Round'; break;
			case	'i': $_SESSION[game::sessName()]['rkpage'] = 'Damage'; break;
			default:
				if	(is_null($_SESSION[game::sessName()]['rkpage']))
					$_SESSION[game::sessName()]['rkpage'] = 'Summary';
				break;
		endswitch;
		$this->data = $_SESSION[game::sessName()]['rkpage'];
		$this->output = "rank";
	}
}

?>

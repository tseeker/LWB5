<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			'init'	=> "makeEnemyListTooltips();\ninitList();",
			'func'	=> array('getEnemies', 'removeEnemies', 'addEnemy'),
		);

	function	getEnemies()
	{
		$pid = $_SESSION[game::sessName()]['player'];
		$rs = array();

		$epl = gameAction('getEnemyPlayers', $pid);
		foreach	($epl as $id => $name)
			array_push($rs, "0#$id#".utf8entities($name));

		$eal = gameAction('getEnemyAlliances', $pid);
		foreach	($eal as $id => $name)
			array_push($rs, "1#$id#".utf8entities($name));

		return	join("\n", $rs);
	}

	function removeEnemies($type, $list) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$l = explode('#', $list);
		$action = 'removeEnemy' . ($type == "1" ? 'Alliance' : 'Player');
		foreach	($l as $eid) {
			gameAction($action, $pid, (int)$eid);
		}
		return	$this->getEnemies();
	}

	function addEnemy($type, $name) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}
		if ($type != "0" && $type != "1") {
			return	"ERR#0";
		}
		$name = preg_replace('/\s+/', ' ', trim($name));
		if	($name == "")
			return	"ERR#1";
		elseif	(($type == 0 && strlen($name) > 15) || ($type == 1 && strlen($name) > 5))
			return	"ERR#0";

		$pid = $_SESSION[game::sessName()]['player'];
		if ($type == 0 && $this->game->params['victory'] == 0 && strtolower($name) == 'ai>peacekeepers') {
			return "ERR#9";
		}
		$eid = gameAction($type == 0 ? "getPlayer" : "getAlliance", $name);
		if	(is_null($eid))
			return	"ERR#" . ($type == 0 ? 2 : 3);

		$list = array_keys(gameAction('getEnemy'. ($type == 0 ? 'Players' : 'Alliances'), $pid));
		if	(in_array($eid, $list))
			return	"ERR#" . ($type == 0 ? 4 : 5);

		if	($type == 0 && $eid == $pid)
			return	"ERR#6";
		elseif	($type == 0 && gameAction('isPlayerAlly', $pid, $eid))
			return	"ERR#8";
		elseif	($type == 1)
		{
			$pinf = gameAction('getPlayerInfo', $pid);
			if	(!is_null($pinf['aid']) && $pinf['aid'] == $eid)
				return	"ERR#7";
		}

		gameAction('addEnemy' . ($type == 0 ? 'Player' : 'Alliance'), $pid, $eid);

		return	$this->getEnemies();
	}

	function	handle($input)
	{
		$this->data = $this->getEnemies();
		$this->output = "enemylist";
	}
}

?>

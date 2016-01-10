<?php

class page_handler {
	var	$needsAuth = true;
	var	$ajax = array(
			"init" => "makeAllianceTooltips();\ninitAlliancePage();",
			"func" => array(
				// Main page
				"getGeneralData", "getTagInformation", "createAlliance", "sendRequest",
				"cancelRequest", "leaveAlliance", "modifySettings", "leaderStepDown",
				"mainPage",
				// Leader election
				"getCandidates", "setVote", "runForPresidency", "cancelCandidate",
				"takeControl",
				// Lists
				"getPlanetList", "getMemberList", "kickMembers", "changeMemberRank",
				"getAttackList",
				// Pending requests
				"getPending", "acceptRequests", "rejectRequests",
				// Forums
				"getForums", "newForum", "changeForum", "delForum",
				"moveForum", "getForumAcl",
				// Ranks
				"getRanks", "newRank", "changeRank", "delRank"
			),
			"method"=> array(
				"newForum"	=> "POST",
				"changeForum"	=> "POST",
				"newRank"	=> "POST",
				"changeRank"	=> "POST",
			),
		);


	//-------------------------------------------
	// MAIN PAGE FUNCTIONS
	//-------------------------------------------

	function getAllianceRanking($tag) {
		if (! $this->rkLib) {
			$this->rkLib = input::$game->getLib('main/rankings');
		}
		$rt = $this->rkLib->call('getType', "a_general");
		$r = $this->rkLib->call('get', $rt, $tag);
		if (!$r) {
			return	array('','');
		}
		return	$r;
	}

	function	mainPage()
	{
		$_SESSION[game::sessName()]['alliance_page'] = 'Main';
	}

	function getTagInformation($tag) {
		$tag = trim($tag);
		if ($tag == "") {
			return	"ERR#0";
		}

		$aid = gameAction('getAlliance', $tag);
		if (is_null($aid)) {
			return	"ERR#1";
		}

		$ainf = gameAction('getAllianceInfo', $aid);
		if (is_null($ainf)) {
			return	"ERR#1";
		}

		$s = $ainf['id'] . '#' . $ainf['nplanets'] . '#' . $ainf['avgx'] . '#' . $ainf['avgy'];
		$rk = $this->getAllianceRanking($tag);
		$s .= "#" . $rk['ranking'] . "#" . $rk['points'];

		if (!is_null($ainf['leader'])) {
			$alinf = gameAction('getPlayerInfo', $ainf['leader']);
		} else {
			$alinf = array();
		}

		if ($this->game->params['victory'] == 1) {
			$vVal = $this->game->getLib('beta5/alliance')->call('updateVictory', $aid);
		} elseif ($this->game->params['victory'] == 2) {
			$vVal = $this->game->getLib('beta5/ctf')->call('getTeamPoints', $aid);
		} else {
			$vVal = "";
		}

		return	"$s#$vVal\n" . $ainf['tag'] . "\n" . $ainf['name'] . "\n" . $alinf['name'];
	}

	function getGeneralData() {
		$pLib = $this->game->getLib('beta5/player');
		$aLib = $this->game->getLib('beta5/alliance');

		$pinf = $pLib->call('get', $player = $_SESSION[game::sessName()]['player']);
		if (is_null($pinf)) {
			l::warn("Couldn't find current player");
			return;
		}

		if (!is_null($pinf['aid'])) {
			$lockedAlliances = (int) $this->game->params['lockalliances'];
			$lockedAlliances = ($lockedAlliances > 1) ? 1 : 0;
			$retval = $pinf['aid'] . "#1#$lockedAlliances\n";
			$retval .= $pinf['alliance'] . "\n";
			$retval .= $pinf['aname'] . "\n";

			$ainf = $aLib->call('get', $pinf['aid']);
			$s  = $ainf['id'] . '#' . $ainf['nplanets'] . '#' . $ainf['avgx'] . '#' . $ainf['avgy'];
			$s .= '#' . ($ainf['democracy'] == 't' ? 1 : 0);
			$rk = $this->getAllianceRanking($ainf['tag']);
			$s .= "#" . $rk['ranking'] . "#" . $rk['points'] . "#"
				. $ainf['enable_tt'];
			if (!is_null($ainf['leader'])) {
				$alinf = $pLib->call('get', $ainf['leader']);
				if (!is_null($ainf['successor'])) {
					$asinf = $pLib->call('get', $ainf['successor']);
					$succ = $asinf['name'];
				} else {
					$succ = "";
				}
				$lName = $alinf['name'];
			} else {
				$lName = "";
			}
			if ($this->game->params['victory'] == 1) {
				$vVal = $aLib->call('updateVictory', $pinf['aid']);
			} elseif ($this->game->params['victory'] == 2) {
				$vVal = $this->game->getLib('beta5/ctf')->call('getTeamPoints', $pinf['aid']);
			} else {
				$vVal = "";
			}
			$retval .= "$s#$vVal\n$lName\n$succ\n";

			$pr = $aLib->call('getPrivileges', $player);
			$retval .= "{$pr['list_access']}#{$pr['attacks']}#"
				. "{$pr['can_set_grades']}#{$pr['can_kick']}#{$pr['can_accept']}#"
				. "{$pr['forum_admin']}#{$pr['can_vote']}#{$pr['can_be_cand']}#"
				. "{$pr['is_leader']}\n";
			if (is_null($_SESSION[game::sessName()]['alliance_page'])) {
				$_SESSION[game::sessName()]['alliance_page'] = 'Main';
			}
			$retval .= $_SESSION[game::sessName()]['alliance_page'];

			if ($pr['attacks']) {
				$retval .= "\n" . count($aLib->call('getMilitary', $pinf['aid']));
			}
		} elseif (!is_null($pinf['arid'])) {
			$retval  = $pinf['arid'] . "#0\n";
			$retval .= $pinf['alliance_req'] . "\n";
			$retval .= $pinf['aname'];
		} else {
			$retval = "";
		}
		return	$retval;
	}

	function sendRequest($tag) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#3";
		}

		$tag = preg_replace('/\s+/', ' ', trim($tag));
		if ($tag == "") {
			return "ERR#0";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (!(is_null($p["aid"]) && is_null($p['arid']))) {
			return	"ERR#2";
		}

		$a = gameAction('getAlliance', $tag);
		if (is_null($a)) {
			return	"ERR#1";
		}

		gameAction('sendJoinRequest', $pid, $a);
		return	$this->getGeneralData();
	}

	function cancelRequest() {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#201";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#5";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['arid']))
			return	"ERR#4";
		gameAction('cancelJoinRequest', $pid, $p['arid']);
		return	$this->getGeneralData();
	}

	function createAlliance($tag, $name) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#6";
		}

		$tag = preg_replace('/\s/', ' ', trim($tag));
		$name = preg_replace('/\s+/', ' ', trim($name));
		if (preg_match('/\s\s+/', $tag)) {
			return "ERR#0";
		}
		if ($tag == "") {
			return "ERR#1";
		}
		if ($name == "") {
			return "ERR#2";
		}
		if (strlen($tag) > 5 || strlen($name) > 64) {
			return "ERR#3";
		}

		$a = gameAction('getAlliance', $tag);
		if (!is_null($a)) {
			return	"ERR#4";
		}

		$a = gameAction('createAlliance', $tag, $name, $_SESSION[game::sessName()]['player']);
		if (is_null($a)) {
			return	"ERR#5";
		}
		return	$this->getGeneralData();
	}

	function modifySettings($dem, $techTrade, $succ) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return "ERR#0";
		}
		$a = gameAction('getAllianceInfo', $p['aid']);
		if ($a['leader'] != $pid) {
			return "ERR#0";
		}

		$succ = trim($succ);
		if ($succ == "") {
			$sid = null;
		} else {
			$sid = gameAction('getPlayer', $succ);
			if (is_null($sid)) {
				return "ERR#1";
			}
			if ($sid == $pid) {
				return "ERR#2";
			}
			$sinf = gameAction('getPlayerInfo', $sid);
			if ($sinf['aid'] !== $p['aid']) {
				return "ERR#3";
			}
		}

		gameAction('setAllianceDemo', $p['aid'],
			($dem == 1) || ((int) input::$game->params['lockalliances'] > 1));
		$this->game->getLib('beta5/alliance')->call('setTechTradeMode', $p['aid'], $techTrade);
		gameAction('setAllianceSuccessor', $p['aid'], $sid);
		return $this->getGeneralData();
	}

	function leaderStepDown() {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$a = gameAction('getAllianceInfo', $p['aid']);
		if	($a['leader'] != $pid || is_null($a['successor']))
			return	"ERR#0";

		gameAction('allianceStepDown', $p['aid']);
		return	$this->getGeneralData();
	}

	function leaveAlliance() {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#4";
		}

		gameAction('leaveAlliance', $_SESSION[game::sessName()]['player']);
		return	$this->getGeneralData();
	}


	//-------------------------------------------
	// PENDING REQUESTS
	//-------------------------------------------

	function getPending($ids) {
		if ((int) input::$game->params['lockalliances'] > 1) {
			return "";
		}
		$_SESSION[game::sessName()]['alliance_page'] = 'Pending';

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"";

		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['can_accept'] != 1)
			return	"";

		$l = gameAction('getAllianceRequests', $p['aid']);
		$ls = array();

		$lo = explode('#', $ids);
		foreach	($lo as $id)
		{
			if	($id == "" || (int)$id != $id || !is_null($l[$id]))
				continue;
			array_push($ls, "-#$id");
		}

		foreach ($l as $id => $n)
		{
			if	(in_array($id, $lo))
				continue;
			array_push($ls, "+#$id#" . utf8entities($n));
		}
		return	join("\n", $ls);
	}

	function acceptRequests($lall, $lacc) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#3";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";

		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['can_accept'] != 1)
			return	"ERR#1";

		$lo = explode('#', $lacc);

		// Apply alliance capping rules if required
		$cap = (int) input::$game->params['alliancecap'];
		if ($cap > 0 && $cap <= 100) {
			$libB5 = input::$game->getLib('beta5');
			$libAlliance = input::$game->getLib('beta5/alliance');

			$players = $libB5->call('getPlayerCount');
			$maxPlayers = ceil($players * $cap / 100);
			$members = count($libAlliance->call('getMembers', $p['aid']));

			//logText("Alliance cap= $maxPlayers players ($cap% of $players), new size = " . ($members + count($lo)));

			if ($members + count($lo) > $maxPlayers) {
				return "ERR#2";
			}
		}

		foreach	($lo as $id)
		{
			if	($id == "" || (int)$id != $id)
				continue;
			gameAction('acceptAllianceRequest', $p['aid'], $id, $pid);
		}

		return	$this->getPending($lall);
	}

	function rejectRequests($lall, $lrej) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#3";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return	"ERR#0";
		}

		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['can_accept'] != 1)
			return "ERR#1";

		$lo = explode('#', $lrej);
		foreach	($lo as $id) {
			if ($id == "" || (int)$id != $id) {
				continue;
			}
			gameAction('rejectAllianceRequest', $p['aid'], $id, $pid);
		}

		return	$this->getPending($lall);
	}


	//-------------------------------------------
	// LISTINGS
	//-------------------------------------------

	function getPlanetList($ids) {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return	"";
		}
		$pr = gameAction('getAlliancePrivileges', $pid);
		if ($pr['list_access'] < 2) {
			return	"";
		}
		$_SESSION[game::sessName()]['alliance_page'] = 'PList';

		$pl = gameAction('getAlliancePlanets', $p['aid']);
		$lo = explode('#', $ids);
		$ls = array();

		// Remove planets that are no longer in the list
		foreach	($lo as $id)
		{
			if	($id == "" || (int)$id != $id || !is_null($pl[$id]))
				continue;
			array_push($ls, "-#$id");
		}

		// Add new planets and update current list
		foreach	($pl as $id => $pd)
		{
			if	(in_array($id, $lo))
				$s = "=";
			else
				$s = "+";
			$s .= "#$id#";
			if	($pid != $pd['ownerId'])
				$s .= $pd['ownerId'];
			$s .= "#" . $pd['x'] . "#" . $pd['y'] . "#" . $pd['orbit'] . "#";
			if	($pr['list_access'] == 3)
				$s .= $pd['fact'] . "#" . $pd['turrets'];
			else
				$s .= "#";
			$s .= "#";
			if	($pr['attacks'] == 1)
				$s .= $pd['attack'] ? 1 : 0;
			$s .= "\n" . utf8entities($pd['owner']) . "\n" . utf8entities($pd['name']);
			array_push($ls, $s);
		}
		return	join("\n", $ls);
	}

	function getMemberList($mIds) {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"";
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['list_access'] < 1)
			return	"";
		$_SESSION[game::sessName()]['alliance_page'] = 'MList';

		$rkl = gameAction('getAllianceRanks', $p['aid']);
		$mbl = gameAction('getAllianceMembers', $p['aid']);
		$a = gameAction('getAllianceInfo', $p['aid']);

		// Transform the member list into a list of strings indicating changes
		$mcl = array();
		$lo = explode('#', $mIds);
		$now = time();
		foreach	($lo as $mid) {
			if ($mid == "" || (int)$mid != $mid) {
				continue;
			}
			if (is_null($mbl[$mid])) {
				array_push($mcl, "-#$mid");
			} else {
				$rank = ($mid == $a['leader']) ? "-"
					: (is_null($mbl[$mid]['rank'])
						? $a['default_grade']
						: $mbl[$mid]['rank']);
				$lastOnline = $this->game->getLib('beta5/player')->call('lastOnline', $mid);
				if ($lastOnline != 0) {
					$lastOnline = ceil(($now - $lastOnline) / 60);
				}
				$onVacation = $this->game->getLib('beta5/player')->call('isOnVacation', $mid) ? 1 : 0;
				array_push($mcl, "=#$mid#$rank#$lastOnline#$onVacation");
			}
		}
		foreach	($mbl as $id => $cnt)
		{
			if	(in_array($id,$lo))
				continue;
			$rank = ($id == $a['leader']) ? "-"
				: (is_null($mbl[$id]['rank'])
					? $a['default_grade']
					: $mbl[$id]['rank']);
			$isMe = ($id == $pid) ? 0 : 1;
			$lastOnline = $this->game->getLib('beta5/player')->call('lastOnline', $id);
			if ($lastOnline != 0) {
				$lastOnline = ceil(($now - $lastOnline) / 60);
			}
			$onVacation = $this->game->getLib('beta5/player')->call('isOnVacation', $id) ? 1 : 0;
			array_push($mcl, "+#$id#$rank#$isMe#$lastOnline#$onVacation#" . $mbl[$id]['name']);
		}

		// Transform ranks into a list of strings
		$rks = array();
		foreach	($rkl as $i => $n)
			array_push($rks, "$i#$n");

		$s = count($rkl) . "#" . count($mcl) . "\n";
		$s .= join('#', $pr['change_ranks']) . "\n";
		$s .= join('#', $pr['kick_ranks']) . "\n";
		$s .= join("\n", array_merge($mcl, $rks));

		return	$s;
	}

	function changeMemberRank($mIds, $rId, $allIds) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['list_access'] < 1 || $pr['can_set_grades'] == 0 || !in_array($rId, $pr['change_ranks']))
			return	"ERR#1";
		$a = gameAction('getAllianceInfo', $aid);
		if	($rId == $a['default_grade'])
			$rId = 'NULL';

		$mlist = gameAction('getAllianceMembers', $aid);
		$mlids = array_keys($mlist);
		$m2c = explode('#', $mIds);
		foreach	($m2c as $id)
		{
			if	(!in_array($id, $mlids) || $id == $pid || !(is_null($mlist[$id]['rank']) || in_array($mlist[$id]['rank'], $pr['change_ranks'])))
				continue;
			gameAction('changeMemberRank', $id, $rId);
		}
		return	$this->getMemberList($allIds);
	}

	function kickMembers($mIds, $allIds) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		} elseif ((int) input::$game->params['lockalliances'] > 1) {
			return "ERR#2";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['list_access'] < 1 || $pr['can_kick'] == 0)
			return	"ERR#1";
		$a = gameAction('getAllianceInfo', $aid);

		$mlist = gameAction('getAllianceMembers', $aid);
		$mlids = array_keys($mlist);
		$m2c = explode('#', $mIds);
		foreach	($m2c as $id)
		{
			if	(is_null($mlist[$id]['rank']))
				$mlist[$id]['rank'] = $a['default_grade'];
			if	(!in_array($id, $mlids) || $id == $pid || !in_array($mlist[$id]['rank'], $pr['kick_ranks']))
				continue;
			gameAction('kickAllianceMember', $id);
		}
		return	$this->getMemberList($allIds);
	}

	function getAttackList() {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"";
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	($pr['attacks'] == 0)
			return	"";
		$_SESSION[game::sessName()]['alliance_page'] = 'MStat';

		$pl = gameAction('getAllianceMilitary', $p['aid']);
		$ls = array();

		foreach	($pl as $id => $data)
		{
			$s  = "$id#";
			if	($pr['list_access'] > 1)
				$s .= ($data['owner'] == $pid) ? -1 : $data['owner'];
			$s .= "#".$data['friendly']."#".$data['enemy']."#";
			$s .= count($data['f_list']) . '#' . count($data['e_list']);
			$s .= "#".$data['x'].",".$data['y'].",".$data['orbit'];
			$s .= "\n".$data['name'];
			if	($pr['list_access'] > 1)
				$s .= "\n" . gameAction('getPlayerName', $data['owner']);
			for	($i=0;$i<count($data['f_list']);$i++)
				$s .= "\n" . gameAction('getPlayerName', $data['f_list'][$i]);
			for	($i=0;$i<count($data['e_list']);$i++)
				$s .= "\n" . gameAction('getPlayerName', $data['e_list'][$i]);
			array_push($ls, $s);
		}

		return	join("\n", $ls);
	}


	//-------------------------------------------
	// LEADER ELECTION
	//-------------------------------------------

	function getCandidates() {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"";
		$aid = $p['aid'];

		$locked = (int) input::$game->params['lockalliances'] > 1;
		$a = gameAction('getAllianceInfo', $aid);
		if ($a['democracy'] == 'f' && !$locked) {
			return	"";
		}
		$_SESSION[game::sessName()]['alliance_page'] = 'Vote';

		$pr = gameAction('getAlliancePrivileges', $pid);
		$cl = gameAction('getAllianceCandidates', $aid);
		$ml = gameAction('getAllianceMembers', $aid);
		$cpl = array();
		$mv = 0;
		foreach	($cl as $cid => $cd)
		{
			$cpl[$cd['pid']] = $cid;
			if	($cd['votes'] > $mv)
				$mv = $cd['votes'];
		}

		$afSel = ($pr['can_vote'] == 1 || $locked) && is_null($cpl[$pid]);
		$afRun = ($pr['can_be_cand'] == 1 || $locked) && is_null($cpl[$pid]);
		$afCancel = ($pr['can_be_cand'] == 1 || $locked) && !is_null($cpl[$pid]) && !$pr['is_leader'];
		$afTake = !is_null($cpl[$pid]) && !$pr['is_leader'] && ($cl[$cpl[$pid]]['votes'] >= 0.2 * count($ml)) && ($cl[$cpl[$pid]]['votes'] == $mv);
		$voteAct = is_null($cpl[$pid]) ? $p['vote'] : $cpl[$pid];

		$s = ($afSel?1:0) . "#" . ($afRun?1:0) . "#" . ($afCancel?1:0) . "#" . ($afTake?1:0) . "#$voteAct";
		foreach	($cl as $cid => $cd)
			$s .= "\n$cid#".$cd['votes']."#".utf8entities($cd['name']);

		return	$s;
	}

	function setVote($vid) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return "ERR#0";
		}
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		$locked = (int) input::$game->params['lockalliances'] > 1;
		if (!($pr['can_vote'] || $locked)) {
			return "ERR#1";
		}

		$cl = gameAction('getAllianceCandidates', $aid);
		$cpl = array();
		foreach	($cl as $cid => $cd)
			$cpl[$cd['pid']] = $cid;
		if (!(is_null($cpl[$pid]) && ($vid == "" || !is_null($cl[$vid])))) {
			return "ERR#2";
		}
		gameAction('setAllianceVote', $pid, ($vid == "") ? 'NULL' : $vid);
		return $this->getCandidates();
	}

	function runForPresidency() {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return "ERR#0";
		}
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		$locked = (int) input::$game->params['lockalliances'] > 1;
		if (!($pr['can_be_cand'] || $locked)) {
			return "ERR#1";
		}

		$cl = gameAction('getAllianceCandidates', $aid);
		foreach	($cl as $cid => $cd) {
			if ($cd['pid'] == $pid) {
				return "ERR#3";
			}
		}

		gameAction('newAllianceCandidate', $aid, $pid);
		return	$this->getCandidates();
	}

	function cancelCandidate() {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return "ERR#0";
		}
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		$locked = (int) input::$game->params['lockalliances'] > 1;
		if (!($pr['can_be_cand'] || $locked)) {
			return "ERR#1";
		}

		$cl = gameAction('getAllianceCandidates', $aid);
		$nc = true;
		foreach	($cl as $cid => $cd) {
			if ($cd['pid'] == $pid) {
				$nc = false;
				break;
			}
		}

		if ($nc) {
			return "ERR#4";
		}
		gameAction('removeAllianceCandidate', $aid, $pid);
		return	$this->getCandidates();
	}

	function takeControl() {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return "ERR#0";
		}
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		$locked = (int) input::$game->params['lockalliances'] > 1;
		if (!($pr['can_be_cand'] || $locked)) {
			return "ERR#1";
		}

		$cl = gameAction('getAllianceCandidates', $aid);
		$ml = gameAction('getAllianceMembers', $aid);
		$cpl = array();
		$mv = 0;
		foreach	($cl as $cid => $cd) {
			$cpl[$cd['pid']] = $cid;
			if ($cd['votes'] > $mv) {
				$mv = $cd['votes'];
			}
		}

		$afTake = !is_null($cpl[$pid]) && !$pr['is_leader'] && ($cl[$cpl[$pid]]['votes'] >= 0.2 * count($ml)) && ($cl[$cpl[$pid]]['votes'] == $mv);
		if ($afTake) {
			gameAction('takePresidency', $aid, $pid);
		} else {
			return "ERR#5";
		}
		return	$this->getCandidates();
	}


	//-------------------------------------------
	// ALLIANCE FORUMS MANAGEMENT
	//-------------------------------------------

	function doGetForums($aid) {
		$afl = gameAction('getAllianceForums', $aid);
		$s = "";
		foreach	($afl as $id => $afd)
		{
			if	($s != "")
				$s .= "\n";
			$s .= "$id#" . $afd['order'] . "#" . ($afd['user_post'] ? 1 : 0) . "#" . $afd['title'];
			if	($afd['description'] != '')
			{
				$dll = split("\n", $afd['description']);
				foreach	($dll as $dl)
					$s .= "\n+#$dl";
			}
		}
		return	$s;
	}

	function getForums() {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return	"ERR#0";
		}
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if (!$pr['forum_admin']) {
			return	"ERR#4";
		}
		$_SESSION[game::sessName()]['alliance_page'] = 'FAdmin';

		return	$this->doGetForums($aid);
	}

	function newForum($name, $userPost, $after, $description, $acl) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	(!$pr['forum_admin'])
			return	"ERR#4";

		$afl = gameAction('getAllianceForums', $aid);
		if (count($afl) >= 30) {
			return	"ERR#5";
		}

		$name = preg_replace('/\s+/', ' ', trim($name));
		if	($name == "" || strlen($name) < 4)
			return	"ERR#1";
		foreach	($afl as $fid => $fd)
			if	($fd['title'] == $name)
				return	"ERR#2";

		if	($after != "-1" && is_null($afl[$after]))
			return	"ERR#6";

		$description = trim($description);
		gameAction('newAllianceForum', $aid, $name, ($userPost == 1), $after, $description);

		$afl = gameAction('getAllianceForums', $aid);
		$mId = false;
		foreach ($afl as $fid => $fd)
			if	($fd['title'] == $name)
			{
				$mId = $fid;
				break;
			}
		if (!$mId) {
			return	"ERR#7";
		}

		$rl = gameAction('getAllianceRanks', $aid);
		$fread = $fmod = array();
		$acla = explode('#', $acl);
		foreach	($acla as $as)
		{
			list($rank,$level) = explode('!', $as);
			$level --;
			if	(is_null($rl[$rank]) || ($level != 0 && $level != 1))
				continue;
			if	($level)
				array_push($fmod, $rank);
			else
				array_push($fread, $rank);
		}
		gameAction('setForumAccess', $mId, $fread, $fmod);

		return	$this->doGetForums($aid);
	}

	function changeForum($id, $name, $userPost, $description, $acl) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	(!$pr['forum_admin'])
			return	"ERR#1";

		$afl = gameAction('getAllianceForums', $aid);
		if	(is_null($afl[$id]))
			return	"ERR#3";

		$name = preg_replace('/\s+/', ' ', trim($name));
		if	($name == "" || strlen($name) < 4)
			return	"ERR#1";
		foreach	($afl as $fid => $fd)
			if	($fid != $id && $fd['name'] == $name)
				return	"ERR#2";

		$description = trim($description);
		gameAction('modifyAllianceForum', $id, $name, ($userPost == 1), $description);

		$rl = gameAction('getAllianceRanks', $aid);
		$fread = $fmod = array();
		$acla = explode('#', $acl);
		foreach	($acla as $as)
		{
			list($rank,$level) = explode('!', $as);
			$level --;
			if	(is_null($rl[$rank]) || ($level != 0 && $level != 1))
				continue;
			if	($level)
				array_push($fmod, $rank);
			else
				array_push($fread, $rank);
		}
		gameAction('setForumAccess', $id, $fread, $fmod);

		return	$this->doGetForums($aid);
	}

	function delForum($id) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	(!$pr['forum_admin'])
			return	"ERR#1";

		$afl = gameAction('getAllianceForums', $aid);
		if (is_null($afl[$id])) {
			return "ERR#8";
		}
			gameAction('deleteAllianceForum', $id);
		return	$this->doGetForums($aid);
	}

	function moveForum($id, $up) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	(!$pr['forum_admin'])
			return	"ERR#1";

		$afl = gameAction('getAllianceForums', $aid);
		if	(!is_null($afl[$id]))
			gameAction('moveAllianceForum', $id, ($up == "1"));
		return	$this->doGetForums($aid);
	}

	function getForumAcl($id) {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"";
		$aid = $p['aid'];
		$pr = gameAction('getAlliancePrivileges', $pid);
		if	(!$pr['forum_admin'])
			return	"";
		if	($id != "" && (int)$id != (string)$id)
			return	"";

		$l = gameAction('getAllianceRanks', $aid);
		$rs = array();
		foreach	($l as $rId => $rName)
		{
			$s = "$rId#";
			$pr = gameAction('getRankPrivileges', $rId);

			if	($id == "")
				$s .= $pr['forum_admin'] ? 3 : 0;
			else
				$s .= $pr['forum_admin'] ? 3 : (in_array($id, $pr['f_mod']) ? 2 : (in_array($id, $pr['f_read']) ? 1 : 0));
			$s .= "#".utf8entities($rName);
			array_push($rs, $s);
		}
		return	join("\n", $rs);
	}


	//-------------------------------------------
	// RANKS MANAGEMENT
	//-------------------------------------------

	function doGetRanks($aid) {
		$aLib = $this->game->getLib('beta5/alliance');

		$afl = $aLib->call('getForums', $aid);
		$forums = array();
		foreach	($afl as $fid => $fd) {
			array_push($forums, "$fid#" . $fd['title']);
		}

		$arl = $aLib->call('getRanks', $aid);
		$ranks = array();
		foreach	($arl as $rid => $rname) {
			$r = array();

			$pr = $aLib->call('getRankPrivileges', $rid);
			$nbp = $aLib->call('getRankSize', $aid, $rid);
			$str  = "$rid#$nbp#{$pr['list_access']}#{$pr['attacks']}#{$pr['can_set_grades']}"
					. "#{$pr['can_kick']}#{$pr['can_accept']}#{$pr['forum_admin']}#"
					. "{$pr['dipl_contact']}#{$pr['can_vote']}#{$pr['can_be_cand']}"
					. "#{$pr['tech_trade']}";
			array_push($r, $str);
			array_push($r, $rname);

			if ($pr['can_kick']) {
				array_push($r, join('#', $pr['kick_ranks']));
			}
			if ($pr['can_set_grades']) {
				array_push($r, join('#', $pr['change_ranks']));
			}
			array_push($r, join('#', $pr['f_read']));
			array_push($r, join('#', $pr['f_mod']));

			array_push($ranks, join("\n", $r));
		}

		$a = array(count($forums) . "#" . count($ranks));
		$a = array_merge($a, $forums);
		$a = array_merge($a, $ranks);
		return	join("\n", $a);
	}

	function doRankEdit($aid, $name, $privileges, $rkick, $rchange, $fread, $fmod, $id = null) {
		$rl0 = gameAction('getAllianceRanks', $aid);
		$rl = array_keys($rl0);
		$rn = array_values($rl0);
		if (!(is_null($id) || in_array($id, $rl))) {
			return	"ERR#5";
		}

		// Check for valid name
		$a = gameAction('getAllianceInfo', $aid);
		if ($id != $a['default_grade']) {
			$name = preg_replace('/\s+/', ' ', trim($name));
			if (strlen($name) < 4) {
				return "ERR#1";
			} elseif (strlen($name) > 32) {
				return "ERR#2";
			} elseif (in_array($name, $rn) && (is_null($id) || $rl0[$id] != $name)) {
				return "ERR#3";
			}
		} else {
			$name = null;
		}

		// Check privileges
		$pl = split('#', $privileges);
		$pnames = array('list_access', 'attacks', 'can_set_grades', 'can_kick',
			'can_accept', 'forum_admin', 'dipl_contact', 'can_vote', 'can_be_cand',
			'tech_trade');
		if (count($pl) != count($pnames)) {
			return "ERR#6";
		}
		$privs = array();
		for ($i = 0; $i < count($pnames); $i ++) {
			$pl[$i] = (int) $pl[$i];
			switch ($i) {
				case 0: $mv = 3; break;
				case 9: $mv = 4; break;
				default: $mv = 1; break;
			}
			if ($pl[$i] < 0 || $pl[$i] > $mv) {
				return "ERR#6";
			}
			$privs[$pnames[$i]] = $pl[$i];
		}

		// Check kickable ranks
		if ($privs['can_kick'] && $rkick != "") {
			$rkl = split('#', $rkick);
			for ($i=0;$i<count($rkl);$i++) {
				$rkl[$i] = (int)$rkl[$i];
				if (!in_array($rkl[$i], $rl)) {
					return	"ERR#5";
				}
			}
		} else {
			$rkl = array();
		}

		// Check changeable ranks
		if ($privs['can_set_grades'] && $rchange != "") {
			$rcl = split('#', $rchange);
			for ($i=0;$i<count($rcl);$i++) {
				$rcl[$i] = (int)$rcl[$i];
				if (!in_array($rcl[$i], $rl)) {
					return	"ERR#5";
				}
			}
		} else {
			$rcl = array();
		}

		$fl = array_keys(gameAction('getAllianceForums', $aid));
		if (!$privs['forum_admin']) {
			// Check readable forums
			if ($fread != '') {
				$frl = split('#', $fread);
				for ($i=0;$i<count($frl);$i++) {
					$frl[$i] = (int)$frl[$i];
					if (!in_array($frl[$i], $fl)) {
						return	"ERR#7";
					}
				}
			} else {
				$frl = array();
			}

			// Check moderated forums
			if ($fmod != '') {
				$mrl = split('#', $fmod);
				for ($i=0;$i<count($mrl);$i++) {
					$mrl[$i] = (int)$mrl[$i];
					if (!in_array($mrl[$i], $fl)) {
						return	"ERR#7";
					}
				}
			} else {
				$mrl = array();
			}
		} else {
			$frl = array();
			$mrl = array();
		}

		if (is_null($id)) {
			gameAction('createAllianceRank', $aid, $name, $privs, $rkl, $rcl, $frl, $mrl);
		} else {
			gameAction('modifyAllianceRank', $id, $name, $privs, $rkl, $rcl, $frl, $mrl);
		}
		return	$this->doGetRanks($aid);
	}

	function getRanks() {
		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"";
		$aid = $p['aid'];
		$a = gameAction('getAllianceInfo', $aid);
		if	($a['leader'] != $pid)
			return	"";
		$_SESSION[game::sessName()]['alliance_page'] = 'RAdmin';

		return	$this->doGetRanks($aid);
	}

	function newRank($name, $privileges, $rkick, $rchange, $fread, $fmod) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return	"ERR#0";
		}
		$aid = $p['aid'];
		$a = gameAction('getAllianceInfo', $aid);
		if ($a['leader'] != $pid) {
			return	"ERR#4";
		}

		return	$this->doRankEdit($aid, $name, $privileges, $rkick, $rchange, $fread, $fmod);
	}

	function changeRank($id, $name, $privileges, $rkick, $rchange, $fread, $fmod) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if	(is_null($p['aid']))
			return	"ERR#0";
		$aid = $p['aid'];
		$a = gameAction('getAllianceInfo', $aid);
		if	($a['leader'] != $pid)
			return	"ERR#4";

		return	$this->doRankEdit($aid, $name, $privileges, $rkick, $rchange, $fread, $fmod, $id);
	}

	function delRank($id, $dmId) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$p = gameAction('getPlayerInfo', $pid);
		if (is_null($p['aid'])) {
			return	"ERR#0";
		}
		$aid = $p['aid'];
		$a = gameAction('getAllianceInfo', $aid);
		if ($a['leader'] != $pid) {
			return	"ERR#4";
		}

		$rl = array_keys(gameAction('getAllianceRanks', $aid));
		if ($id == $a['default_grade'] || !in_array($id, $rl) || !in_array($dmId, $rl)) {
			return	"ERR#5";
		}
		if ($dmId == $a['default_grade']) {
			$dmId = "NULL";
		}
		gameAction('deleteAllianceRank', $aid, $id, $dmId);

		return	$this->doGetRanks($aid);
	}

	//-------------------------------------------
	// MAIN HANDLER
	//-------------------------------------------

	function	handle($input)
	{
		$this->data = $this->getGeneralData();
		$this->output = "alliance";
	}
}

?>

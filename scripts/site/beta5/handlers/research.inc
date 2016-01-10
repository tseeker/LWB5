<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			"func"	=> array(
				'getMode', 'setMode', 'getDescriptions',
				'getTopicsData', 'getLawsData', 'getAttributionsData',
				'implementTechnology', 'switchLaw', 'validateBudget',
				'getDiplomacyData', 'sendOffer', 'acceptOffer',
				'declineOffer'
			),
			"init"	=> "makeResearchTooltips(); x_getMode(initResearchPage);"
		);

	function	guessResearchMode($i)
	{
		switch	($i['p']) :
			case 't': $_SESSION[game::sessName()]['respage_mode'] = 'Topics'; break;
			case 'l': $_SESSION[game::sessName()]['respage_mode'] = 'Laws'; break;
			case 'b': $_SESSION[game::sessName()]['respage_mode'] = 'Attributions'; break;
			case 'd': $_SESSION[game::sessName()]['respage_mode'] = 'Diplomacy'; break;
		endswitch;
		if	(is_null($_SESSION[game::sessName()]['respage_mode']))
			$_SESSION[game::sessName()]['respage_mode'] = 'Topics';
		return	$_SESSION[game::sessName()]['respage_mode'];
	}

	function	getMode()
	{
		if	(is_null($_SESSION[game::sessName()]['respage_mode']))
			return	$this->guessResearchMode(array());
		return	$_SESSION[game::sessName()]['respage_mode'];
	}

	function	setMode($nm)
	{
		$pm = array('Topics', 'Laws', 'Attributions', 'Diplomacy');
		if	(!in_array($nm, $pm))
			return	$this->guessResearchMode(array());

		$_SESSION[game::sessName()]['respage_mode'] = $nm;
		return	$nm;
	}

	function	getDescriptions($lst)
	{
		$l = getLanguage();
		$a = split('#', $lst);
		$s = "";
		foreach	($a as $id)
		{
			if	((int)$id != $id)
				continue;
			$ns = gameAction('getResearchData', $l, $id);
			if	($ns == "")
				continue;
			if	($s != "")
				$s .= "\n";
			$s .= $ns;
		}
		return	$s;
	}

	function	getTopicsData()
	{
		$pl = $_SESSION[game::sessName()]['player'];
		$impl = gameAction('getResearchTopics', $pl, 1);
		$disp = gameAction('getResearchTopics', $pl, 0);
		$brkt = gameAction('getResearchTopics', $pl, -1);
		return join("#", $impl) . "!" . join("#", $disp) . "!" . join("#", $brkt);
	}

	function implementTechnology($id) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}
		if ((int)$id != $id) {
			logText("(CHEAT) research/implement received weird ID from user {$_SESSION['userid']}");
			return "ERR#1";
		}
		return	(gameAction('implementTechnology', $_SESSION[game::sessName()]['player'], $id) ? "OK" : "ERR#0");
	}

	function getLawsData() {
		return join('#', gameAction('getLaws', $_SESSION[game::sessName()]['player']));
	}

	function switchLaw($id) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}
		if ((int)$id != $id) {
			return	"ERR#1";
		}
		return	(gameAction('switchLaw', $_SESSION[game::sessName()]['player'], $id) ? "OK" : "ERR#0");
	}

	function getAttributionsData() {
		$pl = $_SESSION[game::sessName()]['player'];
		$a = array(gameAction('getResearchPoints', $pl));
		$b = gameAction('getResearchBudget', $pl);
		$a = array_merge($a, $b);
		return	join('#', $a);
	}

	function validateBudget($b) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$a = split('#', $b);
		if (count($a) != 3) {
			return	"ERR#0";
		}
		$s = 0;
		for ($i=0;$i<count($a);$i++) {
			$s += $a[$i];
			if ((int)$a[$i] != $a[$i] || $a[$i] < 0 || $s > 100) {
				return "ERR#0";
			}
		}
		gameAction('setResearchBudget', $_SESSION[game::sessName()]['player'], $a);
		return "OK";
	}

	function	getDiplomacyData()
	{
		$pl = $_SESSION[game::sessName()]['player'];
		$pr = gameAction('isPlayerRestrained', $pl);
		if	($pr > 0)
			return	$pr;

		$impl = gameAction('getResearchTopics', $pl, 1);
		$impl = array_merge($impl, gameAction('getResearchTopics', $pl, 0));
		$time = $this->game->ticks['day']->interval - $this->game->ticks['hour']->interval * 2;
		$s1 = $time . "\n" . join('#',$impl);
		$laws = gameAction('getLaws', $pl);
		for	($i=0;$i<count($laws);$i+=2)
		{
			if	($s1 != "")
				$s1 .= "#";
			$s1 .= $laws[$i];
		}

		$ol = gameAction('getResearchOffers', $pl);
		$str = "";
		foreach	($ol as $o)
		{
			$str .= "\n".$o['id']."#".$o['type']."#".$o['status']."#".$o['price']."#".$o['tech']."#".$o['time'];
			$str .= "#".$o['pending']."\n".$o['player'];
		}
		if	($str == "")
			$str = "\n";
		$str = $s1.$str;

		return	$str;
	}

	function sendOffer($ty, $tid, $pname, $pr) {
		$pLib = $this->game->getLib('beta5/player');
		$cpid = $_SESSION[game::sessName()]['player'];

		if ($pLib->call('isOnVacation', $cpid)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $cpid)) {
			return "ERR#201";
		}

		$pname = trim($pname);
		if ($pname == "") {
			return "ERR#2";
		}

		$tpid = $pLib->call('getPlayerId', $pname);
		if (is_null($tpid)) {
			return "ERR#4";
		} elseif ($tpid == $cpid) {
			return "ERR#6";
		}

		$tLib = $this->game->getLib('beta5/tech');
		if ($ty == 0) {
			$tid = null;
		} elseif ($ty == 1) {
			$tid = (int)$tid;
			if (!$tLib->call('has', $cpid, $tid)) {
				logText("*** CHEAT? research::sendOffer: player $cpid trying to send tech $tid");
				return "ERR#5";
			}
		} else {
			logText("*** CHEAT? research::sendOffer: player $cpid, received invalid status '$ty'");
			return "ERR#8";
		}

		if ("$pr" != (int)$pr || $pr < 0) {
			return "ERR#3";
		}
		if ($tLib->call('checkOffer', $cpid)) {
			logText("*** CHEAT? research::sendOffer: player $cpid has already sent an offer");
			return "ERR#9";
		}

		if ($pLib->call('isOnVacation', $tpid)) {
			return "ERR#7";
		} elseif ($pLib->call('getProtectionLevel', $tpid)) {
			return "ERR#10";
		}

		$tLib->call('makeOffer', $cpid, $tpid, $tid, $pr);

		return	$this->getDiplomacyData();
	}

	function acceptOffer($oid) {
		$pLib = $this->game->getLib('beta5/player');
		$pid = $_SESSION[game::sessName()]['player'];

		if ($pLib->call('isOnVacation', $pid)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $pid)) {
			return "ERR#201";
		}

		$oid = (int)$oid;
		$a = gameAction('acceptResearchOffer', $pid, $oid);
		if	($a)
			return	"ERR#$a";
		return	$this->getDiplomacyData();
	}

	function declineOffer($oid) {
		$pLib = $this->game->getLib('beta5/player');
		$pid = $_SESSION[game::sessName()]['player'];

		if ($pLib->call('isOnVacation', $pid)) {
			return "ERR#200";
		} elseif ($pLib->call('getProtectionLevel', $pid)) {
			return "ERR#201";
		}

		$oid = (int)$oid;
		$a = gameAction('declineResearchOffer', $pid, $oid);
		if	($a)
			return	"ERR#$a";
		return	$this->getDiplomacyData();
	}

	function	handle($input)
	{
		switch	($input['mode']) :
		case	't':
			$this->setMode('Topics');
			break;
		case	'l':
			$this->setMode('Laws');
			break;
		case	'b':
			$this->setMode('Attributions');
			break;
		case	'd':
			$this->setMode('Diplomacy');
			break;
		default:
			$this->guessResearchMode($input);
		endswitch;
		$this->output = "research";
	}
}

?>

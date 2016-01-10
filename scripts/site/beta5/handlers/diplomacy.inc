<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			'func'	=> array('getInformation'),
			'init'	=> "makeDiplomacyTooltips();\ninitPage();"
		);

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

	function	getInformation()
	{
		$out = array();
		$pid = $_SESSION[game::sessName()]['player'];
		$pinf = gameAction('getPlayerInfo', $pid);
		if	(!is_null($pinf['arid']))
		{
			$ainf = gameAction('getAllianceInfo', $pinf['arid']);
			$s  = "1#" . $ainf['nplanets'] . '#' . $ainf['avgx'] . '#' . $ainf['avgy'];
			list($points,$ranking) = $this->getAllianceRanking($ainf['tag']);
			$s .= "#$ranking#$points";
			array_push($out, $s);
			array_push($out, utf8entities($pinf['alliance_req']));
			array_push($out, utf8entities($pinf['aname']));
			$alinf = gameAction('getPlayerName', $ainf['leader']);
			array_push($out, utf8entities($alinf));
		}
		elseif	(!is_null($pinf['aid']))
		{
			$ainf = gameAction('getAllianceInfo', $pinf['aid']);
			$pr = gameAction('getAlliancePrivileges', $pid);

			$s  = "2#" . $ainf['nplanets'] . '#' . $ainf['avgx'] . '#' . $ainf['avgy'];
			list($points,$ranking) = $this->getAllianceRanking($ainf['tag']);
			$s .= "#$ranking#$points#";
			$s .= $pr['is_leader'] . "#" . (count($pr['f_read']) + count($pr['f_mod']));
			array_push($out, $s);
			array_push($out, utf8entities($pinf['alliance']));
			array_push($out, utf8entities($pinf['aname']));
			if	(!$pr['is_leader'])
			{
				array_push($out, utf8entities(gameAction('getPlayerName', $ainf['leader'])));
				if	(is_null($pinf['a_grade']))
					array_push($out, "-");
				else
				{
					$rkl = gameAction('getAllianceRanks', $pinf['aid']);
					array_push($out, $rkl[$pinf['a_grade']]);
				}
			}

			$fl = gameAction('getAllianceForumsComplete', $pinf['aid']);
			foreach	($fl as $fd)
			{
				$fid = $fd['id'];
				if	(!(in_array($fid, $pr['f_read']) || in_array($fid, $pr['f_mod'])))
					continue;
				$tot = $fd['topics'];
				$unread = $tot - gameAction('getReadTopics', $fid, $pid);
				array_push($out, "$fid#$tot#$unread#".utf8entities($fd['title']));
			}
		}
		else
			array_push($out, "0");

		$pm = gameAction('getAllMessages', $pid, 'IN');
		$pmn = gameAction('getNewMessages', $pid, 'IN');
		$it = gameAction('getAllMessages', $pid, 'INT');
		$itn = gameAction('getNewMessages', $pid, 'INT');

		$rook = !gameAction('isPlayerRestrained', $pid);
		if	($rook)
		{
			$rol = gameAction('getResearchOffers', $pid);
			$now = time(); $nrec = 0; $sentTo = "";
			foreach	($rol as $roffer)
			{
				if	($now - $roffer['time'] > 86400)
					break;
				if	($roffer['type'] == "S")
					$sentTo = utf8entities($roffer['player']);
				elseif	($roffer['status'] == "P")
					$nrec ++;
			}
		}
		else
			$nrec=$sentTo="";

		array_push($out, join('#', gameAction('getDiploSummary', $pid)));
		array_push($out, "$pm#$pmn#$it#$itn");
		array_push($out, ($rook ? 1 : 0) . "#$nrec#$sentTo");

		return	join("\n", $out);
	}

	function	handle($input)
	{
		$this->data = $this->getInformation();
		$this->output = "diplomacy";
	}
}

?>

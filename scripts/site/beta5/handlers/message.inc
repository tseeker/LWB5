<?php

class page_handler {
	var	$engines = array('page', 'rpc', 'css', 'js', 'redirect');
	var	$defaultEngine = 'page';

	var	$needsAuth = true;
	var	$ajaxPOST = true;
	var	$ajax = array(
			"func" => array(
				"sendMessage", "setSortParameters", "setMessagesPerPage",
				"getFolderSettings", "getMessageList", "getTargetName",
				"getMessageText", "updateFolders", "deleteMessages",
				"moveMessages", "renameFolder", "addFolder", "deleteFolders",
				"flushFolders", "switchThreaded"
			),
			"init"	=> "makeMessagesTooltips();\ninitMessages();"
		);


	function	getFolderSettings($n, $cid)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return	"";

		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return	"";

		$pn = $n;
		if	($n == "CUS")
			$pn .= "!$cid";
		$rs = "";
		$p = prefs::get("beta5/$pn!nmsg");
		$rs .= ($p == "" ? 20 : $p) . "#";
		$p = prefs::get("beta5/$pn!orderby");
		$rs .= ($p == "" ? "date" : $p) . "#";
		$p = prefs::get("beta5/$pn!direction");
		$rs .= ($p == "" ? 0 : $p) . "#";
		$p = prefs::get("beta5/$pn!threaded");
		$rs .= ($p == "" ? 1 : $p);

		return	$rs;
	}


	function	getMessageList($n, $cid, $alst)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return	"";

		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return	"";

		$list = gameAction('getMessageHeaders', $pid, $n, $cid);
		$s = "";

		$al = array();
		if	(trim($alst) != "")
		{
			$alc = explode('#', $alst);
			$cmid = array_keys($list);
			foreach	($alc as $mid)
			{
				if	($mid == "" || (int)$mid != $mid)
					continue;
				array_push($al, $mid);
				if	(in_array($mid, $cmid))
					continue;
				if	($s != "")
					$s .= "\n";
				$s .= "-#$mid";
			}
		}

		foreach	($list as $id => $hdr)
		{
			if	(in_array($id, $al))
				continue;
			if	($s != "")
				$s .= "\n";
			$s .= "+#$id#".$hdr['received']."#".$hdr['status']."#";
			$s .= $hdr['slink']."#".$hdr['rlink'].'#'.$hdr['replink'];
			$s .= "#" . $hdr['replyTo'] . "\n";
			$s .= $hdr['from'] . "\n";
			$s .= $hdr['to'] . "\n";
			$s .= $hdr['subject'];
		}

		return	$s;
	}


	function	sendMessage($t, $rec, $sub, $msg, $replyId)
	{
		$e = false;
		$errs = array(0,0,0);
		$rec = trim($rec);
		$sub = trim($sub);
		$msg = trim($msg);

		if	($rec == "")
		{
			$errs['rec'] = 1;
			$e = true;
		}
		elseif	($t == 0)
		{
			$rid = gameAction('getPlayer', $rec);
			if	(is_null($rid))
			{
				$errs[0] = 2;
				$e = true;
			}
			else
				$sendAction = 'Player';
		}
		elseif	($t == 1)
		{
			$pinf = gameAction('getPlanetByName', $rec);
			if	(is_null($pinf))
			{
				$errs[0] = 2;
				$e = true;
			}
			else
			{
				$rid = $pinf['id'];
				$sendAction = 'Planet';
			}
		}
		else
		{
			$rid = gameAction('getAlliance', $rec);
			$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);
			if	(is_null($rid))
			{
				$errs[0] = 2;
				$e = true;
			}
			elseif	($rid == $pinf['aid'])
				$sendAction = 'Alliance';
			else
				$sendAction = 'Diplomacy';
		}

		if	($sub == "")
		{
			$errs[1] = 1;
			$e = true;
		}
		elseif	(strlen($sub) > 64)
		{
			$errs[1] = 2;
			$e = true;
		}

		if	($msg == "")
		{
			$errs[2] = 1;
			$e = true;
		}

		if	($replyId == -1)
			$replyId = null;

		if	(!$e)
			gameAction("sendMessage$sendAction", $_SESSION[game::sessName()]['player'], $rid, $sub, $msg, $replyId);

		return	$e ? join('#', $errs) : "OK";
	}

	function	getFolderParms($i)
	{
		$cf = -1;
		$ftype = null;
		switch	($i['f'])
		{
		case	'I':
			$ftype = 'IN';
			break;
		case	'O':
			$ftype = 'OUT';
			break;
		case	'T':
			$ftype = 'INT';
			break;
		case	'C':
			if	(is_null($this->cFolders[$i['cf']]))
				$ftype = null;
			else
			{
				$ftype = 'CUS';
				$cf = $i['cf'];
			}
			break;
		}

		if	(is_null($ftype))
			return	"#-1";

		return	"#$ftype#$cf";
	}

	function	setSortParameters($n, $cid, $se, $so)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return;

		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return;

		$seOk = array('st', 'sub', 'date', 'from', 'to');
		if	(!in_array($se, $seOk))
			return;

		$pn = $n;
		if	($n == "CUS")
			$pn .= "!$cid";
		prefs::set("beta5/$pn!orderby", $se);
		prefs::set("beta5/$pn!direction", ($so=="1")?"1":"0");
	}

	function	setMessagesPerPage($n, $cid, $mpp)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return;

		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return;

		if	($mpp == "" || (int)$mpp != $mpp || $mpp<1 || $mpp>5)
			return;

		$pn = $n;
		if	($n == "CUS")
			$pn .= "!$cid";
		prefs::set("beta5/$pn!nmsg", $mpp * 10);
	}

	function	switchThreaded($n, $cid)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return;

		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return;

		$pn = $n;
		if	($n == "CUS")
			$pn .= "!$cid";
		$p = prefs::get("beta5/$pn!threaded");
		prefs::set("beta5/$pn!threaded", ($p == "1") ? "0" : "1");
	}

	function	getTargetName($type, $id)
	{
		$type = (int)$type;
		$id = (int)$id;
		$rv = "";
		switch	($type) :
		case	0:
			$rv = gameAction('getPlayerName', $id);
			break;
		case	1:
			$pinf = gameAction('getPlanetById', $id);
			if	(!is_null($pinf))
				$rv = $pinf['name'];
			break;
		case	2:
			$pinf = gameAction('getAllianceInfo', $id);
			if	(!is_null($pinf))
				$rv = $pinf['tag'];
			break;
		endswitch;
		return	$rv;
	}

	function	getMessageText($mId)
	{
		$mId = (int)$mId;
		$pl = $_SESSION[game::sessName()]['player'];
		$msg = gameAction('getCompleteMessage', $mId, $pl);
		if	(is_null($mId) || $msg['player'] != $pl)
			return	"";
		$s = $msg['ftype'] . '#' . (is_null($msg['cfid']) ? 0 : $msg['cfid']) . '#' . $msg['id'];
		$s .= "\n" . $msg['subject'];
		if	($msg['text'])
		{
			$s .= "\n" . $msg['text'];
			gameAction('setMessageRead', $mId);
		}
		return	$s;
	}

	function	updateFolders()
	{
		$pl = $_SESSION[game::sessName()]['player'];
		$flist = "IN##" . gameAction('getNewMessages', $pl, 'IN');
		$flist .= "#" . gameAction('getAllMessages', $pl, 'IN') . "\n\n";
		$flist .= "INT##" . gameAction('getNewMessages', $pl, 'INT');
		$flist .= "#" . gameAction('getAllMessages', $pl, 'INT') . "\n\n";
		$flist .= "OUT##" . gameAction('getNewMessages', $pl, 'OUT');
		$flist .= "#" . gameAction('getAllMessages', $pl, 'OUT') . "\n";
		$fl = gameAction('getCustomFolders', $pl);
		foreach	($fl as $id => $name)
		{
			$flist .= "\nCUS#$id#" . gameAction('getNewMessages', $pl, 'CUS', $id);
			$flist .= "#" . gameAction('getAllMessages', $pl, 'CUS', $id);
			$flist .= "\n".utf8entities($name);
		}

		return	$flist;
	}

	function	deleteMessages($n, $cid, $dList, $aList)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return;

		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return;

		$dl = explode("#", $dList);
		foreach	($dl as $mid)
		{
			if	($mid == "" || (int)$mid != $mid)
				continue;
			gameAction('deleteMessage', $mid, $pid);
		}
		return	$this->getMessageList($n, $cid, $aList);
	}

	function	moveMessages($n, $cid, $tn, $tcid, $mList, $aList)
	{
		$nok = array('IN','OUT','INT','CUS');
		if	(!in_array($n, $nok))
			return;
		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		if	($n=="CUS" && is_null($fl[$cid]))
			return;

		if	(	!in_array($tn, $nok)
			||	($tn=="CUS" && is_null($fl[$tcid]))
			)
			return	$this->getMessageList($n, $cid, $aList);

		$dl = explode("#", $mList);
		foreach	($dl as $mid)
		{
			if	($mid == "" || (int)$mid != $mid)
				continue;
			gameAction('moveMessage', $mid, $pid, $tn, $tcid);
		}
		return	$this->getMessageList($n, $cid, $aList);
	}

	function	renameFolder($fid, $name)
	{
		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		$name = trim($name);
		if	(is_null($fl[$fid]) || $name == "" || strlen($name) > 32)
			return	$this->updateFolders();
		gameAction('renameCustomFolder', $fid, $name);
		return	$this->updateFolders();
	}

	function	addFolder($name)
	{
		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		$name = trim($name);
		if	($name == "" || strlen($name) > 32 || count($fl) >= 15 || in_array($name, array_values($fl)))
			return	$this->updateFolders();
		gameAction('createCustomFolder', $pid, $name);
		return	$this->updateFolders();
	}

	function	deleteFolders($lst)
	{
		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		$dl = explode('#', $lst);
		foreach	($dl as $id)
		{
			if	(is_null($fl[$id]))
				continue;
			gameAction('deleteCustomFolder', $pid, $id);
		}
		return	$this->updateFolders();
	}

	function	flushFolders($lst)
	{
		$pid = $_SESSION[game::sessName()]['player'];
		$fl = gameAction('getCustomFolders', $pid);
		$dl = explode('#', $lst);
		$nok = array('IN','OUT','INT','CUS');
		foreach	($dl as $idc)
		{
			list($n, $id) = explode('!', $idc);
			if	(!in_array($n, $nok) || ($n == "CUS" && is_null($fl[$id])))
				continue;
			gameAction('flushFolder', $pid, $n, $id);
		}
		return	$this->updateFolders();
	}

	function	handle($input)
	{
		$pl = $_SESSION[game::sessName()]['player'];
		$this->cFolders = gameAction('getCustomFolders', $pl);

		// Generate complete folder list
		$flist = "IN##" . gameAction('getNewMessages', $pl, 'IN');
		$flist .= "#" . gameAction('getAllMessages', $pl, 'IN') . "\n\n";
		$flist .= "INT##" . gameAction('getNewMessages', $pl, 'INT');
		$flist .= "#" . gameAction('getAllMessages', $pl, 'INT') . "\n\n";
		$flist .= "OUT##" . gameAction('getNewMessages', $pl, 'OUT');
		$flist .= "#" . gameAction('getAllMessages', $pl, 'OUT') . "\n";
		foreach	($this->cFolders as $id => $name)
		{
			$flist .= "\nCUS#$id#" . gameAction('getNewMessages', $pl, 'CUS', $id);
			$flist .= "#" . gameAction('getAllMessages', $pl, 'CUS', $id);
			$flist .= "\n".utf8entities($name);
		}

		switch ($input['a']) :
		case	'c':
			$dInit = "Compose";
			$type = $input['ct'];
			$id = $input['id'];
			if	($type != '' && $id != '')
				$dInit .= "#$type#$id#";
			break;
		case	'f':
			$dInit = "Browse";
			$dInit .= $this->getFolderParms($input);
			break;
		case	'mf':
			$dInit = "Folders";
			break;
		default:
			$dInit = "Compose";
			break;
		endswitch;
		$this->data = array(
			"dinit"	=> $dInit,
			"flist"	=> $flist
		);
		$this->output = "message";
	}

	function redirect($input) {
		if (is_null($_SESSION['userid'])) {
			return "message";
		}

		$this->msgs = $this->game->getLib('beta5/msg');
		$player = $_SESSION[game::sessName()]['player'];

		if ($this->msgs->call('getNew', $player, 'IN')) {
			return "message?a=f&f=I";
		} elseif ($this->msgs->call('getNew', $player, 'INT')) {
			return "message?a=f&f=T";
		}
		l::FIXME("check messages in other folders");
		return "message";
	}
}


?>

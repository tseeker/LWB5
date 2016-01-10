<?php

class page_handler {
	var $needsAuth = true;

	private function logAdm($text) {
		logText("*** ADMIN {$_SESSION['login']} $text");
	}

	function isAdmin() {
		$this->accounts	= $this->game->getLib('main/account');
		return $this->accounts->call('isAdmin', $_SESSION['userid']);
	}


	function manualUpdate() {
		$this->logAdm("is regenerating the manual");
		$manual = $this->game->getLib('main/manual');
		$x = $manual->call('readXMLFile', config::$main['scriptdir'] . '/../manual/beta5/en/main.lwdoc');
		if (is_array($x)) {
			$manual->call('updateSections', $x);
		}
	}


	function handleKickReq(&$input, &$data) {
		if (is_null($input['kr'])) {
			return false;
		} elseif (!is_null($input['cancel'])) {
			return true;
		}

		$data['error'] = 0;
		$data['name'] = trim($input['p']);
		$data['reason'] = trim($input['r']);

		$a = $this->accounts->call('getUser', $data['name']);
		if (is_null($a) || $a['status'] == 'KICKED' || $a['admin'] == 't') {
			$data['error'] = 1;
		} elseif (strlen($data['reason']) < 10) {
			$data['error'] = 2;
		} else {
			$r = $this->accounts->call('requestKick', $_SESSION['userid'], $a['id'], $data['reason']);
			$data['error'] = $r ? 0 : 3;
			if ($r) {
				$this->logAdm("is requesting to kick {$data['name']}");
			}
		}

		return ($data['error'] == 0);
	}


	function handlePNCommand($command, $id) {
		$pd = $this->planets->call('byId', $id);
		if (is_null($pd)) {
			return;
		}

		$pLog = "planet '{$pd['name']}' (#$id)";
		if (!is_null($pd['owner'])) {
			$pLog .= " owned by player {$pd['owner']}";
		}

		if ($command == "v") {
			$this->b5adm->call('validatePlanetName', $id);
			$this->logAdm("validated $pLog");
			$this->db->query("UPDATE credits SET credits_obtained = credits_obtained + 200 WHERE account = {$_SESSION['userid']}");
		} elseif ($command == "r") {
			$this->b5adm->call('resetPlanet', $id, $_SESSION[game::sessName()]['player']);
			$this->logAdm("reset $pLog");
		} elseif ($command == "w") {
			$this->b5adm->call('sendPlanetWarning', $id, $_SESSION[game::sessName()]['player']);
			$this->logAdm("warned $pLog");
			$this->db->query("UPDATE credits SET credits_obtained = credits_obtained + 200 WHERE account = {$_SESSION['userid']}");
		}
	}


	function handlePlanetNames(&$input, &$data) {
		$this->db = $this->game->db;
		if (!is_null($input['pc'])) {
			$this->planets = $this->game->getLib("beta5/planet");
			$this->handlePNCommand($input['pc'], (int) $input['id']);
		}

		list($mode, $q) = $this->b5adm->call('getPlanetsModList', $input['m']);
		if ($input['m'] == 'o') {
			$this->logAdm("is accessing the complete planet list");
		}
		$perPage = 30;
		$lines = dbCount($q);
		$mod = $lines % $perPage;
		$pages = ($lines - $mod) / $perPage + ($mod > 0 ? 1 : 0);

		$page = (int) $input['p'];
		if ($page < 0 || $pages == 0) {
			$page = 0;
		} elseif ($page >= $pages) {
			$page = $pages - 1;
		}

		$data['list'] = array();
		$this->players = $this->game->getLib('beta5/player');
		for ($i = 0; $i < ($page + 1) * $perPage && $i < $lines; $i ++) {
			$r = dbFetchHash($q);
			if ($i >= $page * $perPage) {
				if (!is_null($r['owner'])) {
					$r['oname'] = $this->players->call('getName', $r['owner']);
				}
				array_push($data['list'], $r);
			}
		}

		$data['mode'] = $mode;
		$data['pages'] = $pages;
		$data['page'] = $page;
	}


	function handleKickList(&$input, &$data) {
		if (is_null($input['i']) || is_null($input['a'])) {
			$data['lists'] = $this->accounts->call('getKickList');
			return;
		}

		$kr = $this->accounts->call('getKickRequest', (int)$input['i']);
		if (is_null($kr) || ($kr['requested_by'] == $_SESSION['userid'] && $input['a'] == 1) || $kr['status'] != 'P') {
			return;
		}

		if ($input['a'] == 1) {
			$this->accounts->call('terminate', $kr['to_kick'], 'KICKED', $kr['reason']);
			$this->logAdm("accepted request to kick account #{$kr['to_kick']}");
		}
		$this->accounts->call('kickRequestHandled', $kr['id'], $_SESSION['userid'], $input['a'] == 1);

		$data['lists'] = $this->accounts->call('getKickList');
	}


	function linkMainList() {
		$categories = $this->lib->call('getCategories');
		for ($i=0;$i<count($categories);$i++) {
			$categories[$i]['links'] = $this->lib->call('getLinks', $categories[$i]['id']);
		}

		return array("lklist", $categories);
	}


	function checkCat(&$title, &$desc, $id = null) {
		$title = preg_replace('/\s+/', ' ', trim($title));
		$desc = preg_replace('/\s+/', ' ', trim($desc));
		if (strlen($title) < 5) {
			return 1;
		} elseif (strlen($title) > 64) {
			return 2;
		} elseif (!$this->lib->call('checkCategoryName', $title, $id)) {
			return 3;
		} elseif ($desc != '' && strlen($desc) < 10) {
			return 4;
		}
		return 0;
	}


	function linkMainAddCat(&$input) {
		if ($input['cancel'] != "") {
			return $this->linkMainList();
		}
		if ($input['r'] != 1) {
			return array('lkcat', array());
		}
		$title = $input['title'];
		$desc = $input['desc'];
		$error = $this->checkCat($title, $desc);

		if ($error) {
			return array('lkcat', array(
				'title'	=> $title,
				'desc'	=> $desc,
				'error'	=> $error,
			));
		}
		$this->lib->call('createCategory', $title, ($desc == "" ? null : $desc));
		return $this->linkMainList();
	}


	function linkMainEditCat(&$input) {
		if ($input['cancel'] != "") {
			return $this->linkMainList();
		}

		$id = (int)$input['cid'];
		$cat = $this->lib->call('getCategory', $id);
		if (is_null($cat)) {
			return null;
		}

		if ($input['r'] != 1) {
			return array('lkcat', array(
				'id'	=> $id,
				'title'	=> $cat['title'],
				'desc'	=> $cat['description']
			));
		}

		$title = $input['title'];
		$desc = $input['desc'];
		$error = $this->checkCat($title, $desc, $id);

		if ($error) {
			return array('lkcat', array(
				'id'	=> $id,
				'title'	=> $title,
				'desc'	=> $desc,
				'error'	=> $error,
			));
		}
		$this->lib->call('changeCategory', $id, $title, $desc);
		return $this->linkMainList();
	}


	function linkMainDelCat(&$input) {
		$id = (int)$input['cid'];
		$cat = $this->lib->call('getCategory', $id);
		if (!is_null($cat)) {
			$this->lib->call('deleteCategory', $id);
		}
		return $this->linkMainList();
	}


	function linkMainMoveCat($id, $up) {
		$cat = $this->lib->call('getCategory', $id);
		if (!is_null($cat)) {
			$this->lib->call('moveCategory', $id, $up);
		}
		return $this->linkMainList();
	}


	function checkLink(&$title, &$url, &$desc, $id = null) {
		$title = preg_replace('/\s+/', ' ', trim($title));
		$url = preg_replace('/\s+/', ' ', trim($url));
		$desc = preg_replace('/\s+/', ' ', trim($desc));
		if (strlen($title) < 5) {
			return 1;
		} elseif (strlen($title) > 64) {
			return 2;
		} elseif ($desc != '' && strlen($desc) < 10) {
			return 6;
		} elseif (!preg_match('/^(http|https):\/\/(([A-Z0-9][A-Z0-9_-]*)(\.[A-Z0-9][A-Z0-9_-]*)+)(:(\d+))?(\/.*)?$/i', $url, $m)) {
			return 3;
		} else {
			list($junk, $proto, $hostname, $port) = $m;
			if (!preg_match('/^\d+\.\d+\.\d+\.\d+$/', $hostname)) {
				$ip = gethostbyname($hostname);
				if ($ip === $hostname) {
					return 4;
				}
			}
			if (!$this->lib->call('checkLink', $url, $id)) {
				return 5;
			}
		}
		return 0;
	}


	function linkMainAddLink(&$input) {
		if ($input['cancel'] != "") {
			return $this->linkMainList();
		}

		$cid = (int) $input['cid'];
		$cat = $this->lib->call('getCategory', $cid);
		if (is_null($cat)) {
			return $this->linkMainList();
		}

		if ($input['r'] != 1) {
			return array('lklk', array(
				'cid'	=> $cid
			));
		}
		$title = $input['title'];
		$url = $input['url'];
		$desc = $input['desc'];
		$error = $this->checkLink($title, $url, $desc);

		if ($error) {
			return array('lklk', array(
				'cid'	=> $cid,
				'title'	=> $title,
				'url'	=> $url,
				'desc'	=> $desc,
				'error'	=> $error,
			));
		}
		$this->lib->call('addLink', $cid, $title, ($desc == "" ? null : $desc), $url);
		return $this->linkMainList();
	}


	function linkMainEditLink(&$input) {
		if ($input['cancel'] != "") {
			return $this->linkMainList();
		}

		$lid = (int) $input['lid'];
		$lnk = $this->lib->call('getLink', $lid);
		if (is_null($lnk)) {
			return $this->linkMainList();
		}

		if ($input['r'] != 1) {
			return array('lklk', array(
				'id'	=> $lid,
				'title'	=> $lnk['title'],
				'url'	=> $lnk['url'],
				'desc'	=> $lnk['description']
			));
		}

		$title = $input['title'];
		$url = $input['url'];
		$desc = $input['desc'];
		$error = $this->checkLink($title, $url, $desc, $lid);

		if ($error) {
			return array('lklk', array(
				'id'	=> $lid,
				'title'	=> $title,
				'url'	=> $url,
				'desc'	=> $desc,
				'error'	=> $error,
			));
		}
		$this->lib->call('changeLink', $lid, $title, $url, ($desc == "" ? null : $desc));
		return $this->linkMainList();
	}


	function linkMainDelLink(&$input) {
		$id = (int)$input['lid'];
		$lnk = $this->lib->call('getLink', $id);
		if (!is_null($lnk)) {
			$this->lib->call('deleteLink', $id);
		}
		return $this->linkMainList();
	}


	function linkMain(&$input) {
		$this->lib = $this->game->getLib('main/links');
		switch ($input['ac']) :
			case '0': return $this->linkMainAddCat($input);
			case '1': return $this->linkMainAddLink($input);
			case '2': return $this->linkMainEditCat($input);
			case '3': return $this->linkMainDelCat($input);
			case '4': return $this->linkMainMoveCat((int)$input['cid'], true);
			case '5': return $this->linkMainMoveCat((int)$input['cid'], false);
			case '10': return $this->linkMainEditLink($input);
			case '11': return $this->linkMainDelLink($input);
			default: return $this->linkMainList();
		endswitch;
	}


	function linkReports(&$input) {
		$lib = $this->game->getLib('main/links');
		$acc = $this->game->getLib('main/account');

		if ($input['id'] != '') {
			$id = (int) $input['id'];
			$lnk = $lib->call('getLink', $id);
			if (!is_null($lnk)) {
				if ($input['dl'] == 1) {
					$lib->call('deleteLink', $id);
				} elseif ($input['dr'] == 1) {
					$lib->call('deleteReports', $id);
				}
			}
		}

		$links = array();
		$reports = $lib->call('getBrokenReports');
		foreach ($reports as $rep) {
			if (is_null($links[$rep['link']])) {
				$links[$rep['link']] = $lib->call('getLink', $rep['link']);
				$links[$rep['link']]['category'] = $lib->call('getCategory', $links[$rep['link']]['category']);
				$links[$rep['link']]['reporters'] = array();
			}
			array_push($links[$rep['link']]['reporters'], utf8entities($acc->call('getUserName', $rep['reported_by'])));
		}

		return $links;
	}


	function linkSubmissions(&$input) {
		$lib = $this->game->getLib('main/links');
		$acc = $this->game->getLib('main/account');

		if ($input['su'] != '') {
			if ($input['sid'] == '') {
				$lib->call('deleteSubmissions', $input['su']);
			} elseif ($input['cid'] == '') {
				$sub = $lib->call('getSubmission', $input['su'], (int)$input['sid']);
				if (!is_null($sub)) {
					$cats = $lib->call('getCategories');
					return array('lksadd', array(
						"sub"	=> $sub,
						"cats"	=> $cats
					));
				}
			} elseif ($input['cancel'] == '') {
				$sub = $lib->call('getSubmission', $input['su'], (int)$input['sid']);
				$cat = $lib->call('getCategory', (int)$input['cid']);
				if (!(is_null($sub) || is_null($cat))) {
					$d = $sub['description'];
					$lib->call('addLink', $cat['id'], $sub['title'], $d == "" ? null : $d, $sub['url']);
					$lib->call('deleteSubmissions', $sub['url']);
				}
			}
		}

		$subs = $lib->call('getSubmissions');
		$urls = array();
		foreach ($subs as $sub) {
			if (!is_array($urls[$sub['url']])) {
				$urls[$sub['url']] = array();
			}
			array_push($urls[$sub['url']], array(
				"sid"		=> $sub['submitted_by'],
				"submitter"	=> utf8entities($acc->call('getUserName', $sub['submitted_by'])),
				"title"		=> utf8entities($sub['title']),
				"description"	=> utf8entities($sub['description'])
			));
		}

		return array('lksub', $urls);
	}


	function manageAccounts(&$input) {
		$aName = $input['an'];
		if ($aName == '') {
			return array("account" => '');
		}

		// Get account data
		$account = $this->accounts->call('getUser', $aName);
		if (is_null($account)) {
			return array(
				"account"	=> $aName,
				"notfound"	=> true
			);
		}
		$rv = array(
			"account"		=> $account['name'],
			"status"		=> $account['status'],
			"email"			=> $account['email'],
			"admin"			=> $account['admin'] == 't',
			"conf_code"		=> $account['conf_code'],
			"password"		=> $account['password']
		);

		if ($input['sa'] || $rv['admin']) {
			$this->logAdm("is inspecting account {$account['name']}");
			return $rv;
		}

		$this->db = $this->game->db;
		if ($input['mma'] && strcasecmp($input['ma'], $account['email'])) {
			// Change email address
			$rv['email'] = $nMail = strtolower($input['ma']);
			if (!preg_match('/^[A-Za-z0-9_\.\-\+]+@([A-Za-z0-9_\.\-\+]+)+\.[A-Za-z]{2,6}/', $nMail)) {
				$rv['mailerr'] = 1;
				return $rv;
			}

			$q = $this->db->query("SELECT name FROM main.account WHERE LOWER(email)='"
				. addslashes($nMail) . "'");
			if ($q && dbCount($q)) {
				$rv['mailerr'] = 2;
				list($rv['oacc']) = dbFetchArray($q);
				return $rv;
			} elseif (!$q) {
				$rv['mailerr'] = 3;
				return $rv;
			}

			$q = $this->db->query("UPDATE main.account SET email='" . addslashes($nMail)
				. "' WHERE id={$account['id']}");
			if (!$q) {
				$rv['mailerr'] = 3;
			} else {
				$this->logAdm("changed email for account {$account['name']}");
				$rv['mailerr'] = -1;
			}
			return $rv;
		} elseif ($input['mpw'] && strcmp($input['pw'], $account['password'])) {
			// Change password
			$rv['password'] = $nPass = strtolower($input['pw']);
			if (strlen($nPass) < 4) {
				$rv['passerr'] = 1;
				return $rv;
			}

			$q = $this->db->query("UPDATE main.account SET password='" . addslashes($nPass)
				. "' WHERE id={$account['id']}");
			if (!$q) {
				$rv['passerr'] = 2;
			} else {
				$this->logAdm("changed password for account {$account['name']}");
				$rv['passerr'] = -1;
			}
			return $rv;
		}

		return $rv;
	}


	private function getSpamFormData($in) {
		$action = is_null($in['e'])
			? (is_null($in['p'])
				? (is_null($in['cc']) ? 'none' : 'cancel')
				: 'preview')
			: 'post';
		$subject = $in['sub'];
		$txt = $in['txt'];
		$allGames = ($in['ag'] == '1');
		return	array($action,$subject,$txt,$allGames);
	}

	private function checkSpamData($s, &$t) {
		$maxNL = 500; $maxNC = 100;

		if (strlen($s) < 2) {
			$e = 1;
		} elseif (strlen($s) > 100) {
			$e = 2;
		} elseif (strlen($t) < 3) {
			$e = 3;
		} else {
			$ot = $t; $nt = ""; $nl = 0;
			while ($ot != '' && $nl < $maxNL) {
				$p = strpos($ot, '\n');
				if ($p !== false && $p < $maxNC) {
					$nt .= substr($ot, 0, $p+1);
					$ot = substr($ot, $p+1);
				} else if (strlen($ot) < $maxNC) {
					$nt .= $ot;
					$ot = "";
				} else {
					$s = substr($ot, 0, $maxNC);
					$p = strrpos($s, ' ');
					$ot = substr($ot, $maxNC);
					$nt .= $s;
					if ($p === false) {
						$nt .= "\n";
					}
				}
				$nl ++;
			}

			if ($nl >= $maxNL) {
				$e = 4;
			} else {
				$t = $nt;
				$e = 0;
			}
		}
		return $e;
	}


	private function handleSpam($input) {
		list($action, $subject, $text, $allGames) = $this->getSpamFormData($input);

		if ($action == 'cancel') {
			return array('main', array());
		}

		$data = array(
			'sub'	=> $subject,
			'txt'	=> $text,
			'prev'	=> '',
			'err'	=> 0,
			'ag'	=> $allGames
		);
		if ($action == 'none') {
			return array('spam', $data);
		}

		$data['err'] = $err = $this->checkSpamData($subject, $text);
		if ($err) {
			return array('spam', $data);
		}

		if ($action == 'preview') {
			$fLib = $this->game->getLib('main/forums');
			$data['prev'] = $fLib->call('substitute', $text, 't', 'f');
			return array('spam', $data);
		}

		// Send the spam
		if ($data['ag']) {
			$this->logAdm("is sending spam '$subject' in all games");
			foreach (config::getGames() as $game) {
				if ($game->name == 'main') {
					continue;
				}

				$status = $game->status();
				if ($status == 'FINISHED' || $status == 'PRE') {
					continue;
				}

				$aLib = $game->getLib('admin/beta5');
				$aLib->call('sendSpam', $_SESSION['userid'], $subject, $text);
			}
		} else {
			$this->logAdm("is sending spam '$subject' in game {$this->game->text}");
			$aLib = $this->game->getLib('admin/beta5');
			$aLib->call('sendSpam', $_SESSION['userid'], $subject, $text);
		}

		return array('main', array());
	}


	function handle($input) {
		if (!$this->isAdmin()) {
			$this->logAdm("is a rat bastard who tried to use the admin page!");
			$this->output = "rat";
			return;
		}

		$this->b5adm = $this->game->getLib("admin/beta5");

		$this->output = "admin";
		$d = array();
		switch ($input['c']) :
		case 'k':
			if ($this->handleKickReq($input, $d)) {
				$sp = "main";
			} else {
				$sp = "kickreq";
			}
			break;
		case 'kl':
			$this->handleKickList($input, $d);
			$sp = "kicklst";
			break;
		case 'p':
			$this->handlePlanetNames($input, $d);
			$sp = "pnlist";
			break;
		case 'lk':
			list($sp, $d) = $this->linkMain($input);
			break;
		case 'lkr':
			$d = $this->linkReports($input);
			$sp = "lkrep";
			break;
		case 'lks':
			list($sp, $d) = $this->linkSubmissions($input);
			break;
		case 'a':
			$sp = 'acmgmt';
			$d = $this->manageAccounts($input);
			break;
		case 's':
			list($sp, $d) = $this->handleSpam($input);
			break;
		case 'm':
			$this->manualUpdate();
		default:
			$sp = "main";
			$d = array();
			break;
		endswitch;

		$this->data = array(
			"subpage"	=> $sp,
			"data"		=> $d
		);
	}
}

?>

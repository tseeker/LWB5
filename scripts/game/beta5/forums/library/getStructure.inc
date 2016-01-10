<?php

class beta5_forums_getStructure {

	function beta5_forums_getStructure($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->alliance	= $this->lib->game->getLib('beta5/alliance');
		$this->account	= $this->lib->game->getLib('main/account');
		$this->mForums	= $this->lib->game->getLib('main/forums');
	}


	function run($player) {
		$rv = array();
		$ord = 0;
		$pi = $this->players->call('get', $player);
		$isAdm = $this->account->call('isAdmin', $pi['uid']);
		$gAdm = $this->mForums->call('getAdministrator', $pi['uid']);
		$gMod = $this->mForums->call('getModerator', $pi['uid']);

		// Get categories for general forums
		$gCats = $this->mForums->call('getCategories');
		foreach	($gCats as $gc) {
			$rv['G#'.$gc['id']] = array(
				"id"		=> $gc['id'],
				"type"		=> "G",
				"title"		=> $gc['title'],
				"desc"		=> $gc['description'],
				"order"		=> $ord ++,
				"forums"	=> array()
			);
		}

		// Get version-specific general category
		$vCat = $this->mForums->call('getVersionCategory', 'beta5');
		if ($vCat) {
			$rv['G#'.$vCat['id']] = array(
				"id"		=> $vCat['id'],
				"type"		=> "V",
				"title"		=> "Legacy Worlds - Beta 5",
				"desc"		=> $vCat['description'],
				"order"		=> $ord ++,
				"forums"	=> array()
			);
		}

		// Get general forums
		$gCats = array_keys($rv);
		foreach	($gCats as $cid) {
			$rcid = substr($cid,2);
			$rv[$cid]['forums'] = $this->mForums->call('getForums', $rcid);
			$foDiff = 0;
			for ($i=0;$i<count($rv[$cid]['forums']);$i++) {
				if (!$isAdm && $rv[$cid]['forums'][$i]['admin_only']) {
					array_splice($rv[$cid]['forums'], $i, 1);
					$i --;
					$foDiff ++;
					continue;
				}

				$id = $rv[$cid]['forums'][$i]['id'];
				$rv[$cid]['forums'][$i]['unread'] = $rv[$cid]['forums'][$i]['topics'] - $this->mForums->call('getRead', $id, $pi['uid']);
				$rv[$cid]['forums'][$i]['mod'] = in_array($rv[$cid]['id'], $gAdm) || in_array($id, $gMod);
				$rv[$cid]['forums'][$i]['forder'] -= $foDiff;
			}
		}

		// Get alliance forums
		$ap = $this->alliance->call('getPrivileges', $player);
		if (count($ap['f_read']) || count($ap['f_mod'])) {
			$rv['A#'.$pi['aid']] = array(
				"id"		=> $pi['aid'],
				"type"		=> "A",
				"title"		=> $pi['aname'],
				"order"		=> $ord ++,
				"forums"	=> array()
			);

			$fl = $this->alliance->call('getForumsComplete', $pi['aid']);
			$rcid = 'A#'.$pi['aid'];
			foreach	($fl as $f) {
				if (!(in_array($f['id'],$ap['f_read']) || in_array($f['id'],$ap['f_mod']))) {
					continue;
				}
				$f['mod'] = in_array($f['id'],$ap['f_mod']);
				$f['unread'] = $f['topics'] - $this->lib->call('getRead', $f['id'], $player);
				$f['rcid'] = $rcid;
				array_push($rv[$rcid]['forums'],$f);
			}
		}

		return $rv;
	}
}

?>

<?php

class main_account_library {
	var $index	= array(
		'createAccount',
		'getAccounts',
		'getKickList',
		'getLanguage',
		'getQuitCountdown',
		'getUserName',
		'isLeech',
		'log',
		'requestKick',
		'setQuitCountdown',
		'terminate'
	);

	function main_account_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function isOnline($uid) {
		$q = $this->db->query("SELECT COUNT(*) FROM account WHERE id=$uid AND last_login IS NOT NULL AND (last_logout IS NULL OR last_logout<last_login)");
		list($online) = dbFetchArray($q);
		return ($online == 1);
	}

	function lastOnline($userID) {
		$q = $this->db->query("SELECT last_login, last_logout FROM account WHERE id = $userID");
		list($login, $logout) = dbFetchArray($q);

		if (is_null($logout) || $logout < $login) {
			return 0;
		}

		return $logout;
	}


	function cancelQuitCountdown($uid) {
		$this->db->query("UPDATE account SET quit_ts=NULL,reason=NULL WHERE id=$uid");
	}


	function isAdmin($uid) {
		$q = $this->db->query("SELECT admin FROM account WHERE id=$uid");
		if ($q && dbCount($q) == 1) {
			list($a) = dbFetchArray($q);
		} else {
			$a = 'f';
		}
		return ($a == 't');
	}


	function getUser($name) {
		$i = (int) $name;
		if ((string)$i == (string)$name) {
			$qs = "id=$name";
		} else {
			$qs = "LOWER(name)=LOWER('" . addslashes($name) . "')";
		}
		$q = $this->db->query("SELECT * FROM account WHERE $qs");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		return dbFetchHash($q);
	}


	function getKickRequest($kid) {
		$q = $this->db->query("SELECT * FROM adm_kick WHERE id=$kid");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		return dbFetchHash($q);
	}


	function kickRequestHandled($kid, $admin, $accepted) {
		$this->db->query("UPDATE adm_kick SET examined_by=$admin,status='" . ($accepted ? "Y" : "N") . "' WHERE id=$kid");
	}
}

?>

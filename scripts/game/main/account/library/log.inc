<?php

/** This function adds an entry to the account_log table. The $uid parameter
 * indicates the user's database identifier; the $what parameter indicates
 * the action that warrants the log entry. $what can take the following
 * values:
 *
 * [Ii]		User has logged in
 * [Oo]		User has logged out
 * [Cc]		The account has just been created
 * [Vv]		User has validated his account using his confirmation code
 * [Qq]		User has quit the game
 * VS		User has entered vacation mode
 * VE		User has left vacation mode
 */

class main_account_log {

	function main_account_log($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid, $what) {
		if (class_exists('tracking') && !is_null(tracking::$dbId)) {
			$track = tracking::$dbId;
		} else {
			$track = 'NULL';
		}

		if (gettype($_SERVER['REMOTE_ADDR']) == 'NULL') {
			$addr = 'AUTO';
		} else {
			$addr = addslashes($_SERVER['REMOTE_ADDR']);
		}

		switch ($what) :
		case 'I': case 'i':
			$w = 'IN'; break;
		case 'O': case 'o':
			$w = 'OUT'; break;
		case 'C': case 'c':
			$w = 'CREATE'; break;
		case 'V': case 'v':
			$w = 'CONF'; break;
		case 'Q': case 'q':
			$w = 'QUIT'; break;
		case 'Q': case 'q':
			$w = 'QUIT'; break;
		case 'VS': case 'vs':
			$w = 'VSTART'; break;
		case 'VE': case 've':
			$w = 'VEND'; break;
		default:
			return;
		endswitch;

		$this->db->query("INSERT INTO account_log(tracking,account,ip_addr,action) VALUES ($track,$uid,'$addr','$w')");
	}
}

?>

<?php

class main_account_createAccount {

	function main_account_createAccount($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($user, $password, $email, $lang, $pName) {
		$conf = substr(md5(uniqid(rand())), 0, 16);
		$asu = addslashes($user);

		$id = $this->db->query(
			"INSERT INTO account(name,email,password,status,conf_code) "
			. "VALUES('$asu','$email','".addslashes($password)."','NEW','$conf')"
		);
		if (!$id) {
			return false;
		}

		$this->db->query(
			"INSERT INTO pass_change (account, old_pass, new_pass) "
				. "VALUES($id, '', '" . addslashes($password) . "')"
		);
		$this->db->query("INSERT INTO credits (account) VALUES ($id)");

		$this->db->query("INSERT INTO user_preferences VALUES('language','main',$id,'$lang');");
		$this->db->query(
			"INSERT INTO account_log(tracking,account,ip_addr,action) VALUES("
			. tracking::$dbId . ",$id,'".$_SERVER['REMOTE_ADDR']."','CREATE')"
		);

		// Insert the planet in the registration queue for the default game
		$game = config::getDefaultGame();
		$this->db->query(
			"INSERT INTO reg_queue (account, game) "
				. "VALUES ($id, '{$game->name}')"
		);
		$game->getLib()->call('preRegister', $id, $pName);

		$main = $this->lib->game->getLib('main');
		$rv = $main->call('sendMail', "mail-reg.$lang.txt", $email, array(
			"USER"	=> $user,
			"PASS"	=> $password,
			"CCODE"	=> $conf
		));

		// If sending the mail failed, rollback the transaction
		if (!$rv) {
			$this->db->end(true); $this->db->begin();
		}

		return $rv;
	}
}

?>

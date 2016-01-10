<?


/** This action allows an user to get a new password; the user first enters
 * his username and his email address; once this step is completed, and if
 * the mail address matches the one in the database for the account, a
 * confirmation code is generated and an email is sent to the address. The
 * user is then required to enter that code, at which time the password is
 * replaced with a random password and a second email is sent.
 */
class main_lostPassword {

	function main_lostPassword($main) {
		$this->main	= $main;
		$this->db	= $this->main->db;
		$this->lib	= $this->main->getLib();
		$this->accounts	= $this->main->getLib("main/account");
	}

	function run($userName, $mailAddress, $confirmationCode) {
		if (is_null($userName)) {
			return false;
		}

		$r = $this->checkAccount($userName, $mailAddress, $confirmationCode);
		if (is_null($r)) {
			return array(
				"error"	=> is_null($confirmationCode) ? 1 : 3,
				"name"	=> $userName,
				"mail"	=> $mailAddress,
				"code"	=> $confirmationCode
			);
		}

		list($accountId, $userName, $realCode) = $r;

		// No confirmation code
		if (is_null($confirmationCode)) {
			return $this->checkNewForm($accountId, $userName, $mailAddress, $realCode);
		}

		return $this->generatePassword($accountId, $userName, $mailAddress);
	}


	function checkAccount($userName, $mailAddress, $confirmationCode) {
		if (!($this->lib->call('isValidName', $userName) && $this->lib->call('isValidAddress', $mailAddress))) {
			return null;
		}
		if (!(is_null($confirmation) || preg_match('/^[A-Fa-f0-9]{32,32}$/', $confirmationCode))) {
			return null;
		}

		$qs = "SELECT id,name,pw_conf FROM account WHERE status NOT IN ('NEW','KICKED') "
			. "AND name='" . addslashes($userName) . "' AND email='" . addslashes($mailAddress) . "'";
		if (!is_null($confirmationCode)) {
			$qs .= " AND pw_conf='$confirmationCode'";
		}
		$q = $this->db->query($qs);
		if (!($q && dbCount($q) == 1)) {
			logText("main::actions::lostPassword() failed to authenticate account '$userName'", LOG_WARNING);
			return null;
		}

		return dbFetchArray($q);
	}


	function checkNewForm($id, $name, $mail, $code) {
		$rc = 4;
		if (is_null($code)) {
			// No code created yet; generate one and send the mail
			$code = $this->createConfirmationCode($id, $name, $mail);
			if (is_null($code)) {
				$rc = 2;
			}
		}
		return array(
			"error"	=> $rc,
			"name"	=> $name,
			"mail"	=> $mail,
			"code"	=> null
		);
	}


	function createConfirmationCode($id, $name, $mail) {
		$conf = substr(md5(uniqid(rand())), 0, 16);
		$this->db->query("UPDATE account SET pw_conf='$conf' WHERE id='$id'");

		$lang = $this->accounts->call('getLanguage', $id);
		$rv = $this->lib->call('sendMail', "mail-change-pass-conf.$lang.txt", $mail, array(
			"USER"	=> $name,
			"CODE"	=> $conf
		));

		if (!$rv) {
			$this->db->end(true); $this->db->start();
			return null;
		}

		return $conf;
	}


	function generatePassword($id, $name, $mail) {
		$newPass = "";
		$chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+/*_@=#~&!";
		for ($i=0;$i<16;$i++) {
			do {
				$nc = $chars{rand(0,strlen($chars)-1)};
			} while (strstr($newPass, $nc) !== false);
			$newPass .= $nc;
		}

		$this->db->query("UPDATE account SET pw_conf=NULL,password='$newPass' WHERE id=$id");

		$lang = $this->accounts->call('getLanguage', $id);
		$rv = $this->lib->call('sendMail', "mail-change-pass.$lang.txt", $mail, array(
			"USER"	=> $name,
			"PASS"	=> $newPass
		));

		if (!$rv) {
			$this->db->end(true); $this->db->start();
			return array(
				"error"	=> 5,
				"name"	=> $name,
				"mail"	=> $mail,
				"code"	=> null
			);
		}

		return true;
	}
}


?>

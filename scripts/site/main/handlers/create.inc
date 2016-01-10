<?php

class page_handler {

	function checkUsername($n) {
		$this->data['username'] = $n;
		if (strlen($n) > 15) {
			return 1;
		}
		if (preg_match('/[^A-Za-z0-9_\.\-\+@\/'."'".' ]/', $n)) {
			return 2;
		}
		if (trim($n) != $n) {
			return 3;
		}
		if (preg_match('/\s\s+/', $n)) {
			return 4;
		}
		if (strlen($n) < 2) {
			return 5;
		}
		if (!preg_match('/[A-Za-z]/', $n)) {
			return 6;
		}
		$q = dbQuery("SELECT id FROM account WHERE LOWER(name)='" . addslashes(strtolower($n)) . "'");
		if (dbCount($q)) {
			return 7;
		}
		return 0;
	}

	function checkMailAddr($a) {
		return	preg_match(
			'/^[A-Za-z0-9_\.\-\+]+@([A-Za-z0-9_\.\-\+]+)+\.[A-Za-z]{2,6}/',
			$a
		);
	}

	function checkMail($a1, $a2) {
		$this->data['mail'] = $a1;
		if	($a1 != $a2)
			return	1;
		if	(!$this->checkMailAddr($a1))
			return	2;
		$this->data['mail2'] = $a1;

		$q = dbQuery("SELECT id,status FROM account WHERE LOWER(email) = LOWER('$a1')");
		if (!dbCount($q)) {
			return	0;
		}
		list($id,$status) = dbFetchArray($q);
		if ($status == 'KICKED') {
			dbQuery("INSERT INTO banned_attempt (ip_addr) VALUES ('{$_SERVER['REMOTE_ADDR']}')");
			tracking::$data['bat'] = true;
			tracking::$data['uid'] = $id;
			return	-1;
		} else {
			return	3;
		}
		return	0;
	}

	function checkPassword($np, $cp) {
		if	($np != $cp)
			return	1;
		elseif	(strlen($np) < 4)
			return	2;
		elseif	(strlen($np) > 64)
			return	3;
		elseif	(strtolower($np) == strtolower($this->data['username']))
			return	4;
		return	0;
	}

	function checkLanguage($l) {
		$pLang = array('en');
		if (!in_array($l, $pLang)) {
			$l = 'en';
		}
		$this->data['lang'] = $l;
		tracking::$data['language'] = $l;
	}

	private function checkPlanetName($name) {
		$game = config::getDefaultGame();
		$this->data['planetname'] = $name;
		return $game->getLib()->call('checkPlanetName', $name);
	}

	function checkData($in) {
		$this->data = array();
		$this->data['err1'] = $this->checkUsername($in['username']);
		$this->data['err2'] = $this->checkMail($in['email'], $in['email2']);
		$this->data['err3'] = $this->checkPassword($in['password'], $in['password2']);
		$this->data['err4'] = $this->checkPlanetName($in['planet']);
		$this->checkLanguage($in['lang']);
		return	(!($this->data['err1']||$this->data['err2']||$this->data['err3']||$this->data['err4']));
	}

	function handle($input) {
		if ($_SESSION['authok']) {
			$this->output = "index";
		} elseif (!tracking::$data['readDisclaimer']) {
			tracking::$data['readDisclaimer'] = true;
			$this->output = "disclaimer";
			$this->data = true;
		} elseif (tracking::$data['bat']) {
			$this->output = "kicked";
		} elseif ($input['create'] == "") {
			$this->output = "create";
			$this->data = array();
		} elseif (!$this->checkData($input)) {
			if (tracking::$data['bat']) {
				$this->output = "kicked";
			} else {
				$this->output = "create";
			}
		} else {
			$vLib = $this->game->getLib('main/account');
			$this->data['success'] = $vLib->call('createAccount', $this->data['username'],
				$input['password'], strtolower($this->data['mail']), $this->data['lang'],
				$this->data['planetname']);
			$this->output = "created";
		}
	}
}

?>

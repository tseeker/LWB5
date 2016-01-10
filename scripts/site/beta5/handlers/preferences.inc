<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array();

	function checkMail($a) {
		return preg_match(
			'/^[A-Za-z0-9_\.\-\+]+@([A-Za-z0-9_\.\-\+]+)+\.[A-Za-z]{2,6}/',
			$a
		);
	}

	function checkPassword($op, $np, $cp) {
		$q = dbQuery("SELECT password FROM account WHERE id=".$_SESSION['userid']);
		if (!$q) {
			$this->passError = 1;
			return;
		}
		list($rop) = dbFetchArray($q);
		if	($rop != $op)
			$this->passError = 2;
		elseif	($np != $cp)
			$this->passError = 3;
		elseif	(strlen($np) < 4)
			$this->passError = 4;
		elseif	(strlen($np) > 64)
			$this->passError = 5;
		elseif	($np == $_SESSION['login'])
			$this->passError = 6;
		else
		{
			$p = addslashes($np);
			$op = addslashes($rop);
			dbQuery(
				"INSERT INTO pass_change (account, old_pass, new_pass) "
					. "VALUES({$_SESSION['userid']}, '$op', '$p')"
			);
			$q = dbQuery("UPDATE account SET password='$p' WHERE id=".$_SESSION['userid']);
			if (!$q) {
				$this->passError = 1;
			}
		}
	}

	function	checkFormData($input)
	{
		$pLang = array('fr', 'en');
		if	(in_array($input['lang'], $pLang))
			prefs::set('main/language', $input['lang']);

		$pCol = array('red','green','blue','yellow','grey','purple');
		if	(in_array($input['col'], $pCol))
			prefs::set('main/colour', $input['col']);

		$themes = array('default','invert','classic','cripes');
		if (in_array($input['thm'], $themes)) {
			prefs::set('beta5/theme', $input['thm']);
			if ($input['thm'] == 'cripes') {
				$input['tt'] = 0;
			}
		}

		if	(preg_match('/^[0-4]$/', $input['fs']))
			prefs::set('main/font_size', $input['fs']);

		if	($this->checkMail($input['mail']))
			dbQuery("UPDATE account SET email='".$input['mail']."' WHERE id=".$_SESSION['userid']);
		else
			$this->mailError = preg_replace('/"/', '&quot;', $input['mail']);

		$tt = (int)$input['tt'];
		if	($tt >= 0 && $tt <= 6)
			prefs::set('main/tooltips', $tt);

		if	(preg_match('/^[1-5]0$/', $input['tpp']))
			prefs::set('main/forums_ntopics', $input['tpp']);
		if	(preg_match('/^[1-5]0$/', $input['mpp']))
			prefs::set('main/forums_nitems', $input['mpp']);
		prefs::set('main/smileys', ($input['gsm'] == "1")?"1":"0");
		prefs::set('main/forum_code', ($input['gft'] == "1")?"1":"0");
		prefs::set('main/forums_threaded', ($input['fdm'] == "1")?"1":"0");
		prefs::set('main/forums_reversed', ($input['fmo'] == "1")?"1":"0");
		
		$fsig = preg_replace('/ +/', ' ', trim($input['fsig']));
		if	(strlen($fsig) <= 255)
			prefs::set('main/forums_sig', $input['fsig']);

		if	($input['opass'] != "")
			$this->checkPassword($input['opass'], $input['npass'], $input['cpass']);
	}

	function handleQuit() {
		if (gameAction('isFinished')) {
			return;
		}
		$pid = $_SESSION[game::sessName()]['player'];
		$pinf = gameAction('getPlayerInfo', $pid);
		if	(!is_null($pinf['qts']))
			gameAction('cancelPlayerQuit', $pid);
		else
			gameAction('setPlayerQuit', $pid);
	}

	function	handle($input)
	{
		if	(!is_null($input['quit']))
			$this->handleQuit();
		elseif	($input['col'] != "")
			$this->checkFormData($input);

		$q = dbQuery("SELECT email FROM account WHERE id=".$_SESSION['userid']);
		list($email) = dbFetchArray($q);
		$pinf = gameAction('getPlayerInfo', $_SESSION[game::sessName()]['player']);

		$this->data = array(
			"lang"	=> getLanguage(),
			"mail"	=> $email,
			"col"	=> prefs::get('main/colour', 'red'),
			"fs"	=> prefs::get('main/font_size', 2),
			"err1"	=> $this->mailError,
			"err2"	=> $this->passError,
			"tpp"	=> prefs::get('main/forums_ntopics', 20),
			"mpp"	=> prefs::get('main/forums_nitems', 20),
			"gsm"	=> (prefs::get('main/smileys', 1) == 1),
			"gft"	=> (prefs::get('main/forum_code', 1) == 1),
			"fdm"	=> (prefs::get('main/forums_threaded', 1) == 1),
			"fmo"	=> (prefs::get('main/forums_reversed', 1) == 1),
			"fsig"	=> prefs::get('main/forums_sig', ""),
			"tt"	=> prefs::get('main/tooltips', 2),
			"thm"	=> prefs::get('beta5/theme', 'default'),
			"quit"	=> $pinf['qts'],
			"name"	=> $this->game->text,
			"lok"	=> !(gameAction('isFinished') || $this->game->params['victory'])
		);

		$this->output = "preferences";
	}
}

?>

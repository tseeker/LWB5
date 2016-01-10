<?php

class page_handler {
	var $needsAuth = false;
	var $ajax = array();
	var $engines = array('page', 'css', 'js', 'xml');

	function accountPage(&$input) {
		foreach (config::getGames() as $game) {
			if ($game->name == 'main') {
				continue;
			}

			$status = $game->status();
			if ($status == 'FINISHED') {
				continue;
			}

			$lib = $game->getLib();
			$pid = $lib->call('doesUserPlay', $_SESSION['userid']);
			if (!is_null($pid)) {
				$a2[$game->name] = $lib->call('getPlayerStatus', $pid);
				array_unshift($a2[$game->name], $game->text);
				array_push($a2[$game->name], $status);
				if ($status == 'ENDING') {
					array_push($a2[$game->name], $game->lastTick());
				} elseif ($status == 'READY') {
					array_push($a2[$game->name], $game->firstTick());
				}
			} elseif ($status != 'PRE') {
				$a1[$game->name] = array(
					$game->text, $lib->call('getPlayerCount'),
					$status,
					$status != 'VICTORY' && $lib->call('canJoin')
				);
				if ($status == 'ENDING') {
					array_push($a1[$game->name], $game->lastTick());
				} elseif ($status == 'READY') {
					array_push($a1[$game->name], $game->firstTick());
				}
			}
		}

		if	($input['sw'] != "") {
			$_SESSION['show_unregistered'] = !$_SESSION['show_unregistered'];
		}

		// Get the quit timestamp
		$quit = $this->aLib->call('getQuitCountdown', $_SESSION['userid']);

		// Get data regarding vacation mode
		if (is_null($quit)) {
			$vacation = $this->vLib->call('getStatus', $_SESSION['userid']);
			if (is_null($vacation)) {
				$vacation = array(
					"status"	=> 'VAC',
					"vac_start"	=> null,
					"vac_credits"	=> 1
				);
			}
			$vacation['can_set'] = ($vacation['status'] != 'VAC') && is_null($vacation['vac_start'])
				&& $this->vLib->call('canSet', $_SESSION['userid']);
		} else {
			$vacation = null;
		}

		$this->data = array(
			"other"	=> $a1,
			"play"	=> $a2,
			"vac"	=> $vacation,
			"quit"	=> $quit,
			"leech"	=> $this->aLib->call('isLeech', $_SESSION['userid'])
		);
		$this->output = "account";
	}

	function exitVacation(&$input) {
		if ($this->vLib->call('isOnVacation', $_SESSION['userid'])) {
			$this->output = "vac_leave";
		} else {
			$this->accountPage($input);
		}
	}

	function actualExitVacation(&$input) {
		if (!$input['cancel'] && $this->vLib->call('isOnVacation', $_SESSION['userid'])) {
			$this->vLib->call('leave', $_SESSION['userid']);
		}
		$this->accountPage($input);
	}

	function startVacation(&$input) {
		if ($this->vLib->call('canSet', $_SESSION['userid'])) {
			$this->output = "vac_start";
		} else {
			$this->accountPage($input);
		}
	}

	function actualStartVacation(&$input) {
		if (!$input['cancel'] && $this->vLib->call('canSet', $_SESSION['userid'])) {
			$this->vLib->call('setStart', $_SESSION['userid']);
		}
		$this->accountPage($input);
	}

	function cancelStart(&$input) {
		$vacation = $this->vLib->call('getStatus', $_SESSION['userid']);
		if (is_null($vacation['vac_start'])) {
			$this->accountPage($input);
		} else {
			$this->output = "vac_cancel";
		}
	}

	function actualCancelStart(&$input) {
		if (!$input['cancel']) {
			$vacation = $this->vLib->call('getStatus', $_SESSION['userid']);
			if (!is_null($vacation['vac_start'])) {
				$this->vLib->call('resetStart', $_SESSION['userid']);
			}
		}
		$this->accountPage($input);
	}

	function closeAccount($input) {
		$quit = $this->aLib->call('getQuitCountdown', $_SESSION['userid']);
		if (!is_null($quit)) {
			$this->accountPage($input);
		} else {
			$this->data = array();
			$this->output = "quit_confirm";
		}
	}

	function actualCloseAccount($input) {
		// FIXME: SQL query in handler
		$q = dbQuery("SELECT password FROM account WHERE id={$_SESSION['userid']} AND quit_ts IS NULL");
		if ($input['cancel'] || !($q && dbCount($q) == 1)) {
			$this->accountPage($input);
		} else {
			list($rPass) = dbFetchArray($q);

			$this->data = array(
				"ePass"	=> ($input['q_pass'] != $rPass),
			);
			if ($this->data['ePass']) {
				$this->data['reason'] = $input['q_reason'];
				$this->output = "quit_confirm";
				logText("main/confirm_quit: Account {$_SESSION['userid']} provided wrong password", LOG_WARNING);
			} else {
				$this->aLib->call('setQuitCountdown', $_SESSION['userid'], $input['q_reason']);
				$this->accountPage($input);
			}
		}
	}

	function cancelClose($input) {
		$quit = $this->aLib->call('getQuitCountdown', $_SESSION['userid']);
		if (is_null($quit)) {
			$this->accountPage($input);
		} else {
			$this->data = array();
			$this->output = "back_confirm";
		}
	}

	function actualCancelClose($input) {
		$quit = $this->aLib->call('getQuitCountdown', $_SESSION['userid']);
		if (!$input['cancel'] && !is_null($quit)) {
			$this->aLib->call('cancelQuitCountdown', $_SESSION['userid']);
		}
		$this->accountPage($input);
	}

	function loggedIn(&$input) {
		$this->main = $this->game->getLib();
		$this->vLib = $this->game->getLib("main/vacation");
		$this->aLib = $this->game->getLib("main/account");

		if ($input['evm'] == 1) {
			$this->exitVacation($input);
		} else if ($input['evmc'] == 1) {
			$this->actualExitVacation($input);
		} else if ($input['svm'] == 1) {
			$this->startVacation($input);
		} else if ($input['svmc'] == 1) {
			$this->actualStartVacation($input);
		} else if ($input['cvms'] == 1) {
			$this->cancelStart($input);
		} else if ($input['cvmsc'] == 1) {
			$this->actualCancelStart($input);
		} else if ($input['rq'] == 1) {
			$this->closeAccount($input);
		} else if ($input['rqc'] == 1) {
			$this->actualCloseAccount($input);
		} else if ($input['crq'] == 1) {
			$this->cancelClose($input);
		} else if ($input['crqc'] == 1) {
			$this->actualCancelClose($input);
		} else {
			$this->accountPage($input);
		}
	}


	function xml($input) {
		if (!$_SESSION['authok']) {
			return null;
		}

		$data = new data_node('Games');

		foreach (config::getGames() as $game) {
			if ($game->name == 'main') {
				continue;
			}

			$lib = $game->getLib();
			$pid = $lib->call('doesUserPlay', $_SESSION['userid']);
			if (!is_null($pid)) {
				$node = new data_leaf('Game', utf8_encode($game->text));
				$node->setAttribute('version', $game->version->id);
				$node->setAttribute('path', $game->name);
				$data->addContents($node);
			}
		}

		return $data;
	}


	function handle($input) {
		if ($_SESSION['authok']) {
			$this->loggedIn($input);
		} else {
			$this->output = "index";
		}
	}
}

?>

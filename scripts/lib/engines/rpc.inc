<?php

class display_engine {

	function rpcHeader() {
		header("Expires: Thu, 19 Feb 1981 05:00:00 GMT");
		header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
		header("Cache-Control: no-cache, must-revalidate");
		header("Pragma: no-cache");
		header("Content-Type: text/plain; charset=utf-8");
	}


	function maintenance($maintenance) {
		$this->rpcHeader();
		echo "-:m";
	}


	function displayFatalError($code, $i, $i2) {
		$this->rpcHeader();
		echo "-:f:$code";
	}


	function initSession() {
		$create = handler::$h->needsAuth;
		$r = session::handle($create);
		if ($r == 1 && $create) {
			$this->needsLogin();
		} elseif ($r == 2 && $create) {
			l::debug("(RPC engine) tracknew? " . (tracking::$new ? "yes" : "no"));
			l::fatal(12);
		}
	}


	function sendKick($reason) {
		$this->rpcHeader();
		echo "-:k:$reason";
		endRequest();
	}


	function needsLogin() {
		$this->rpcHeader();
		echo "-:a";
		endRequest();
	}


	function checkAuth() {
		if (!handler::$h->needsAuth) {
			return;
		}

		if (tracking::$data['bat']) {
			// User tried to log in after being kicked
			session::kill();

			$q = dbQuery("SELECT reason FROM account WHERE id=" . tracking::$data['uid'] . " AND status='KICKED'");
			if ($q && dbCount($q) == 1) {
				list($reason) = dbFetchArray($q);
			} else {
				$reason = "";
			}
			$this->sendKick($reason);
		} elseif ($_SESSION['login'] != "") {
			$q = dbQuery("SELECT status,reason FROM account WHERE id={$_SESSION['userid']}");

			if (!($q && dbCount($q) == 1)) {
				$this->needsLogin();
			}

			list($status,$reason) = dbFetchArray($q);

			if ($status == "STD" || $status == "VAC") {
				$this->checkGameRegistration();
				return;
			}

			if ($status == 'KICKED') {
				session::kill();
				$this->sendKick($reason);
			}

			$this->rpcHeader();
			echo "-:";
			endRequest();
		} else {
			$this->needsLogin();
		}
	}

	private function checkGameRegistration() {
		if (input::$path == "main") {
			return;
		}

		$game = input::$game;
		if (!($game->status() == 'PRE' || $game->status() == 'FINISHED')) {
			$lib = $game->getLib();
			$pid = $lib->call("doesUserPlay", $_SESSION['userid']);
			if (!is_null($pid)) {
				if (!is_array($_SESSION["{$game->name}_data"])) {
					$_SESSION["{$game->name}_data"] = array("player" => $pid);
				}
				dbQuery("UPDATE main.credits SET resources_used = resources_used + 1 WHERE account = {$_SESSION['userid']}");
				return;
			}
		}

		$this->rpcHeader();
		echo "-:";
		endRequest();
	}

	function handleInput() {
		// Get arguments and function name
		$this->func = preg_replace('/[^A-Za-z0-9_]/', '', input::$input['rs']);
		$this->args = empty(input::$input['rsargs']) ? array() : input::$input['rsargs'];
	}


	function initRPC() {
		ajax::init();
	}


	function outputData() {
		if (is_array($_SESSION) && $_SESSION['authok']) {
			if (is_null($_SESSION['last_page'])) {
				$_SESSION['last_page'] = time();
			} else if (time() - $_SESSION['last_page'] >= 7200) {
				l::notice("Player '{$_SESSION['login']}' (#{$_SESSION['userid']}) is idling - forcing log out");
				session::kill();
				$this->needsLogin();
			}
		}
		$this->rpcHeader();

		$page = input::$page;
		if (ajax::canCall($this->func)) {
			$res = ajax::call($this->func, $this->args);

			// Trace activity
/* FIXME: Disabled for now, should be somewhere else.
			$spyOn = array();
			if (in_array($_SESSION['userid'], $spyOn)) {
				logText("*** TRACE ({$_SESSION['userid']}:{$_SESSION['login']}) RPC: {$page}::{$this->func} called, arguments:");
				foreach ($this->args as $arg) {
					logText("*** TRACE ({$_SESSION['userid']}:{$_SESSION['login']}) RPC:   -> $arg");
				}
				logText("*** TRACE ({$_SESSION['userid']}:{$_SESSION['login']}) RPC: returning $res");
			}
*/

			echo "+:$res";
		} else {
			l::notice("RPC: unknown function call {$page}::{$this->func}");
			l::debug("Referer: " . $_SERVER['HTTP_REFERER']);
			echo "-:c:{$this->func}";
		}
	}
}



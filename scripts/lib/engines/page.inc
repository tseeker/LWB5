<?php

// Standard page output engine

class display_engine {

	public static $version;

	function maintenance($maintenance) {
		include(config::$main['scriptdir'] . "/site/main/maintenance.inc");
	}


	function displayFatalError($errno, $errorText, $information) {
		ob_end_clean();
		ob_start();
?>
<html>
 <head>
  <title>Legacy Worlds</title>
 </head>
 <body>
  <p>
   <b>The LegacyWorlds game engine encountered a fatal error:</b><br/>
   <?=$errorText?><br/>
   Please report this error by sending an e-mail to the staff at
   <a href='mailto:webmaster@legacyworlds.com'>webmaster@legacyworlds.com</a>
   explaining what happened. Please make sure you include information such
   as the name of the Web browser you're using, the address of the page on
   which you got this message, etc ... The more information you give us, the
   better we'll be able to fix the problem.
  </p>
 </body>
</html>
<?php
		$contents = ob_get_contents();
		ob_end_clean();
		return $contents;
	}


	function displayLogin($default = "", $error = false) {
		input::$path = 'main';
		input::$page = 'login';
		$this->outputPage('main', 'login', array('login' => $default, 'error' => $error));
		endRequest();
	}


	function displayConfirm($error = false) {
		input::$path = 'main';
		input::$page = 'confirm';
		$this->outputPage('main', 'confirm', array('error' => $error));
		endRequest();
	}

	function displayKicked() {
		input::$path = 'main';
		input::$page = 'kicked';
		$this->outputPage('main', 'kicked', null);
		endRequest();
	}


	function initSession() {
		$create = handler::$h->needsAuth || input::$input['userlogin'] == 1 || input::$input['authcode'] != '' || input::$input['ac_restart'] != "";
		$r = session::handle($create);
		if ($r == 1 && $create) {
			$this->displayLogin();
		} elseif ($r == 2 && $create) {
			l::trace("tracknew? " . (tracking::$new ? "yes" : "no"));
			l::fatal(12);
		}
	}


	function checkGameRegistration() {
		if (input::$path == "main") {
			return;
		}

		$game = input::$game;
		if ($game->status() == 'PRE' || $game->status() == 'FINISHED') {
			input::$path = 'main';
			input::$page = 'notfound';
			$this->outputPage('main', 'notfound', array());
			endRequest();
		}

		$lib = $game->getLib();
		$pid = $lib->call("doesUserPlay", $_SESSION['userid']);
		if (is_null($pid)) {
			input::$path = 'main';
			input::$page = 'notregistered';
			$this->outputPage('main', 'notregistered', array(
				"id" => $game->name,
				"name" => $game->text
			));
			endRequest();
		}
		if (!is_array($_SESSION["{$game->name}_data"])) {
			$_SESSION["{$game->name}_data"] = array("player" => $pid);
		}
		dbQuery("UPDATE main.credits SET resources_used = resources_used + 1 WHERE account = {$_SESSION['userid']}");
	}

	function checkCredits() {
		if (! is_null($_SESSION['annoy_until'])) {
			if (time() >= $_SESSION['annoy_until']) {
				$_SESSION['annoy_until'] = null;
				return;
			}
			$this->outputPage('main', 'annoy', array(
				"time" => $_SESSION['annoy_until'] - time()
			));
			endRequest();
		}

		$q = dbQuery("SELECT resources_used, credits_obtained FROM credits WHERE account = {$_SESSION['userid']}");
		list($used, $cred) = dbFetchArray($q);
		if ($used < $cred) {
			return;
		}

		$time = min(60, round(5 + ($used - $cred) / 1000));
		$_SESSION['annoy_until'] = time() + $time;

		$this->outputPage('main', 'annoy', array(
			"time" => $time + 1
		));
		endRequest();
	}

	function checkConfirmationCode($code) {
		$q = "SELECT status,conf_code FROM account WHERE id=" . $_SESSION['userid'];
		$qr = dbQuery($q);

		if (!$qr || dbCount($qr) != 1) {
			l::warn("Reading the {$_SESSION['userid']} account's confirmation code failed");
			killSession();
			$this->displayLogin();
		}

		list($status, $cc) = dbFetchArray($qr);
		if ($cc != strtolower($code)) {
			l::notice("Account {$_SESSION['userid']} entered an invalid confirmation code");
			$this->displayConfirm(true);
		}

		// Validate account
		dbQuery("UPDATE account SET conf_code=NULL,status='STD' WHERE id={$_SESSION['userid']}");
		$_SESSION['authok'] = true;
		accountLog('v', $_SESSION['userid']);

		if ($status == 'NEW') {
			// New accounts -> register to default game
			$main = config::getGame('main');
			$game = $main->getLib()->call('preJoin', $_SESSION['userid']);
			input::$path = 'main';
			input::$page = 'play';
			if (is_null($game)) {
				$this->outputPage('main', 'play', 2);
			} else {
				$this->outputPage('main', 'play', array('registered' => $game));
			}
			endRequest();
		} else {
			$input = $_SESSION['original_request'];
			$this->checkGameRegistration();
		}
	}

	function doRestart($uid) {
		$conf = substr(md5(uniqid(rand())), 0, 16);

		$q = dbQuery("SELECT name,email FROM account WHERE id='$uid' AND conf_code IS NULL");
		if (!($q && dbCount($q) == 1)) {
			return false;
		}
		list($u, $e) = dbFetchArray($q);

		if (!dbQuery("UPDATE account SET conf_code='$conf' WHERE id='$uid'")) {
			return false;
		}

		dbQuery(
			"INSERT INTO account_log(tracking,account,ip_addr,action) VALUES("
			. tracking::$dbId . ",$uid,'".$_SERVER['REMOTE_ADDR']."','CREATE')"
		);

		$mail = @file_get_contents(config::$main['scriptdir'] . "/game/main/mail/mail-restart.en.txt");
		if (is_bool($mail)) {
			return false;
		}
		$tmp = explode("\n", $mail);
		$sub = array_shift($tmp);
		$mail = preg_replace(
			array('/_USER_/', '/_CCODE_/'),
			array($u, $conf), join("\n", $tmp)
		);
		$hdr = "From: webmaster@legacyworlds.com\r\n"
			. "Reply-To: webmaster@legacyworlds.com\r\n"
			. "X-Mailer: LegacyWorlds\r\n"
			. "Mime-Version: 1.0\r\n"
			. "Content-Type: text/plain; charset=utf-8";
		if (!mail($e, $sub, $mail, $hdr)) {
			return false;
		}

		return true;
	}

	function restartAccount($mail) {
		if (is_null(input::$input['ac_restart'])) {
			input::$path = 'main';
			input::$page = 'restart';
			$this->outputPage('main', 'restart', array("email"=>$mail));
			endRequest();
		} else {
			$this->doRestart($_SESSION['userid']);
			$this->displayLogin();
		}
	}

	function accountValidation($loggedIn = false) {
		$q = dbQuery("SELECT status,reason,email,conf_code FROM account WHERE id={$_SESSION['userid']}");

		if (!$q || dbCount($q) != 1) {
			$engine->displayLogin($_SESSION['login'], true);
		}

		list($status,$reason,$mail,$code) = dbFetchArray($q);
		if ($status == 'KICKED') {
			l::notice("Got a login attempt from kicked user {$_SESSION['userid']}; setting banned attempt flag");
			dbQuery("INSERT INTO banned_attempt (ip_addr) VALUES ('{$_SERVER['REMOTE_ADDR']}')");
			tracking::$data['bat'] = true;
			tracking::$data['uid'] = $_SESSION['userid'];
			session::kill();
			$this->displayKicked();
		}

		if ($status == 'STD' || $status == 'VAC') {
			if (! $_SESSION['authok']) {
				$_SESSION['authok'] = true;
				$this->checkCredits();
			} elseif (! is_null($_SESSION['annoy_until'])) {
				$this->checkCredits();
			}
			$this->checkGameRegistration();
			$_SESSION['last_page'] = time();
			return;
		}

		if (is_null(input::$input['authcode']) && !is_null($code)) {
			$_SESSION['original_request'] = input::$input;
			$this->displayConfirm(false);
		} elseif ($loggedIn) {
			session::kill();
			$this->displayLogin();
		} elseif (!is_null(input::$input['authcode']) && !is_null($code)) {
			$this->checkConfirmationCode(input::$input['authcode']);
		} else {
			$this->restartAccount($mail);
		}
	}

	function checkAccountAuth($l, $p) {
		$l1 = addslashes($l); $p1 = addslashes($p);
		$q = "SELECT id,name,password FROM account WHERE lower(name) = lower('$l1')";
		$qr = dbQuery($q);

		if (!$qr || dbCount($qr) != 1) {
			l::info("Account authentication failed, login '".urlencode($l)."' not found");
			if ($qr) {
				l::debug("Query returned " . dbCount($qr) . " row(s)", LOG_DEBUG);
			}
			return false;
		}

		list($uid,$l2,$pw) = dbFetchArray($qr);
		if ($p != $pw) {
			l::info("Account authentication failed, login '".urlencode($l)."' used invalid password");
			l::debug("User ID: $uid ; real user name: $l2", LOG_DEBUG);
			return false;
		}

		// Close existing sessions for the same user
		$q = dbQuery("SELECT id,tracking FROM web_session WHERE account=$uid");
		if ($q && dbCount($q)) {
			list($sid, $tid) = dbFetchArray($q);
			dbQuery(
				"INSERT INTO account_log(tracking,account,ip_addr,action) "
				. "VALUES ($tid,$uid,'AUTO','OUT')"
			);
			dbQuery("DELETE FROM web_session WHERE id=$sid");
		}

		$_SESSION['login'] = $l2;
		$_SESSION['userid'] = $uid;
		l::info("Player '$l2' (#$uid) logged in");
		tracking::$data['previous_login'] = $l;
		session::setAccount($uid);

		accountLog('i', $uid);
		$q = dbQuery("UPDATE account SET last_login=UNIX_TIMESTAMP(NOW()) WHERE id=$uid");
		return true;
	}

	function checkAuth() {
		$create = handler::$h->needsAuth || input::$input['userlogin'] == 1 || input::$input['authcode'] != '' || input::$input['ac_restart'] != "";

		if (!$create) {
			if ($_SESSION['authok']) {
				$this->accountValidation(true);
			}
			return;
		}

		if (tracking::$data['bat']) {
			// User tried to log in after being kicked
			session::kill();
			$this->displayKicked();
		} elseif ($_SESSION['login'] != "") {
			$this->accountValidation();
		} elseif (input::$input['userlogin'] == 1) {
			if ($this->checkAccountAuth(input::$input['login'], input::$input['password'])) {
				input::$input = $_SESSION['original_request'];
				$this->accountValidation();
			} else {
				sleep(5);
				$this->displayLogin(input::$input['login'], true);
			}
		} else {
			$_SESSION['original_request'] = input::$input;
			$this->displayLogin();
		}
	}


	function handleInput() {
		handler::$h->handle(input::$input);
		if (is_null(handler::$h->output)) {
			l::fatal(18, "In handler for ".input::$path." / " . input::$page);
		}
	}


	function initRPC() {
		ajax::init();
	}


	function outputData() {
		$this->outputPage(input::$game->version->id, handler::$h->output, handler::$h->data);
	}

	function outputPage($version, $page, $args) {
		$lang = getLanguage();
		$f = $this->checkOutput($version, $page, $lang);

		self::$version = $version;
		$layoutFile = config::$main['scriptdir'] . "/site/$version/page.inc";
		loader::load($layoutFile, "page_layout");
		$this->layout = new page_layout();

		ob_start();
		$this->layout->header($page, $lang);
		$this->layout->includeFile($f, $args);
		$this->layout->footer($page, $lang);
		$data = ob_get_contents();
		ob_end_clean();

		$this->sendHTTP($data);
	}

	/** This function checks for the presence of a file containing the specified
	 * page in the specified language. It dies with an internal error if the file
	 * isn't found, and returns the file's path otherwise.
	 */
	function checkOutput($version, $page, $lg) {
		$file = config::$main['scriptdir']."/site/$version/output/$page.$lg.inc";
		if (file_exists($file) && is_readable($file) && is_file($file)) {
			return $file;
		}

		$file = config::$main['scriptdir']."/site/$version/output/$page.inc";
		if (!(file_exists($file) && is_readable($file) && is_file($file))) {
			l::fatal(16, "Page was '$page', language was '$lg'");
		}

		return $file;
	}


	function sendHTTP($data) {
		header("Content-Type: text/html; charset=utf-8");
		if (input::$IE || input::$safari) {
			// Don't even bother with the cache; in fact, make sure the cache doesn't interfere
			header("Expires: Thu, 19 Feb 1981 05:00:00 GMT");
			header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
			header("Cache-Control: no-cache, must-revalidate");
			header("Pragma: no-cache");

			// Send the data directly
			endRequest(false);
			echo $data;
			exit(0);
		}

		// Generate a HTML resource from the data
		addRawResource('html', $data);
		$resId = storeResource("html", 21600, false);
		$htmlId = md5($_SERVER['REQUEST_URI']) . ":$resId";

		// Check the tracking cookie for the required contents
		if (!is_array(tracking::$data['htmlId'])) {
			tracking::$data['htmlId'] = array();
		} else {
			$now = time();
			foreach (tracking::$data['htmlId'] as $ck => $t) {
				if ($now - $t > 21600) {
					unset(tracking::$data['htmlId'][$ck]);
				}
			}
		}
		if (!isset(tracking::$data['htmlId'][$htmlId])) {
			tracking::$data['htmlId'][$htmlId] = time();
		}

		// Send the page's header
		$time = tracking::$data['htmlId'][$htmlId];
		$lastModified = substr(date('r', $time), 0, -5).'GMT';
		$etag = '"'.md5($lastModified).'"';
		header("Last-Modified: $lastModified GMT");
		header("ETag: $etag");
		header("Cache-Control: no-cache, must-revalidate");

		// Sends either a 304 reply or the data depending on the browser's will
		$ifModifiedSince = isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) ? stripslashes($_SERVER['HTTP_IF_MODIFIED_SINCE']) : false;
		$ifNoneMatch = isset($_SERVER['HTTP_IF_NONE_MATCH']) ? stripslashes($_SERVER['HTTP_IF_NONE_MATCH']) : false;
		if (($ifNoneMatch || $ifModifiedSince) && (!$ifNoneMatch || $ifNoneMatch == $etag) && (!$ifModifiedSince || $ifModifiedSince != $lastModified)) {
			header('HTTP/1.0 304 Not Modified');
		} else {
			endRequest(false);
			echo $data;
			exit(0);
		}
	}
}


?>

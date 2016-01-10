<?php

class display_engine {

	private $document;
	private $outputMode;
	private $data;

	function __construct() {
		$this->document = new DomDocument('1.0', 'utf-8');
		$this->outputMode = "xml";
	}

	function maintenance($maintenance) {
		$this->data = new data_node('Maintenance');
		$this->data->addContents(new data_leaf('Until', strftime("%Y-%m-%d %H:%M:%S", $maintenance['until'])));
		$this->data->addContents(new data_leaf('Current', gmstrftime("%Y-%m-%d %H:%M:%S")));
		$this->data->addContents(new data_leaf('Reason', $maintenance['reason']));

		$this->rawOutput($this->genOutput('xml'));
	}


	function displayFatalError($code, $i, $i2) {
		$this->data = new data_node('FatalError');
		$this->data->setAttribute('code', $code);
		$this->data->addContents(new data_leaf('Text', $i));

		return $this->genOutput('xml');
	}


	private function loginFailed($code) {
		$this->data = new data_node('Failed');
		$this->data->setAttribute('code', $code);
		$this->rawOutput($this->genOutput('xml'));
	}


	private function kicked($reason) {
		$this->data = new data_node('Kicked');
		$this->data->addContents(new data_leaf('Reason', $reason));
		$this->rawOutput($this->genOutput('xml'));
	}


	function initSession() {
		// Check for "single-shot" request
		if (input::$input['__s'] == 1) {
			$_SESSION = array();
			return;
		}

		// FIXME: handle session, but why bother doing it when the whole thing will be rewritten?
	}


	function checkAuth() {
		// Check authentication directly in "single-shot" mode
		if (input::$input['__s'] == 1) {
			$this->singleShotAuth(input::$input['__l'], input::$input['__p']);
			$this->outputMode = "lw";
			return;
		}

		// FIXME: handle authentication, but why bother doing it when the whole thing will be rewritten?
	}


	private function singleShotAuth($login, $password) {
		// Check the account's login and password
		if (!$this->checkAccountAuth($login, $password)) {
			sleep(5);
			$this->loginFailed(0);
		}

		// Make sure the account is in a valid state
		$q = dbQuery("SELECT status,reason FROM account WHERE id={$_SESSION['userid']}");
		if (!$q || dbCount($q) != 1) {
			$this->loginFailed(1);
		}

		list($status,$reason) = dbFetchArray($q);
		if ($status == 'KICKED') {
			$this->kicked($reason);
		} elseif ($status == 'QUIT' || $status == 'INAC') {
			$this->loginFailed(2);
		} elseif ($status == 'NEW') {
			$this->loginFailed(3);
		}

		$_SESSION['authok'] = true;
		if (!$this->checkVersionRegistration()) {
			$this->loginFailed(4);
		}

		$this->singleShotLog();
	}


	private function checkAccountAuth($l, $p) {
		$l1 = addslashes($l); $p1 = addslashes($p);
		$q = "SELECT id,name,password FROM account WHERE lower(name)=lower('$l1')";
		$qr = dbQuery($q);

		if (!$qr || dbCount($qr) != 1) {
			l::info("Single-shot account authentication failed: login '".urlencode($l)."' not found");
			if ($qr) {
				l::debug("Query returned " . dbCount($qr) . " row(s)", LOG_DEBUG);
			}
			return false;
		}

		list($uid,$l2,$pw) = dbFetchArray($qr);
		if ($p != $pw) {
			l::info("Single-shot account authentication failed: login '"
				. urlencode($l) . "' used invalid password");
			l::debug("User ID: $uid ; real user name: $l2");
			return false;
		}

		$_SESSION['login'] = $l2;
		$_SESSION['userid'] = $uid;

		return true;
	}


	private function checkVersionRegistration() {
		if (input::$path == "main") {
			return true;
		}

		$game = config::getGame(input::$path);
		$lib = $game->getLib();
		$pid = $lib->call("doesUserPlay", $_SESSION['userid']);
		if (is_null($pid)) {
			return false;
		}
		if (!is_array($_SESSION["{$game->name}_data"])) {
			$_SESSION["{$game->name}_data"] = array("player" => $pid);
		}
		return true;
	}


	private function singleShotLog() {
		$uid = $_SESSION['userid'];

		$q = dbQuery("SELECT id FROM web_session WHERE account = $uid");
		if (!($q && dbCount($q))) {
			$in = time();
			$out = $in + 1;
			$q = dbQuery("UPDATE account SET last_login=$in,last_logout=$out WHERE id=$uid");
		}

		dbQuery("INSERT INTO account_log(account,ip_addr,action,t) VALUES ($uid,'AUTO','IN',unix_timestamp(now()) - 1)");
		dbQuery("INSERT INTO account_log(account,ip_addr,action) VALUES ($uid,'AUTO','OUT')");
	}


	function handleInput() {
		$data = handler::$h->xml(input::$input);
		if (!($data instanceof data_gen)) {
			l::fatal(18, "In XML handler for " . input::$path . "/" . input::$page);
		}

		$this->data = $data;
	}


	function initRPC() { /*NO RPC*/ }


	function outputData() {
		$data = $this->genOutput($this->outputMode);

		if (input::$IE || input::$safari || tracking::$disabled) {
			$this->rawOutput($data);
		} else {
			$this->cachedOutput($data);
		}
	}


	private function cachedOutput($data) {
		// Generate an XML resource from the data
		addRawResource('xml', $data);
		$resId = storeResource("xml", 21600, false);
		$xmlId = md5($_SERVER['REQUEST_URI']) . ":$resId";

		// Check the tracking cookie for the required contents
		if (!is_array(tracking::$data['xmlId'])) {
			tracking::$data['xmlId'] = array();
		}
		if (!isset(tracking::$data['xmlId'][$xmlId])) {
			tracking::$data['xmlId'][$xmlId] = time();
		}

		// Send the page's header
		$time = tracking::$data['xmlId'][$xmlId];
		$lastModified = substr(date('r', $time), 0, -5).'GMT';
		$etag = '"'.md5($lastModified).'"';
		header("Content-Type: text/xml; charset=utf-8");
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


	private function rawOutput($data) {
		header("Content-Type: text/xml; charset=utf-8");
		header("Expires: Thu, 19 Feb 1981 05:00:00 GMT");
		header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
		header("Cache-Control: no-cache, must-revalidate");
		header("Pragma: no-cache");

		// Send the data directly
		endRequest(false);
		echo $data;
		exit(0);
	}


	private function genOutput($mode) {
		if ($mode == 'xml') {
			$data = $this->data->toXML($this->document);
		} else {
			$data = $this->data->toLWData($this->document);
		}
		$this->document->appendChild($data);
		return $this->document->saveXML();
	}
}

?>

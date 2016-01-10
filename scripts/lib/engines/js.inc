<?php

class display_engine {

	function maintenance($maintenance) {
		echo "/* Server is under maintenance: " . $maintenance['reason'] . " */";
	}


	function displayFatalError($code, $i, $i2) {
		echo "/* Fatal error $code */";
	}


	function initSession() {
	}


	function checkAuth() {
	}


	function handleInput() {
		$this->id = (int) input::$input['id'];
	}


	function initRPC() {
	}


	function outputData() {
		header("Content-Type: text/javascript");

		if (!is_array(tracking::$data['jsId'])) {
			tracking::$data['jsId'] = array();
		} else {
			$now = time();
			foreach (tracking::$data['jsId'] as $ck => $t) {
				if ($now - $t > 345600) {
					unset(tracking::$data['jsId'][$ck]);
				}
			}
		}

		$jsId = input::$path . "/" . input::$page . "/{$this->id}";
		if (!isset(tracking::$data['jsId'][$jsId])) {
			tracking::$data['jsId'][$jsId] = time();
		}

		$time = tracking::$data['jsId'][$jsId];
		$lastModified = substr(date('r', $time), 0, -5).'GMT';
		$etag = '"'.md5($lastModified).'"';
		header("Last-Modified: $lastModified GMT");
		header("ETag: $etag");
		if (!input::$IE) {
			header("Cache-Control: no-cache, must-revalidate");
		}
		$ifModifiedSince = isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) ? stripslashes($_SERVER['HTTP_IF_MODIFIED_SINCE']) : false;
		$ifNoneMatch = isset($_SERVER['HTTP_IF_NONE_MATCH']) ? stripslashes($_SERVER['HTTP_IF_NONE_MATCH']) : false;
		if (($ifNoneMatch || $ifModifiedSince) && (!$ifNoneMatch || $ifNoneMatch == $etag) && (!$ifModifiedSince || $ifModifiedSince != $lastModified)) {
			header('HTTP/1.0 304 Not Modified');
			return;
		}

		displayResource($this->id, "js");
		l::fatal(30, "JS resource {$this->id}");
	}
}



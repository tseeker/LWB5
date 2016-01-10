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
		header("Content-Type: text/css");

		if (!is_array(tracking::$data['cssId'])) {
			tracking::$data['cssId'] = array();
		} else {
			$now = time();
			foreach (tracking::$data['cssId'] as $ck => $t) {
				if ($now - $t > 345600) {
					unset(tracking::$data['cssId'][$ck]);
				}
			}
		}

		$cssId = input::$path . "/" . input::$page . "/{$this->id}";
		if (!isset(tracking::$data['cssId'][$cssId])) {
			tracking::$data['cssId'][$cssId] = time();
		}

		$time = tracking::$data['cssId'][$cssId];
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

		displayResource($this->id, "css");
		l::fatal(30, "CSS resource {$this->id}");
	}
}



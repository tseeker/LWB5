<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// lib/pcheck.inc
//
// This library allows access to the proxy detector.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------



class pcheck {

	private static $queue = null;
	private static $pid = null;

	public static function isRunning() {
		$fName = config::$main['cs_path'] . "/proxyDetector.pid";
		if (!file_exists($fName)) {
			return false;
		}

		$file = fopen($fName, "r");
		$line = rtrim(fgets($file));
		fclose($file);

		list($pid, $time) = explode(" ", $line);
		if (time() - $time > 22) {
			return false;
		}

		return $pid;
	}

	private static function getQueue() {
		if (! is_null(self::$queue)) {
			return;
		}

		$key = ftok(config::$main['scriptdir'] . "/lib/pcheck_manager.inc", "C");
		if ($key == -1) {
			throw new Exception("Could not create the control queue's key");
		}

		self::$queue = @msg_get_queue($key);
		if (self::$queue === FALSE) {
			self::$queue = null;
			throw new Exception("Could not access the control queue (using key $key)");
		}

		self::$pid = posix_getpid();
	}

	private static function sendRequest($addresses) {
		array_unshift($addresses, self::$pid);
		if (!@msg_send(self::$queue, 1, $addresses, true, false)) {
			throw new Exception("Error while sending request");
		}
	}

	private static function getResults() {
		$wait = 30;
		do {
			$success = @msg_receive(self::$queue, self::$pid, $type, 32768, $result,
				true, MSG_IPC_NOWAIT, $error);

			if (!$success && $error != MSG_ENOMSG) {
				throw new Exception("Error while waiting for results");
			} elseif ($success) {
				if (is_array($result)) {
					return $result;
				} elseif ($result == 'PING') {
					$wait = 30;
				} else {
					throw new Exception("Invalid message received");
				}
			} else {
				sleep(1);
			}

			$wait --;

		} while ($wait);

		throw new Exception("Timeout while waiting for results");
	}

	public static function check($addresses, $force = false) {
		if (!($force || self::isRunning())) {
			throw new Exception("The detector doesn't seem to be running");
		}
		self::getQueue();
		self::sendRequest($addresses);
		return self::getResults();
	}

}


?>

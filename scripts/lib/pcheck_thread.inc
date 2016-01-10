<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// lib/pcheck_thread.inc
//
// This library contains the code of the open proxy detector's scanning
// threads.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class pcheck_thread {

	/** The process ID of the thread.
	 */
	public $pid;

	/** The message queue the thread is bound to. Undefined in the
	 * parent process.
	 */
	private $queue;

	/** This property indicates that the thread has ended.
	 */
	public $ended = false;


	/** The constructor forks (throwing an exception on failure), then
	 * depending on whether it is in the parent process or in the child,
	 * returns or enters the instruction loop.
	 */
	public function __construct($queue) {
		$pid = @pcntl_fork();
		if ($pid == -1) {
			throw new Exception("Unable to fork");
		}

		if ($pid != 0) {
			// In the parent, store the child's PID and get going.
			$this->pid = $pid;
			$this->free = true;
			$this->queue = $queue;
			return;
		}

		// In the child, store our PID and the queue, then starts
		// waiting for instructions.
		$this->pid = posix_getpid();
		$this->queue = $queue;
		$this->waitLoop();
	}


	/** This method is called by the manager to send messages to the
	 * threads.
	 */
	public function send($message) {
		return $this->ended ? false : msg_send($this->queue, $this->pid, $message, true);
	}


	/** This method implements the instruction loop.
	 */
	private function waitLoop() {
		$quit = false;
		$error = false;

		do {
			$success = msg_receive($this->queue, $this->pid, $type, 32768, $message, true);
			if (! $success) {
				l::error("Child failed to receive a message");
				$quit = $error = true;
			} elseif ($message['type'] == 'QUIT') {
				$quit = true;
			} elseif ($message['type'] == 'SCAN') {
				list($host, $port) = $message['scan'];

				$found = $this->executeCheck($host, $port, "GET")
					|| $this->executeCheck($host, $port, "POST");

				if (!msg_send($this->queue, 1, array($this->pid, $host, $found), true)) {
					l::error("Child failed to send message");
					$quit = $error = true;
				}
			} else {
				l::notice("Unknown message {$message['type']} received");
			}
		} while (!$quit);

		exit($error ? 1 : 0);
	}


	/** This method will try to check one port for open proxy software, using
	 * a specific method (POST or GET).
	 */
	private function executeCheck($ipAddress, $port, $method) {
		$key = md5(uniqid(rand()));

		// Open the socket
		$socket = @fsockopen("tcp://$ipAddress", $port, $errno, $errstr, pcheck_manager::$timeout);
		if ($socket === FALSE) {
			return false;
		}

		// Make sure I/O doesn't timeout
		stream_set_timeout($socket, pcheck_manager::$timeout);

		// Send the request
		$result = @fwrite($socket, preg_replace('/__key__/', $key, pcheck_manager::$requests[$method]));
		if ($result !== FALSE) {
			$info = stream_get_meta_data($socket);
			if ($info['timed_out']) {
				$result = false;
			}
		}

		// Get the page
		if ($result !== FALSE) {
			$result = @fread($socket, 4096);
		}
		if ($result !== FALSE) {
			$info = stream_get_meta_data($socket);
			if ($info['timed_out']) {
				$result = false;
			}
		}

		// Close the socket
		@fclose($socket);
		if ($result === FALSE) {
			return false;
		}

		return preg_match("/Key is \\<$key\\>/", $result);
	}
}


?>

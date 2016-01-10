<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// lib/pcheck_manager.inc
//
// This library contains the code of the open proxy detector's main
// thread.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


declare(ticks = 1);	// PHP stupidity


class pcheck_manager {

	/** This property indicates how many of the threads are free.
	 */
	private $nFree;

	/** These properties are accessed by the detection threads to
	 * read their parameters.
	 */
	public static $timeout;
	public static $requests;


	/** This property points to the only current instance of the
	 * proxy detector.
	 */
	private static $instance = null;

	/** This property indicates whether the manager is being run in
	 * debugging mode.
	 */
	private $debug;

	/** This property contains the objects corresponding to each thread.
	 */
	private $threads = array();

	/** This property is set to true when the manager is shutting down.
	 */
	private $ending = false;

	/** Ports to scan on.
	 */
	static private $ports = array(
		80, 81, 1075, 3128, 4480, 5490, 6588, 7033, 8000, 
		8080, 8081, 8085, 8090, 8095, 8100, 8105, 8110
	);


	/** This method sends commands to the FIFO controller. If the manager
	 * is being run in debugging mode, the commands are not sent.
	 */
	private static function sendToControl($command) {
		if (self::$instance->debug) {
			return;
		}
		$pipe = fopen(config::$main['cs_fifo'], "w");
		fwrite($pipe, "$command\n");
		fclose($pipe);
	}


	/** This method is called by the error manager in case a fatal error
	 * prevents the manager from functionning properly.
	 */
	public static function fatalError($errno, $errorText, $information) {
		$instance->shutdown();
		exit(1);
	}


	/** The constructor forks, initialises the message queues, starts
	 * the detector threads then calls waitForInstructions(). It never
	 * returns.
	 */
	public function __construct($debug) {
		self::$instance = $this;
		$this->debug = $debug;

		$this->backgroundProcess();
		$this->initMessageQueues();
		$this->initSignals();
		$this->initData();
		$this->initThreads();
		$this->initCache();

		// Sends our PID to the controller
		self::sendToControl("PCPID " . ($this->pid = posix_getpid()));

		l::notice("Proxy detector initialised");
		l::debug("Timeout: " . self::$timeout . "s; threads: " . count($this->threads));
		l::debug("Using URL {$this->url}");

		$this->mainLoop();

		$this->shutdown();
		exit(0);
	}


	/** This method initialises the manager's signal handlers.
	 */
	private function initSignals() {
		pcntl_signal(SIGTERM, array($this, "terminateHandler"));
		pcntl_signal(SIGINT, array($this, "terminateHandler"));
		pcntl_signal(SIGCHLD, array($this, "threadEndHandler"));
	}


	/** This method handles the TERM and INT signals, which both cause
	 * a clean shutdown.
	 */
	public function terminateHandler($signo) {
		if (! $this->mustEnd) {
			l::notice("Main thread terminating on SIG" . ($signo == SIGTERM ? "TERM" : "INT"));
			$this->mustEnd = true;
		}
	}


	/** This method handles SIGCHLD and takes appropriate measures if
	 * it has been caused by an error.
	 */
	public function threadEndHandler($signo) {
		// Wait for the child processes and stores their IDs
		$ended = array();
		do {
			$pid = pcntl_waitpid(-1, $status, WNOHANG);
			if ($pid > 0) {
				$ended[$pid] = $status;
			}
		} while ($pid > 0);

		foreach ($this->threads as $thread) {
			if (array_key_exists($thread->pid, $ended)) {
				$thread->ended = true;
			}
		}

		if ($this->ending) {
			l::trace("Threads have ended: " . join(', ', array_keys($ended)));
		} else {
			l::notice("Some children have met an untimely end! Terminating.");
			$this->mustEnd = true;
		}
	}


	/** This method causes the proxy checked to be run in the background.
	 */
	private function backgroundProcess() {
		if ($this->debug) {
			return;
		}

		// Fork to the background
		$pid = pcntl_fork();
		if ($pid == -1) {
			l::crit("The open proxy detector failed to start.");
			exit(1);
		} elseif ($pid) {
			exit(0);
		}
		posix_setsid();
	}


	/** This method initialises the message queues: a first queue to be
	 * used as a control channel and on which requests will be received,
	 * and a second queue to communicate with the threads.
	 */
	private function initMessageQueues() {
		// Create the control queue's key
		$ctrlKey = ftok(config::$main['scriptdir'] . "/lib/pcheck_manager.inc", "C");
		if ($ctrlKey == -1) {
			l::crit("Could not create the control queue's key");
			exit(1);
		}

		// Create the thread queue's key
		$thrdKey = ftok(config::$main['scriptdir'] . "/lib/pcheck_manager.inc", "T");
		if ($ctrlKey == -1) {
			l::crit("Could not create the thread queue's key");
			exit(1);
		}

		// Create the control queue
		$ctrlQueue = msg_get_queue($ctrlKey, 0666 | IPC_CREAT);
		if ($ctrlQueue === FALSE) {
			l::crit("Could not create the control queue (using key $ctrlKey)");
			exit(1);
		}

		// Create the thread queue
		$thrdQueue = msg_get_queue($thrdKey, 0600 | IPC_CREAT);
		if ($thrdQueue === FALSE) {
			l::crit("Could not create the thread queue (using key $thrdKey)");
			@msg_remove_queue($ctrlQueue);
			exit(1);
		}

		$this->control = $ctrlQueue;
		$this->threadQueue = $thrdQueue;
	}


	/** This method destroys the queues.
	 */
	private function destroyMsgQueues() {
		@msg_remove_queue($this->control);
		@msg_remove_queue($this->threadQueue);
	}


	/** This method initialises the data used by the proxy detector
	 * threads.
	 */
	private function initData() {
		$serv = config::getParam('pcheck_server');
		$this->url = "http://$serv" . config::getParam('pcheck_path');
		$timeout = (int) config::getParam('pcheck_timeout');

		self::$requests = array(
			"GET"	=> "GET {$this->url}?k=__key__ HTTP/1.0\r\n"
				. "Host: $serv\r\n"
				. "Cache-Control: no-cache\r\n"
				. "Pragma: no-cache\r\n"
				. "User-Agent: OpenCheck 1.0\r\n"
				. "\r\n",
			"POST"	=> "POST {$this->url} HTTP/1.0\r\n"
				. "Host: $serv\r\n"
				. "Cache-Control: no-cache\r\n"
				. "Pragma: no-cache\r\n"
				. "User-Agent: OpenCheck 1.0\r\n"
				. "Content-Length: 34\r\n"
				. "\r\n"
				. "k=__key__\r\n"
		);
		self::$timeout = $timeout;
	}


	/** This method initialises all threads.
	 */
	private function initThreads() {
		$nThreads = (int) config::getParam('pcheck_threads');
		for ($i = 0; $i < $nThreads; $i ++) {
			try {
				$thread = new pcheck_thread($this->threadQueue);
			} catch (Exception $e) {
				l::crit("Thread " . ($i + 1) . " failed to initialise, exiting!");
				$this->shutdown();
				exit(1);
			}
			array_push($this->threads, $thread);
		}
		$this->nFree = $nThreads;
	}

	/** This method shuts down the manager.
	 */
	private function shutdown() {
		$this->ending = true;

		// Kill threads
		foreach ($this->threads as $thread) {
			$thread->send(array("type" => "QUIT"));
		}

		// Wait until all threads have ended
		do {
			$endOk = true;
			foreach ($this->threads as $thread) {
				if (! $thread->ended) {
					$endOk = false;
					sleep(1);
					break;
				}
			}
		} while (!$endOk);

		// Destroy message queues
		$this->destroyMsgQueues();
	}


	/** This method contains the manager's main loop, which handles everything
	 * from receiving requests and sending results to managing the detection
	 * threads.
	 */
	private function mainLoop() {
		$this->requests = array();
		$this->reqHostsFound = array();
		$this->jobsQueue = array();
		$this->jobsData = array();

		$ticker = 0;
		while (!$this->mustEnd) {
			// Check for incoming requests
			$success = msg_receive($this->control, 1, $type, 32768,
				$message, true, MSG_IPC_NOWAIT, $error);
			if (!$success && $error != MSG_ENOMSG) {
				l::error("Manager failed to receive from control queue");
				break;
			} elseif ($success) {
				$this->requestReceived(array_shift($message), $message);
				continue;
			}

			// Check for incoming results from the detection threads
			$success = msg_receive($this->threadQueue, 1, $type, 32768,
				$message, true, MSG_IPC_NOWAIT, $error);
			if (!$success && $error != MSG_ENOMSG) {
				l::error("Manager failed to receive from thread queue");
				break;
			} elseif ($success) {
				// A result has been received
				$this->resultReceived(array_shift($message), $message);
				continue;
			}

			sleep(1);
			$ticker ++;

			// For each request in progress, send a message every 10 ticks
			// to signal the process we're not dead yet
			if ($ticker % 10 == 0 && count($this->requests)) {
				$this->sendPing();
			}

			// Send PID to controller every 20 ticks
			if ($ticker == 20) {
				$ticker = 0;
				self::sendToControl("PCPID {$this->pid}");
				$this->flushCache();
			}
		}
	}


	/** This method handles the reception of a new request.
	 */
	private function requestReceived($fromPID, $hosts) {
		l::debug("Request received from $fromPID");

		$now = time();
		$this->requests[$fromPID] = array();
		$this->reqHostsFound[$fromPID] = 0;
		foreach ($hosts as $host) {
			if (is_array($this->cache[$host]) && $now - $this->cache[$host]['last'] < 86400) {
				// Cached entry found, store result
				$this->requests[$fromPID][$host] = $this->cache[$host]['status'];
				$this->reqHostsFound[$fromPID] ++;
				continue;
			}

			// No cached entry found
			$this->requests[$fromPID][$host] = -2;
			if (is_array($this->jobsData[$host])) {
				// We're already trying to detect this host
				continue;
			}

			// This host needs to be detected
			$this->addToJobsQueue($host);
		}

		if ($this->reqHostsFound[$fromPID] == count($this->requests[$fromPID])) {
			// The request could be satisfied directly from cached data
			$this->sendResponse($fromPID);
		}
	}


	/** This method stores the results of a scan performed by one of the
	 * detection threads.
	 */
	private function resultReceived($fromThread, $result) {
		list($host, $found) = $result;

		// If a proxy was detected, log it
		if ($found) {
			l::info("Found open proxy at $host on port $port");
		}

		// Store the results
		$this->jobsData[$host][1] |= $found;
		$this->jobsData[$host][0] --;
		if ($this->jobsData[$host][0] == 0) {
			$this->hostFinished($host);
		}

		// Increase amount of free threads, set the thread as free
		$this->nFree ++;
		foreach ($this->threads as $thread) {
			if ($thread->pid == $fromThread) {
				$thread->free = true;
				break;
			}
		}

		// Shift the jobs queue
		$this->moveQueue();
	}


	/** This method adds the jobs required to scan a host to the queue.
	 */
	private function addToJobsQueue($host) {
		l::trace("Adding host $host to the queue...");

		$this->jobsData[$host] = array(count(self::$ports), false);
		foreach (self::$ports as $port) {
			array_push($this->jobsQueue, array($host, $port));
		}

		$this->moveQueue();
	}


	/** This method returns the response to a request through the queue.
	 */
	private function sendResponse($requestPID) {
		$request = $this->requests[$requestPID];

		$nRequests = array();
		$nReqHF = array();
		foreach ($this->requests as $id => $data) {
			if ($id != $requestPID) {
				$nRequests[$id] = $data;
				$nReqHF[$id] = $this->reqHostsFound[$id];
			}
		}
		$this->reqHostsFound = $nReqHF;
		$this->requests = $nRequests;

		l::debug("Sending response to process #$requestPID");
		msg_send($this->control, $requestPID, $request, true);
	}


	/** This method sends a "ping" packet to all waiting processes.
	 */
	private function sendPing() {
		l::trace("Pinging processes");
		foreach (array_keys($this->requests) as $id) {
			msg_send($this->control, $id, "PING", true);
		}
	}


	/** This method is called when the processing of a host
	 * is complete.
	 */
	private function hostFinished($host) {
		l::trace("Host scanning finished for $host");
		$result = $this->jobsData[$host][1];

		// Remove the entry from jobsData
		$nData = array();
		foreach ($this->jobsData as $h => $d) {
			if ($h != $host) {
				$nData[$h] = $d;
			}
		}
		$this->jobsData = $nData;

		// Store result in cache
		$this->storeCache($host, $result);

		// Check all requests that contained this host
		$checkRequests = array();
		foreach (array_keys($this->requests) as $request) {
			if (array_key_exists($host, $this->requests[$request])) {
				$this->reqHostsFound[$request] ++;
				$this->requests[$request][$host] = $result ? 1 : 0;
				array_push($checkRequests, $request);
			}
		}

		// For each request that contained the host, check if it's completed
		$finished = array();
		foreach ($checkRequests as $request) {
			if ($this->reqHostsFound[$request] == count($this->requests[$request])) {
				array_push($finished, $request);
			}
		}

		// Send responses to completed requests
		foreach ($finished as $request) {
			$this->sendResponse($request);
		}
	}


	/** This method sends free threads a scanning order if the
	 * jobs queue isn't empty.
	 */
	private function moveQueue() {
		while ($this->nFree > 0 && count($this->jobsQueue)) {
			$job = array_shift($this->jobsQueue);
			foreach ($this->threads as $thread) {
				if ($thread->free) {
					l::trace("Assigning port {$job[1]} at {$job[0]} to thread {$thread->pid}");
					$this->nFree --;
					$thread->free = false;
					$thread->send(array(
						"type"	=> "SCAN",
						"scan"	=> $job
					));
					break;
				}
			}
		}
	}


	/** This method stores the result of a scan in the cache.
	 */
	private function storeCache($host, $result) {
		$this->cache[$host] = array(
			"status"	=> $result ? 1 : 0,
			"last"		=> time()
		);

		$this->cacheModified = time();
	}

	/** This method reads the cache from the database.
	 */
	private function initCache() {
		$this->cacheModified = 0;
		$this->cache = array();

		$success = false;
		do {
			try {
				$db = db::connect();
				$db->enableExceptions();
				$db->query("LOCK TABLE proxy_detector IN ACCESS EXCLUSIVE MODE");
				$cacheRead = new db_copy("proxy_detector");
				$cacheRead->setAccessor($db);
				$cacheRead->execute();
				$db->end();
				$db->close();
				$db->disableExceptions();
				$success = true;
			} catch (Exception $e) {
				l::notice("Could not read cache from database. Will retry in 20 seconds.");
				l::info($e->getMessage());
				if (!is_null($db)) {
					l::trace("Closing database connection");
					$db->close();
					$db->disableExceptions();
				}
				sleep(20);
			}
		} while (! $success);

		for ($i = 0; $i < $cacheRead->rows(); $i ++) {
			$row = $cacheRead->getRow($i);
			$this->cache[$row[0]] = array(
				"last"		=> $row[1],
				"status"	=> $row[2] == 't' ? 1 : 0
			);
		}
	}


	/** This method tries to store the cache's contents in the
	 * database.
	 */
	private function flushCache() {
		if (! $this->cacheModified || time() - $this->cacheModified < 20) {
			return;
		}

		l::debug("Flushing cache to database");
		$db = null;
		try {
			$db = db::connect();
			$db->enableExceptions();
			$db->query("LOCK TABLE proxy_detector IN ACCESS EXCLUSIVE MODE");
			$toWrite = $this->formatCache();
			$toWrite->setAccessor($db);
			$toWrite->execute();
			$db->end();
			$db->close();
			$db->disableExceptions();
		} catch (Exception $e) {
			l::notice("Could not write cache to database.");
			l::info($e->getMessage());
			if (!is_null($db)) {
				l::trace("Closing database connection");
				$db->close();
				$db->disableExceptions();
			}
			return;
		}

		$this->cacheModified = 0;
	}


	/** This method prepares the cache for copy into the database.
	 */
	private function formatCache() {
		$cache = new db_copy("proxy_detector", db_copy::copyToClean);
		$now = time();
		foreach ($this->cache as $host => $data) {
			if ($now - $data['last'] > 3 * 86400) {
				continue;
			}
			$cache->appendRow(array($host, $data['last'], $data['status'] ? 't' : 'f'));
		}
		return $cache;
	}
}


?>

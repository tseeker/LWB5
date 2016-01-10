<?php

declare(ticks = 1);	// PHP stupidity


class tick_manager {
	var $threads	= array();
	var $byId	= array();

	static function fatalError($errno, $errorText, $information) {
		exit(1);
	}

	function tick_manager($debug) {
		$this->debug = $debug;
		if (!$this->debug) {
			// Forks to the background
			$pid = pcntl_fork();
			if ($pid == -1) {
				die("The tick manager failed to launch: fork() failed.\n");
			} elseif ($pid) {
				exit(0);
			}

			// We're in the child, detach from parent as much as possible
			$this->pid = posix_getpid();
			posix_setsid();

			// Tell the system control script that we are running
			if (file_exists(config::$main['cs_fifo'])) {
				$pipe = fopen(config::$main['cs_fifo'], "w");
				fwrite($pipe, "TMPID {$this->pid}\n");
				fclose($pipe);
			}
		}

		if (file_exists(config::$main['cachedir'] . "/stop_ticks")) {
			unlink(config::$main['cachedir'] . "/stop_ticks");
			$this->ticksStopped = true;
			touch(config::$main['cachedir'] . "/ticks_stopped");
		} elseif (file_exists(config::$main['cachedir'] . "/ticks_stopped")) {
			$this->ticksStopped = true;
		} else {
			$this->ticksStopped = false;
		}

		if ($this->ticksStopped) {
			l::warn("Ticks manager starting but ticks are stopped");
		} else {
			l::notice("Ticks manager started");
		}

		l::setFatalHandler('tick_manager::fatalError');
		$this->run();
	}


	function run() {
		$this->setSignals(false);
		$this->setAlarm();
		while (1) {
			// Sleeps for a while
			sleep(10);

			// Check for configuration update
			$this->update();

			// Check last tick
			if (time() - $this->lastTick >= 6) {
				l::error("No ticks for 6+ seconds! Trying to re-schedule!");
				$this->setSignals(false);
				$this->setAlarm();
				$this->lastTick = time();
			}
		}
	}


	function setSignals($mask) {
		$myHandler = array($this, "signalHandler");
		$sigs = array(SIGTERM, SIGINT, SIGCHLD);
		foreach ($sigs as $s) {
			if ($mask) {
				pcntl_signal($s, SIG_IGN);
			} else {
				pcntl_signal($s, $myHandler);
			}
		}

		if ($mask) {
			pcntl_signal(SIGALRM, SIG_IGN);
		} else {
			pcntl_signal(SIGALRM, array($this, "alarm"));
		}
	}


	function signalHandler($signo) {
		switch ($signo) :
		case SIGTERM:
		case SIGINT:
			l::notice("Main thread terminating on SIG" . ($signo == SIGTERM ? "TERM" : "INT"));
			exit(0);
		case SIGCHLD:
			do {
				$wr = pcntl_waitpid(-1, $status, WNOHANG);
			} while ($wr > 0);
			break;
		endswitch;
	}


	function update() {
		// Mask signals
		$this->setSignals(true);

		// Reload configuration
		if (config::reload()) {
			l::notice("Configuration changed, re-scheduling");
			$this->setAlarm();
		}

		// Stop/start ticks
		if ($this->ticksStopped && file_exists(config::$main['cachedir'] . "/start_ticks")) {
			l::notice("Ticks restarted");
			$this->ticksStopped = false;
			unlink(config::$main['cachedir'] . "/ticks_stopped");
		} elseif (file_exists(config::$main['cachedir'] . "/stop_ticks")) {
			l::notice("Ticks stopped");
			$this->ticksStopped = true;
			touch(config::$main['cachedir'] . "/ticks_stopped");
		}
		@unlink(config::$main['cachedir'] . "/stop_ticks");
		@unlink(config::$main['cachedir'] . "/start_ticks");

		// Tell the system control script that we are still running
		if (!$this->debug && file_exists(config::$main['cs_fifo'])) {
			$pipe = fopen(config::$main['cs_fifo'], "w");
			fwrite($pipe, "TMPID {$this->pid}\n");
			fclose($pipe);
		}

		// Unmask child signals
		$this->setSignals(false);
	}


	function setAlarm() {
		$min = null;
		foreach (config::$config->games as $name => $game) {
			foreach ($game->ticks as $tName => $tick) {
				if (!is_null($tick->next) && (is_null($min) || $tick->next < $min)) {
					$min = $tick->next;
				}
			}
		}
		if (is_null($min)) {
			l::warn("No ticks left to execute");
			return;
		}

		$delay = $min - time();
		$rdelay = $delay <= 1 ? 2 : $delay;
		pcntl_alarm($rdelay);
	}


	function alarm($sig) {
		$this->setSignals(true);

		$exec = array();
		$now = time();
		foreach (config::$config->games as $name => $game) {
			foreach ($game->ticks as $tName => $tick) {
				if (!is_null($tick->next) && $tick->next <= $now) {
					array_push($exec, $tick);
					$oNext = $tick->next;
					$tick->computeNext();
					if ($tick->next !== $oNext && !$this->ticksStopped) {
						if (is_null($tick->next)) {
							l::info("Not rescheduling {$name}::{$tName}");
						}
					}
				}
			}
		}
		if (!count($exec)) {
			l::warning("No ticks scheduled for execution!");
		}

		$this->setAlarm();

		foreach ($exec as $tick) {
			$this->startThread($tick);
		}

		$this->setSignals(false);
	}


	function startThread($tick) {
		// Don't even try if ticks are stopped
		if ($this->ticksStopped) {
			$this->lastTick = time();
			return;
		}

		// Fork
		$pid = pcntl_fork();
		if ($pid == -1) {
			l::error("Fork failed for {$tick->game->name}::{$tick->definition->script}!");
			return false;
		} elseif ($pid) {
			$this->lastTick = time();
			return $pid;
		}

		// We're in the child, detach from parent as much as possible
		if (!$this->debug) {
			posix_setsid();
		}

		// Connect to the DB and check if the game's running
		if (!dbConnect(false)) {
			exit(0);
		}
		$tick->game->getDBAccess();
		if ($tick->game->version->id != 'main') {
			$status = $tick->game->status();
			if ($status == 'FINISHED'
			   || ($status == 'PRE' || $status == 'READY') && $tick->definition->public) {
				exit(0);
			}
		}

		// Execute the tick
		l::info("TICK: {$tick->game->name}::{$tick->definition->script}");
		ob_start();
		$tick->run($t);
		$text = ob_get_contents();
		ob_end_clean();

		if ($text != '') {
			$text = explode("\n", $text);
			foreach ($text as $line) {
				l::notice("{$tick->game->name}::{$tick->definition->script} OUTPUT: $line");
			}
		}

		dbClose();

		exit(0);
	}
}


?>

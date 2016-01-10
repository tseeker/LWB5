<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Engine ticks
//
// main/ticks/deathofrats/library.inc
//
// This tick is the Death of Rats.
//
// Ok, actually, it's the AMS. It checks for open proxies and other
// signs of suspicious behaviour.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class main_ticks_deathofrats_library {

	/** How long does punishment last? */
	const punishmentDays	= 20;
	/** How long before we reaxime an account after it's been warned? */
	const postWarningGrace	= 7;
	/** Delay in hours between ingame checks on suspicious accounts */
	const inGameCheckDelay	= 4;


	/** Levels for attempts to conceal pass-sharing */
	private static $viciousLevels = array(
		1	=> array('VICIOUS-LP'),
		2	=> array('VICIOUS-MP', 'VICIOUS-LP'),
		3	=> array('VICIOUS-HP', 'VICIOUS-MP', 'VICIOUS-LP'),
	);

	/** Single player badness points, days at maximum score and days with
	 * decreasing score.
	 */
	private static $spBadness = array(
		// Action	=> array( Points, MaxDays, DecDays )
		"PROXY"		=> array( 40, 30, 30 ),
		"CLCOOK-SIP"	=> array( 5, 2, 1 ),
		"CLCOOK-DIP"	=> array( 2, 2, 1 ),
		"BAT"		=> array( 80, 60, 60 )
	);

	/** Multiplayer badness points, days at maximum score and days with
	 * decreasing score.
	 */
	private static $mpBadness = array(
		// Action	=> array( Points, MaxDays, DecDays )
		"SIMPLE"	=> array( 15, 6, 1 ),
		"SIMPLE-10"	=> array( 18, 6, 1 ),
		"VICIOUS-LP"	=> array( 15, 3, 3 ),
		"VICIOUS-MP"	=> array( 30, 7, 7 ),
		"VICIOUS-HP"	=> array( 50, 21, 15 ),
		"PASS"		=> array( 90, 90, 90 ),
		"NOPASS"	=> array( -20, 90, 30 )
	);

	/** In-game badness points, days at maximum score and days with
	 * decreasing score.
	 */
	private static $igBadness = array(
		// Action	=> array( Points, MaxDays, DecDays )
		"LSE"		=> array( 5, 3, 1 ),
		"SE"		=> array( 10, 4, 3 ),
		"HSE"		=> array( 15, 5, 5 ),
		"VHSE"		=> array( 20, 8, 7 )
	);

	private $lastExec;
	private $now;
	private $connections;
	private $perAccount;
	private $proxiedAccounts;
	private $nEvents;
	private $browserNames;
	private $nChanges;
	private $userPlays = array();

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		config::$main['debug'] = 2;
	}

	public function runTick() {
		$this->nEvents = $this->nChanges = 0;
		$this->now = time();
		$this->browserNames = array();

		// Fetch the time at which the Death of Rats was last activated
		sleep(1);
		$this->lastExec = $this->db->safeTransaction(array($this, "getLastExec"));
		if ($this->lastExec == $this->now) {
			l::notice("Not running the Death of Rats - last execution is now");
			return;
		}

		// Execute account-level checks
		$this->accountChecks();

		// Execute in-game checks
		$ingameChecks = $this->db->safeTransaction(array($this, 'getInGameCheckList'));
		if (count($ingameChecks)) {
			l::debug("Executing in-game checks");
			foreach ($ingameChecks as $pair) {
				$this->db->safeTransaction(array($this, 'checkInGamePair'), $pair);
			}
		}
		$finalPoints = $this->db->safeTransaction(array($this, 'finalizeInGameData'));

		// Take action if needed
		foreach ($finalPoints as $id => $points) {
			if ($points < 100) {
				continue;
			}
			$accounts = explode(',', $id);
			$this->db->safeTransaction(array($this, 'takeAction'), $accounts);
		}
	}

	/** This method performs account-level checks.
	 */
	private function accountChecks() {
		// If the Death of Rats had never been activated, act as if it had
		// been one hour earlier, and check accounts with identical
		// password.
		if (is_null($this->lastExec)) {
			l::notice("First execution, checking identical passwords");
			$this->lastExec = $this->now - 3600;
			$this->db->safeTransaction(array($this, 'checkIdenticalPassword'));
		} else {
			l::debug("Checking password changes");
			$this->nChanges += $this->db->safeTransaction(array($this, 'checkPassChange'));
		}

		// For each player who logged in or created an account, check
		// single-player suspicious behaviour.
		$this->connections = $this->db->safeTransaction(array($this, 'getLastLogons'));
		$this->nChanges += count($this->connections);
		if (! $this->nChanges) {
			l::info("No new records, skipping account-level checks.");
			return;
		}

		// If we had connection records...
		if (!empty($this->connections)) {
			l::debug("Analysing " . count($this->connections) . " new record(s)");
			$this->makePerAccountRecords();

			// Start with open proxies
			l::debug("Checking for open proxies ...");
			$this->checkOpenProxies();
			if (count($this->proxiedAccounts)) {
				l::info("Logging " . count($this->proxiedAccounts) . " account(s) using open proxies");
				$this->db->safeTransaction(array($this, 'logOpenProxies'));
			}

			// Now examine per-account entries to find different types of rats
			l::debug("Checking single player badness");
			foreach ($this->perAccount as $records) {
				$this->db->safeTransaction(array($this, 'checkSinglePlayer'), array($records));
			}

			// Check for different players logging on from the same IP and on the same tracking
			l::debug("Checking simple multiing / pass sharing");
			$this->db->safeTransaction(array($this, 'checkSimpleMultis'));

			// Check for players who clear cookies before logging on to another account
			l::debug("Checking vicious multiing / pass sharing");
			$this->db->safeTransaction(array($this, 'checkViciousMultis'));
		}

		// Flush the main logs
		$this->db->safeTransaction(array($this, 'finalizeMainData'));
	}


	/** This method returns the last execution time of the Death of Rats.
	 */
	public function getLastExec() {
		$q = $this->db->query("SELECT ts FROM dor_exec ORDER BY ts DESC LIMIT 1");
		if (dbCount($q)) {
			list($last) = dbFetchArray($q);
		} else {
			$last = null;
		}
		return $last;
	}

	/** This method flushes both logs and adds the execution entry
	 */
	public function finalizeMainData() {
		l::debug("Updating main Death of Rats status");
		$this->db->query(
			"LOCK TABLE dor_exec, dor_single, dor_multi, pass_change, dor_single_points, dor_multi_points "
				. "IN ACCESS EXCLUSIVE MODE"
		);
		$this->flushSinglePlayerLog();
		$this->flushMultiPlayerLog();
		$this->db->query(
			"INSERT INTO dor_exec (ts, entries, events) "
				. "VALUES ({$this->now}, {$this->nChanges}, {$this->nEvents})"
		);
		$this->db->query("DELETE FROM pass_change WHERE ts < {$this->now}");
		$this->db->query("DELETE FROM dor_exec WHERE ts < {$this->now} - 7 * 24 * 3600");

		$spBadness = $this->computeSinglePlayerBadness();
		$this->computeMultiPlayerBadness($spBadness);
	}

	/** This method flushes the in-game log and computes final badness
	 * points for each player.
	 */
	public function finalizeInGameData() {
		l::debug("Updating Death of Rats status for in-game checks");
		$this->flushInGameLog();
		return $this->computeInGameBadness();
	}


	/***********************************************************************
	 * ACTIONS TO PERFORM ON POSITIVE DECISIONS                            *
	 ***********************************************************************/

	/** Main action handler for a pair of accounts that have been
	 * positively identified by the Death of Rats.
	 */
	public function takeAction($account1, $account2) {
		// Check and send warnings; return if no further
		// action is required.
		if ($this->checkWarnings($account1, $account2)) {
			return;
		}

		// Punish the accounts if needed
		$this->checkPunishment($account1, $account2);
	}


	/** This function checks the status of two accounts on the "warning"
	 * angle of things.
	 */
	private function checkWarnings($account1, $account2) {
		// Check if both accounts were warned
		$q = $this->db->query(
			"SELECT account1,MAX(ts) FROM dor_warning "
				. "WHERE account1 IN ($account1, $account2) "
				. "GROUP BY account1"
		);
		if (dbCount($q) == 0) {
			// Both accounts need a warning
			$this->sendWarning($account1, $account2);
			$this->sendWarning($account2, $account1);
			return true;
		} elseif (dbCount($q) == 2) {
			// Both accounts have been warned, check the grace
			// period.
			list($account, $mts) = dbFetchArray($q);
			list($account, $mts2) = dbFetchArray($q);
			$mts = ($mts > $mts2) ? $mts : $mts2;
			return ($this->now - $mts > self::postWarningGrace * 86400);
		}

		// One of the accounts has never been warned, send him a warning
		list($account, $mts) = dbFetchArray($q);
		if ($account == $account1) {
			$this->sendWarning($account2, $account1);
		} else {
			$this->sendWarning($account1, $account2);
		}

		return true;
	}


	/** This method sends a warning to a player about him being flagged with
	 * another player.
	 */
	private function sendWarning($account1, $account2) {
		l::trace(" Warning account #$account1");
		$this->db->query(
			"INSERT INTO dor_warning (account1, account2, ts) "
				. "VALUES ($account1, $account2, {$this->now})"
		);
		// FIXME: should send messages in all games they play together
	}


	/** This method checks whether two accounts should be punished.
	 */
	private function checkPunishment($account1, $account2) {
		$q = $this->db->query(
			"SELECT account FROM dor_punishment "
				. "WHERE account IN ($account1, $account2) "
				  . "AND {$this->now} - ts < " . self::punishmentDays * 86400
		);

		if (dbCount($q) >= 2) {
			// Both are already punished
			return;
		} elseif (dbCount($q) == 0) {
			// Punish both accounts
			$this->punish($account1, $account2);
			$this->punish($account2, $account1);
			return;
		}

		// Punish the account that is still unpunished
		list($account) = dbFetchArray($q);
		$this->punish($account == $account1 ? $account2 : $account1,
			$account == $account1 ? $account1 : $account2);
	}


	/** This method punishes a player for abusive pass sharing or multiing
	 * with another player.
	 */
	private function punish($account1, $account2) {
		l::trace(" PUNISHING ACCOUNT #$account1");
		$this->db->query(
			"INSERT INTO dor_punishment (account, other_account, ts) "
				. "VALUES ($account1, $account2, {$this->now})"
		);
		// FIXME: should send messages in all games they play together
	}


	/***********************************************************************
	 * BADNESS POINTS COMPUTATION                                          *
	 ***********************************************************************/

	private function computeSinglePlayerBadness() {
		// Generate the array of query parts
		$qParts = array();
		foreach (self::$spBadness as $type => $data) {
			array_push($qParts, "(message = '$type' AND {$this->now} - ts < "
				. (24 * 3600 * ($data[1] + $data[2])) . ")");
		}

		// Retrieve entries
		$qString = "SELECT * FROM dor_single WHERE " . join(" OR ", $qParts) . " ORDER BY ts";
		$q = $this->db->query($qString);

		// Compute points
		$points = array();
		while ($r = dbFetchHash($q)) {
			$pPoints = $points[$r['account']];

			$badness = self::$spBadness[$r['message']];
			$bPoints = $badness[0];
			$time = $this->now - $r['ts'];

			if ($time > $badness[1] * 24 * 3600) {
				$time -= $badness[1] * 24 * 3600;
				$time = ($badness[2] * 24 * 3600) - $time;
				$bPoints = round($bPoints * $time / ($badness[2] * 24 * 3600));
			}

			$points[$r['account']] = max(0, $pPoints + $bPoints);
		}

		// Insert points
		$spData = new db_copy("dor_single_points", db_copy::copyToClean);
		$spData->setAccessor($this->db);
		foreach ($points as $account => $bPoints) {
			if ($bPoints == 0) {
				continue;
			}
			$spData->appendRow(array($account, $bPoints));
		}
		$spData->execute();

		return $points;
	}


	private function computeMultiPlayerBadness($spBadness) {
		// Generate the array of query parts
		$qParts = array();
		foreach (self::$mpBadness as $type => $data) {
			array_push($qParts, "(message = '$type' AND {$this->now} - ts < "
				. (24 * 3600 * ($data[1] + $data[2])) . ")");
		}

		// Retrieve entries
		$qString = "SELECT * FROM dor_multi WHERE " . join(" OR ", $qParts) . " ORDER BY ts";
		$q = $this->db->query($qString);

		// Compute points
		$points = array();
		while ($r = dbFetchHash($q)) {
			$id = $r['account1'] . ',' . $r['account2'];
			$pPoints = $points[$id];

			$badness = self::$mpBadness[$r['message']];
			$bPoints = $badness[0];
			$time = $this->now - $r['ts'];

			if ($time > $badness[1] * 24 * 3600) {
				$time -= $badness[1] * 24 * 3600;
				$time = ($badness[2] * 24 * 3600) - $time;
				$bPoints = round($bPoints * $time / ($badness[2] * 24 * 3600));
			}

			$points[$id] = max(0, $pPoints + $bPoints);
		}

		// Add single player badness to records
		foreach ($points as $id => $bPoints) {
			$accounts = explode(',', $id);
			$bPoints += ceil(($spBadness[$accounts[0]] + $spBadness[$accounts[1]]) / 2);
			$points[$id] = $bPoints;
		}

		// Insert points
		$mpData = new db_copy("dor_multi_points", db_copy::copyToClean);
		$mpData->setAccessor($this->db);
		foreach ($points as $id => $bPoints) {
			if ($bPoints == 0) {
				continue;
			}
			$accounts = explode(',', $id);
			$mpData->appendRow(array($accounts[0], $accounts[1], $bPoints));
		}
		$mpData->execute();
	}


	private function computeInGameBadness() {
		// Generate the array of query parts
		$qParts = array();
		foreach (self::$igBadness as $type => $data) {
			array_push($qParts, "(message LIKE '$type-%' AND {$this->now} - ts < "
				. (24 * 3600 * ($data[1] + $data[2])) . ")");
		}

		// Retrieve entries
		$qString = "SELECT * FROM dor_ingame_check WHERE " . join(" OR ", $qParts) . " ORDER BY ts";
		$q = $this->db->query($qString);

		// Compute points
		$points = array();
		while ($r = dbFetchHash($q)) {
			$id = $r['account1'] . ',' . $r['account2'];
			$pPoints = $points[$id];

			list($message, $count) = explode("-", $r['message']);

			$badness = self::$igBadness[$message];
			$bPoints = $badness[0] * $count;
			$time = $this->now - $r['ts'];

			if ($time > $badness[1] * 24 * 3600) {
				$time -= $badness[1] * 24 * 3600;
				$time = ($badness[2] * 24 * 3600) - $time;
				$bPoints = round($bPoints * $time / ($badness[2] * 24 * 3600));
			}

			$points[$id] = max(0, $pPoints + $bPoints);
		}

		// Insert points
		$igData = new db_copy("dor_final_points", db_copy::copyToClean);
		$igData->setAccessor($this->db);
		foreach ($points as $id => $bPoints) {
			if ($bPoints == 0) {
				continue;
			}
			$accounts = explode(',', $id);
			$igData->appendRow(array($accounts[0], $accounts[1], $bPoints));
		}
		$igData->execute();

		return $points;
	}


	/***********************************************************************
	 * VARIOUS HELPER METHODS                                              *
	 ***********************************************************************/

	/** This method compares two IP addresses. It returns 2 if they are equal,
	 * 1 if they are on the same /24, or 0 if they don't match at all.
	 */
	static private function ipSimilarity($ip1, $ip2) {
		if ($ip1 == $ip2) {
			return 2;
		}

		$l = strrpos($ip1, ".");
		if (is_bool($l)) {
			l::notice("IP address without dots, that's weird ... IP was \"$ip1\"");
			return 0;
		}

		if (substr($ip1, 0, $l + 1) == substr($ip2, 0, $l + 1)) {
			return 1;
		}
		return 0;
	}


	/** This method returns the name of the browser associated with a
	 * tracking cookie ID.
	 */
	private function getBrowserName($id) {
		if (is_null($this->browserNames[$id])) {
			$q = $this->db->query("SELECT browser FROM web_tracking WHERE id = $id");
			list($this->browserNames[$id]) = dbFetchArray($q);
		}
		return $this->browserNames[$id];
	}


	/***********************************************************************
	 * PASSWORDS                                                           *
	 ***********************************************************************/

	/** This method checks the list of accounts for identical passwords. It is
	 * only called once, the first time the Death of Rats is started.
	 */
	public function checkIdenticalPassword() {
		$q = $this->db->query(
			"SELECT password FROM account "
				. "WHERE status IN ('STD', 'VAC', 'NEW') "
				. "GROUP BY password "
				. "HAVING COUNT(*) > 1"
		);
		while ($r = dbFetchArray($q)) {
			$qId = $this->db->query(
				"SELECT id FROM account "
					. "WHERE password = '" . addslashes($r[0]) . "' "
					  . "AND status IN ('STD', 'VAC', 'NEW')"
			);
			l::notice("Found " . dbCount($qId) . " accounts with password '{$r[0]}'");

			$accounts = array();
			while ($rID = dbFetchArray($qId)) {
				array_push($accounts, $rID[0]);
			}

			for ($i = 0; $i < count($accounts) - 1; $i ++) {
				for ($j = $i + 1; $j < count($accounts); $j ++) {
					$this->multiPlayerLog("PASS", $accounts[$i], $accounts[$j]);
				}
			}
		}
	}

	/** This method checks for players changing their password to another
	 * player's password.
	 */
	public function checkPassChange() {
		// Get all password changes
		$q = $this->db->query("SELECT * FROM pass_change ORDER BY ts");
		if (!dbCount($q)) {
			return 0;
		}

		// Create per-account changes list
		$accounts = array();
		while ($r = dbFetchHash($q)) {
			if (!is_array($accounts[$r['account']])) {
				$accounts[$r['account']] = array("changes" => array());
			}
			if (is_null($accounts[$r['account']]['initial'])) {
				$accounts[$r['account']]['initial'] = addslashes($r['old_pass']);
			}
			$pass = addslashes($r['new_pass']);
			$accounts[$r['account']]['last'] = $pass;
		}

		// For each account, verify the changes
		foreach ($accounts as $account => $data) {
			if ($data['initial'] == $data['last']) {
				continue;
			}

			// Get accounts that use these passwords
			$passwords = array();
			if ($data['initial'] != '') {
				$passwords[] = $data['initial'];
			}
			$passwords[] = $data['last'];

			$q = $this->db->query(
				"SELECT id, password FROM account "
					. "WHERE id <> $account AND password IN ('" . join("','", $passwords) . "')"
			);
			if (!dbCount($q)) {
				continue;
			}
			$otherList = array();
			while ($r = dbFetchArray($q)) {
				$r[1] = addslashes($r[1]);
				if (!is_array($otherList[$r[1]])) {
					$otherList[$r[1]] = array($r[0]);
				} else {
					$otherList[$r[1]][] = $r[0];
				}
			}

			// If the old password was used by another account, mark it
			if ($data['initial'] != '' && is_array($otherList[$data['initial']])) {
				foreach ($otherList[$data['initial']] as $account2) {
					$this->multiPlayerLog("NOPASS", $account, $account2);
				}
			}

			// If the new password is used by another account, mark it
			if (is_array($otherList[$data['last']])) {
				foreach ($otherList[$data['last']] as $account2) {
					$this->multiPlayerLog("PASS", $account, $account2);
				}
			}
		}

		return count($accounts);
	}


	/***********************************************************************
	 * ACCESSING PREVIOUS RECORDS                                          *
	 ***********************************************************************/

	/** This method lists the latest entries from the account_log table.
	 */
	public function getLastLogons() {
		$q = $this->db->query(
			"SELECT account, tracking, ip_addr, action, t FROM account_log "
				. "WHERE action IN ('IN', 'OUT', 'CREATE') "
				  . "AND t > {$this->lastExec} AND t <= {$this->now}"
				. "ORDER BY t"
		);
		$records = array();
		while ($r = dbFetchHash($q)) {
			array_push($records, $r);
		}
		return $records;
	}


	/** This method goes through all records read from the database and
	 * creates separate lists for each account.
	 */
	public function makePerAccountRecords() {
		$paRecords = array();

		foreach ($this->connections as $record) {
			if (!is_array($paRecords[$record['account']])) {
				$paRecords[$record['account']] = array();
			}
			array_push($paRecords[$record['account']], $record);
		}

		$this->perAccount = $paRecords;
	}


	/***********************************************************************
	 * OPEN PROXIES                                                        *
	 ***********************************************************************/

	/** This method checks for open proxies in the latest log entries.
	 */
	private function checkOpenProxies() {
		$IPs = array();

		// Make lists of accounts for each IP
		foreach ($this->connections as $record) {
			if ($record['ip_addr'] == 'AUTO' || $record['action'] == 'OUT') {
				continue;
			}
			$ip = $record['ip_addr'];
			$account = $record['account'];
			if (!is_array($IPs[$ip])) {
				$IPs[$ip] = array($account);
			} elseif (!in_array($account, $IPs[$ip])) {
				array_push($IPs[$ip], $account);
			}
		}

		// Check for proxies on the IPs
		$requests = array();
		$proxies = array();
		foreach (array_keys($IPs) as $ip) {
			if (count($requests) < 20) {
				array_push($requests, $ip);
				continue;
			}

			try {
				$results = pcheck::check($requests);
			} catch (Exception $e) {
				l::error("Failed to check some addresses for open proxies");
				l::info($e->getMessage());
				return;
			}

			foreach ($results as $host => $status) {
				if ($status == 1) {
					array_push($proxies, $host);
				}
			}

			$requests = array();
		}

		// If there are some requests we didn't execute, do it
		if (count($requests)) {
			try {
				$results = pcheck::check($requests);
			} catch (Exception $e) {
				l::error("Failed to check some addresses for open proxies");
				l::info($e->getMessage());
				return;
			}

			foreach ($results as $host => $status) {
				if ($status == 1) {
					array_push($proxies, $host);
				}
			}
		}

		// Check for proxied accounts
		$proxyAccounts = array();
		foreach ($proxies as $ip) {
			foreach ($IPs[$ip] as $account) {
				if (in_array($account, $proxyAccounts)) {
					continue;
				}
				array_push($proxyAccounts, $account);
			}
		}

		$this->proxiedAccounts = $proxyAccounts;
	}


	/** This method logs access to accounts using open proxies. A log
	 * entry is only added every 24h.
	 */
	public function logOpenProxies() {
		// Get all recent open proxy logs
		$this->db->query(
			"SELECT account FROM dor_single "
				. "WHERE message = 'PROXY' AND {$this->now} - ts < 86400"
		);
		$recent = array();
		while ($r = dbFetchArray($q)) {
			$recent[] = $r[0];
		}

		// Insert proxy logs
		foreach ($this->proxiedAccounts as $account) {
			if (in_array($account, $recent)) {
				continue;
			}
			$this->singlePlayerLog("PROXY", $account);
		}
	}


	/***********************************************************************
	 * SINGLE PLAYER SUSPICIOUS BEHAVIOUR                                  *
	 ***********************************************************************/

	public function checkSinglePlayer($records) {
		// Ignore accounts that have already been kicked
		$q = $this->db->query(
			"SELECT status FROM account WHERE id = {$records[0]['account']}"
		);
		list($status) = dbFetchArray($q);
		if ($status == 'KICKED') {
			return;
		}
		l::trace("CHECKING SINGLE PLAYER {$records[0]['account']}");

		// Check log-ons for people who have just tripped the "banned log-in" detector
		$baDetected = false;
		foreach ($records as $record) {
			if ($record['action'] == 'OUT') {
				continue;
			}
			if ($record['action'] == 'CREATE') {
				$time = 300;
			} else {
				$time = 120;
			}
			$q = $this->db->query(
				"SELECT * FROM banned_attempt "
					. "WHERE ip_addr = '{$record['ip_addr']}' AND ts <= {$record['t']} "
					  . "AND {$record['t']} - ts <= $time"
			);
			if (dbCount($q)) {
				$baDetected = true;
				break;
			}
		}

		// Banned log-in attempt detected, check for a similar record in the past 2 weeks
		if ($baDetected) {
			$q = $this->db->query(
				"SELECT * FROM dor_single "
					. "WHERE account = {$records[0]['account']} AND message = 'BAT' "
					  . "AND {$this->now} - ts < 14 * 24 * 3600"
			);
			if (!dbCount($q)) {
				$this->singlePlayerLog('BAT', $records[0]['account']);
			}
		}

		// If the first record isn't the account's creation, get
		// the previous record
		if ($records[0]['action'] != 'CREATE') {
			$q = $this->db->query(
				"SELECT account, tracking, ip_addr, action, t FROM account_log "
					. "WHERE account = {$records[0]['account']} AND t <= {$this->lastExec} "
					  . "AND (action = 'IN' OR action = 'CREATE')"
					. "ORDER BY t DESC LIMIT 1"
			);
			if (dbCount($q)) {
				array_unshift($records, dbFetchHash($q));
				l::trace(" Found previous action with tracking {$records[0]['tracking']}");
			}
		}

		// For each couple of records in the list, check for cookie deletion
		$lastExam = null;
		for ($i = 1; $i < count($records); $i ++) {
			if (is_null($records[$i]['tracking']) || $records[$i]['ip_addr'] == 'AUTO') {
				continue;
			}
			l::trace(" Checking record with tracking ID {$records[$i]['tracking']} (action '"
				. "{$records[$i]['action']}')");

			// Find previous valid record
			for ($j = $i - 1; $j >= 0; $j --) {
				if ($records[$j]['action'] == 'OUT' && $records[$j]['ip_addr'] == 'AUTO'
						|| is_null($records[$j]['tracking'])
						|| $records[$j]['tracking'] == $records[$i]['tracking']) {
					continue;
				}
				break;
			}
			if ($j < 0 || $j === $lastExam) {
				continue;
			}
			$lastExam = $j;

			l::trace("  Found suspicious records: {$records[$j]['tracking']} / {$records[$i]['tracking']}");

			// Check IP address
			$ipCheck = self::ipSimilarity($records[$j]['ip_addr'], $records[$i]['ip_addr']);
			l::trace("  IP similarity: $ipCheck");
			if ($ipCheck == 0) {
				continue;
			}

			// Get browser string for each tracking ID
			$browser1 = $this->getBrowserName($records[$j]['tracking']);
			$browser2 = $this->getBrowserName($records[$i]['tracking']);
			$leven = levenshtein($browser1, $browser2);
			l::trace("  Browser similarity: $leven");
			if ($leven > 10) {
				// *Probably* 2 different browsers, skip
				continue;
			}

			// Check for earlier records that would contradict cookie deletion
			$q = $this->db->query(
				"SELECT COUNT(*) FROM account_log "
					. "WHERE t < {$records[$j]['t']} AND tracking = {$records[$i]['tracking']}"
			);
			list($count) = dbFetchArray($q);
			if ($count > 0) {
				l::trace("  Found older uses of tracking ID {$records[$i]['tracking']}"
					. ", multiple computers");
				continue;
			}

			l::trace("  ADDING RECORD");
			$this->singlePlayerLog($ipCheck == 2 ? "CLCOOK-SIP" : "CLCOOK-DIP", $records[$i]['account']);
		}
	}


	/**********************************************
	 * MULTI PLAYERS DETECTION                    *
	 **********************************************/

	/** This method gets all tracking cookie IDs used in the records, and
	 * generates an "extended" record including the previous X actions on
	 * each tracking record.
	 */
	private function extendRecord($nActions = 1) {
		// Get the list of all tracking cookies used in the log
		$tracking = array();
		foreach ($this->connections as $record) {
			if (is_null($record['tracking'])) {
				continue;
			}
			if (!in_array($record['tracking'], $tracking)) {
				$tracking[] = $record['tracking'];
			}
		}

		if (!empty($tracking)) {
			// First we need to get the previous record for all tracking cookies
			$q = $this->db->query(
				"SELECT account, tracking, ip_addr, action, t FROM account_log "
					. "WHERE tracking IN (" . join(',', $tracking) . ") "
					  . "AND t <= {$this->lastExec} "
					  . "AND action IN ('IN', 'CREATE', 'OUT') "
					. "ORDER BY t DESC"
			);

			// Create the list of records to merge into the log fragment
			$extraRecords = array();
			$okTrackings = array();
			while (count($extraRecords) < $nActions * count($tracking) && $r = dbFetchHash($q)) {
				if ($okTrackings[$r['tracking']] == $nActions) {
					continue;
				}
				$extraRecords[] = $r;
				$okTrackings[$r['tracking']] ++;
			}
		} else {
			$extraRecords = array();
		}
		return array_merge(array_reverse($extraRecords), $this->connections);
	}


	/** This method checks for the simplest type of multi accounts,
	 * but also catches occasional pass-sharers.
	 */
	public function checkSimpleMultis() {
		// Get the extra actions we need
		$connections = $this->extendRecord();

		// Extract the guys who got caught logging in on the same browser
		// from the list of records.
		$caught = array();
		foreach (array_keys($this->perAccount) as $account) {
			// Find all tracking IDs used by this account
			l::trace("CHECKING ACCOUNT $account");
			$aTrack = array();
			foreach ($connections as $record) {
				if (!is_null($record['tracking']) && $record['account'] == $account
						&& !in_array($record['tracking'], $aTrack)) {
					$aTrack[] = $record['tracking'];
				}
			}

			// Find all other accounts who have a record with the same tracking
			foreach ($aTrack as $tracking) {
				l::trace(" Checking tracking #$tracking");
				$lastUse = null; $prevAccount = null;
				foreach ($connections as $record) {
					if ($tracking != $record['tracking']) {
						continue;
					}
					if (is_null($lastUse)) {
						$lastUse = $record['t'];
						$prevAccount = $record['account'];
						l::trace("  Last tracking use by $prevAccount");
						continue;
					}

					if ($record['account'] != $prevAccount) {
						l::trace("  Tracking used by different account, {$record['account']}");
						$bothAccounts = array($record['account'], $prevAccount);
						sort($bothAccounts);
						$bothAccounts = join(',', $bothAccounts);
						$time = ($record['t'] - $lastUse <= 10) ? 2 : 1;
						if ($time > $caught[$bothAccounts]) {
							$caught[$bothAccounts] = $time;
						}
						$prevAccount = $record['account'];
					}
					$lastUse = $record['t'];
				}
			}
		}

		// Adds the log entries
		$nFound = 0;
		foreach ($caught as $accounts => $type) {
			list($a1, $a2) = explode(',', $accounts);
			$nFound ++;
			$this->multiPlayerLog($type == 2 ? "SIMPLE-10" : "SIMPLE", $a1, $a2);
		}
		if ($nFound) {
			l::notice("$nFound simple multiing / pass sharing attempt(s) found");
		}
	}


	/** This method checks for vicious multi accounts: people who clear
	 * cookies before logging on to other accounts. To perform this check,
	 * the game will start by looking up 'IN' records from any account using
	 * a different tracking cookie ID but the same browser and a close IP
	 * address. For each different account found, it will check the sequence
	 * of tracking cookies used.
	 */
	public function checkViciousMultis() {
		$caught = array();
		foreach ($this->perAccount as $account => $records) {
			l::trace(" Verifying account $account");
			for ($i = 0; $i < count($records); $i ++) {
				$recPair = $this->findInterestingPair($records, $i);
				if (is_null($recPair)) {
					continue;
				}
				list($R1, $R2) = $recPair;
				l::trace("  Found pair of records: {$R1['tracking']} vs {$R2['tracking']} ("
					. gmstrftime("%H:%M:%S %Y-%m-%d", $R1['t']) . " / "
					. gmstrftime("%H:%M:%S %Y-%m-%d", $R2['t']) . ")");

				// Get the list of logons between R1 and R2 and
				// presenting ... troubling ... similarities
				$R3List = $this->findIntermediaryLogons($R1, $R2);
				if (empty($R3List)) {
					continue;
				}

				// Check the records in the list of intermediary
				// logons.
				foreach ($R3List as $R3) {
					$caughtID = array($account, $R3['account']);
					sort($caughtID);
					$caughtID = join(',', $caughtID);
					if ($caught[$caughtID] == 3) {
						// We already caught this pair as
						// VICIOUS-HP, don't recheck
						continue;
					}

					l::trace("  Checking triplet with record from account {$R3['account']} "
						. "(tracking {$R3['tracking']})");
					$check = $this->checkViciousTriplet($R1, $R2, $R3);
					if ($caught[$caughtID] < $check) {
						$caught[$caughtID] = $check;
					}
				}
			}
		}

		// For each guy we caught, add an entry to the log if one doesn't exist
		// in the past 6 hours with a probability at least as high.
		$nVicious = 0;
		foreach ($caught as $ids => $level) {
			list($id1, $id2) = explode(',', $ids);
			$levels = self::$viciousLevels[$level];

			$q = $this->db->query(
				"SELECT COUNT(*) FROM dor_multi "
					. "WHERE account1 = $id1 AND account2 = $id2 "
					  . "AND message IN ('" . join("','", $levels) . "') "
					  . "AND {$this->now} - ts <= 6 * 3600"
			);
			list($flagged) = dbFetchArray($q);
			if ($flagged > 0) {
				continue;
			}

			$nVicious ++;
			$this->multiPlayerLog($levels[0], $id1, $id2);
		}
		if ($nVicious) {
			l::notice("$nVicious vicious multiing attempt(s) found");
		}
	}


	/** This method scans the log for two records from the same player,
	 * starting at the $i-th record in the list, where the IP is close,
	 * the browser is similar and the tracking is different.
	 */
	private function findInterestingPair($records, $i) {
		$record = $records[$i];
		if ($record['action'] != 'IN') {
			return null;
		}

		// Try getting a previous record that matches
		$found = null;
		for ($j = $i - 1; $j >= 0; $j --) {
			$oRecord = $records[$j];
			if ($oRecord['tracking'] == $record['tracking']) {
				// Found a previous record with the same tracking cookie
				return null;
			}

			if ($oRecord['action'] != 'IN' || ! self::ipSimilarity($record['ip_addr'], $oRecord['ip_addr'])
					|| $oRecord['tracking'] == $record['tracking']) {
				continue;
			}

			$browser1 = $this->getBrowserName($record['tracking']);
			$browser2 = $this->getBrowserName($oRecord['tracking']);
			if ($browser1 != $browser2 && levenshtein($brower1, $browser2) > 10) {
				// *Probably* 2 different browsers, skip
				continue;
			}

			$found = $oRecord;
			break;
		}

		// If we didn't find a matching record in the list, try finding
		// one from the database
		if (is_null($found)) {
			$classC = substr($record['ip_addr'], 0, strrpos($record['ip_addr'], '.') - 1) . "%";
			$q = $this->db->query(
				"SELECT account, tracking, ip_addr, action, t FROM account_log "
					. "WHERE account = {$record['account']} AND action = 'IN' "
					  . "AND tracking <> {$record['tracking']} AND tracking IS NOT NULL "
					  . "AND t < {$record['t']} AND ip_addr LIKE '$classC' "
					. "ORDER BY t DESC LIMIT 10"
			);
			if (!dbCount($q)) {
				// Nothing found, return
			}
			while ($oRecord = dbFetchHash($q)) {
				// Check browsers
				$browser1 = $this->getBrowserName($record['tracking']);
				$browser2 = $this->getBrowserName($oRecord['tracking']);
				if ($browser1 != $browser2 && levenshtein($brower1, $browser2) > 10) {
					// *Probably* 2 different browsers, skip
					continue;
				}

				// Check if there are intermediary records
				$q2 = $this->db->query(
					"SELECT COUNT(*) FROM account_log "
						. "WHERE account = {$record['account']} AND action = 'IN' "
						  . "AND tracking = {$record['tracking']} "
						  . "AND t < {$record['t']} AND t > {$oRecord['t']}"
				);
				if (dbCount($q2)) {
					continue;
				}

				$found = $oRecord;
				break;
			}

			if (is_null($found)) {
				return null;
			}
		}

		return array($record, $found);
	}


	/** This method checks the database for logons that present the
	 * following characteristics compared to R1 and R2:
	 *		R3.account != R1.account 
	 *		R3.ip close to R1.ip
	 *		R3.browser =~ R1.browser
	 *		R3.tracking != R1.tracking
	 *		R3.tracking != R2.tracking
	 *		R3.timestamp BETWEEN R1.timestamp AND R2.timestamp
	 */
	private function findIntermediaryLogons($R1, $R2) {
		// Get all logons matching the specifications, except
		// for the part about the browser which must be checked
		// manually.
		$classC = substr($record['ip_addr'], 0, strrpos($record['ip_addr'], '.') - 1) . "%";
		$q = $this->db->query(
			"SELECT account, tracking, ip_addr, action, t FROM account_log "
				. "WHERE account <> {$R1['account']} AND tracking <> {$R1['tracking']} "
				  . "AND tracking <> {$R2['tracking']} AND ip_addr LIKE '$classC' "
				  . "AND t > {$R2['t']} AND t < {$R1['t']}"
		);
		if (!dbCount($q)) {
			return array();
		}

		// Check the browser names
		$results = array();
		$browser1 = $this->getBrowserName($R1['tracking']);
		while ($R3 = dbFetchHash($q)) {
			$browser2 = $this->getBrowserName($R3['tracking']);
			if ($browser1 == $browser2 || levenshtein($browser1, $browser2) <= 10) {
				array_push($results, $R3);
			}
		}

		return $results;
	}


	/** This method verifies a triplet of records and tries to determine
	 * whether they were attempts at "vicious multiing".
	 */
	public function checkViciousTriplet($R1, $R2, $R3) {
		// Check if there are previous records R4 such as:
		//	R4.account  = R3.account
		//	R4.tracking = R3.tracking
		//	R4.timestamp < R2.timestamp
		$q = $this->db->query(
			"SELECT COUNT(*) FROM account_log "
				. "WHERE account = {$R3['account']} AND tracking = {$R3['tracking']} "
				  . "AND t < {$R3['t']}"
		);
		list($R4Exists) = dbFetchArray($q);
		if ($R4Exists > 0) {
			return 0;
		}

		// Get browser names
		$browser1 = $this->getBrowserName($R1['tracking']);
		$browser2 = $this->getBrowserName($R2['tracking']);
		$browser3 = $this->getBrowserName($R3['tracking']);

		// Compute browser similarities for (R1,R2) and (R1,R3)
		$brSim12 = ($browser1 == $browser2) ? 2 : ((levenshtein($browser1, $browser2) <= 10) ? 1 : 0);
		$brSim13 = ($browser1 == $browser3) ? 2 : ((levenshtein($browser1, $browser3) <= 10) ? 1 : 0);
		$brSim23 = ($browser2 == $browser3) ? 2 : ((levenshtein($browser2, $browser3) <= 10) ? 1 : 0);

		// Compute IP similarities for (R1,R2) and (R1,R3)
		$ipSim12 = self::ipSimilarity($R1['ip_addr'], $R2['ip_addr']);
		$ipSim13 = self::ipSimilarity($R1['ip_addr'], $R3['ip_addr']);
		$ipSim23 = self::ipSimilarity($R2['ip_addr'], $R3['ip_addr']);

		if ($ipSim12 == 2 && $ipSim13 != 2 || $brSim12 == 2 && $brSim13 != 2) {
			return 0; // Probably not vicious multiing
		} elseif ($ipSim12 == 2 && $ipSim13 == 2 && $brSim12 == 2 && $brSim13 == 2) {
			return 3; // High probability
		} elseif ($ipSim12 == 1 && ($ipSim13 == 2 || $ipSim23 == 2) && $brSim12 == 2 && $brSim13 == 2) {
			return 3; // High probability
		} elseif ($ipSim12 == 1 && ($ipSim13 == 2 || $ipSim23 == 2) && $brSim12 == 1
				&& ($brSim13 == 2 || $brSim23 == 2)) {
			return 2; // Medium probability
		} else {
			return 1; // Low probability
		}
	}


	/**********************************************
	 * IN-GAME CHECKS                             *
	 **********************************************/

	/** This method lists players who should be further investigated
	 * by the Death of Rats.
	 */
	public function getInGameCheckList() {
		// Get all pairs of players which have a score higher than 75
		$q = $this->db->query(
			"SELECT account1, account2 FROM dor_multi_points WHERE points >= 75"
		);
		if (!dbCount($q)) {
			return array();
		}

		// Create a unique list
		$potentialChecks = array();
		$uniquePlayers = array();
		while ($r = dbFetchArray($q)) {
			sort($r);
			$id = join(',', $r);
			if (! array_key_exists($id, $potentialChecks)) {
				$potentialChecks[$id] = $r;
			}
			array_push($uniquePlayers, $r[0]);
			array_push($uniquePlayers, $r[1]);
		}
		$uniquePlayers = array_unique($uniquePlayers);

		// Get accounts that are no longer active
		$q = $this->db->query(
			"SELECT id FROM account "
				. "WHERE id IN (" . join(',', $uniquePlayers) . ") "
				  . "AND status NOT IN ('STD', 'VAC')"
		);
		$noCheckPlayers = array();
		while ($r = dbFetchArray($q)) {
			$noCheckPlayers[] = $r[0];
		}

		// Check accounts that have been punished recently
		$q = $this->db->query(
			"SELECT account FROM dor_punishment "
				. "WHERE account IN (" . join(',', $uniquePlayers) . ") "
				  . "AND {$this->now} - ts <= " . (self::punishmentDays * 86400)
		);
		while ($r = dbFetchArray($q)) {
			$noCheckPlayers[] = $r[0];
		}

		// Check pairs that have been warned recently
		$q = $this->db->query(
			"SELECT account1, account2 FROM dor_warning "
				. "WHERE {$this->now} - ts <= " . (self::postWarningGrace * 86400)
		);
		$noCheckPairs = array();
		while ($r = dbFetchArray($q)) {
			$noCheckPairs[] = join(',', $r);
		}

		// Get the list of recent checks on players
		$q = $this->db->query(
			"SELECT account1, account2 FROM dor_ingame_check "
				. "WHERE {$this->now} - ts <= " . (self::inGameCheckDelay * 3600)
				  . "AND message = 'CHECK' "
				. "GROUP BY account1, account2"
		);
		while ($r = dbFetchArray($q)) {
			$noCheckPairs[] = join(',', $r);
		}

		// Create the list of pairs to check
		$noCheckPlayers = array_unique($noCheckPlayers);
		$noCheckPairs = array_unique($noCheckPairs);
		$checkPairs = array();
		foreach ($potentialChecks as $id => $accounts) {
			if (in_array($id, $noCheckPairs) || in_array($accounts[0], $noCheckPlayers)
					|| in_array($accounts[1], $noCheckPlayers)) {
				continue;
			}
			array_push($checkPairs, $accounts);
		}

		return $checkPairs;
	}


	/** This method checks a pair of accounts for in-game suspicious behaviour.
	 */
	public function checkInGamePair($account1, $account2) {
		foreach (config::getGames() as $game) {
			if ($game->name == 'main' || ($game->status() != 'RUNNING' && $game->status() != 'ENDING')) {
				continue;
			}

			if (! ($p1 = $this->doesUserPlay($game, $account1))
					|| ! ($p2 = $this->doesUserPlay($game, $account2))) {
				continue;
			}

			// Get events and log them
			$events = $this->runGameChecks($game, $p1, $p2);
			if (count($events) > 1) {
				l::trace("  Found " . (count($events) - 1) . " suspicious event(s)");
			}
			foreach ($events as $eType) {
				$this->inGameLog($game->name, $eType, $account1, $account2);
			}
		}
	}

	/** This method checks whether an account plays a specific game.
	 */
	private function doesUserPlay($game, $account) {
		if (is_null($this->userPlays[$game->name . "-" . $account])) {
			$this->userPlays[$game->name . "-" . $account] = $game->getLib()->call(
				'doesUserPlay', $account
			);
		}
		return $this->userPlays[$game->name . "-" . $account];
	}

	/** This method runs checks on a pair of players in a specific game.
	 */
	private function runGameChecks($game, $p1, $p2) {
		l::trace(" Investigating players #$p1 and #$p2 in game \"" . $game->text . "\"");
		return $game->getLib()->call('investigate', $p1, $p2,
			$this->now, self::inGameCheckDelay * 3600);
	}


	/**********************************************
	 * LOG METHODS FOR PLAYER ENTRIES             *
	 **********************************************/

	private function initSinglePlayerLog() {
		if (!is_null($this->spLog)) {
			return;
		}
		$this->spLog = new db_copy("dor_single", db_copy::copyTo);
		$this->spLog->setAccessor($this->db);
	}

	private function singlePlayerLog($logType, $id) {
		$this->initSinglePlayerLog();
		$this->spLog->appendRow(array(
			$logType, $id, $this->now
		));
		$this->nEvents ++;
	}

	private function flushSinglePlayerLog() {
		if (is_null($this->spLog)) {
			return;
		}
		$this->spLog->execute();
		$this->spLog = null;
	}

	/**********************************************
	 * LOG METHODS FOR PLAYER/PLAYER ENTRIES      *
	 **********************************************/

	private function initMultiPlayerLog() {
		if (!is_null($this->mpLog)) {
			return;
		}
		$this->mpLog = new db_copy("dor_multi", db_copy::copyTo);
		$this->mpLog->setAccessor($this->db);
	}

	private function multiPlayerLog($logType, $id1, $id2) {
		$this->initMultiPlayerLog();
		$this->mpLog->appendRow(array(
			$logType, $id1, $id2, $this->now
		));
		$this->mpLog->appendRow(array(
			$logType, $id2, $id1, $this->now
		));
		$this->nEvents ++;
	}

	private function flushMultiPlayerLog() {
		if (is_null($this->mpLog)) {
			return;
		}
		$this->mpLog->execute();
		$this->mpLog = null;
	}

	/**********************************************
	 * LOG METHODS FOR IN-GAME CHECKS             *
	 **********************************************/

	private function initInGameLog() {
		if (!is_null($this->igLog)) {
			return;
		}
		$this->igLog = new db_copy("dor_ingame_check", db_copy::copyTo);
		$this->igLog->setAccessor($this->db);
	}

	private function inGameLog($game, $logType, $id1, $id2) {
		$this->initInGameLog();
		$this->igLog->appendRow(array(
			$id1, $id2, $logType, $this->now, $game
		));
	}

	private function flushInGameLog() {
		if (is_null($this->igLog)) {
			return;
		}
		$this->igLog->execute();
		$this->igLog = null;
	}
}

?>

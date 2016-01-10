<?php


class db_srv_exception extends Exception {

	public function __construct($message, $code = 0) {
		parent::__construct("Database server error ($message)", $code);
	}

}

class db_sql_exception extends Exception {

	private $query;

	public function __construct($query, $message = "") {
		parent::__construct("SQL query exception" . ($message != '' ? ": $message" : ""));
		$this->query = $query;
	}


	public function getQuery() {
		return $this->query;
	}

}

class db_deadlock_exception extends Exception {

	public function __construct($error) {
		parent::__construct("Deadlock detected ($error)");
	}

}



class db {

	static	$database	= null;
	static	$accessors	= array();

	private	$isOpen		= false;
	private $cString	= '';
	private $conn		= null;
	private $inTrans	= false;
	private $queries	= 0;
	private $__needsReset	= false;
	private $useExceptions	= 0;
	private $trace		= false;


	static function connect() {
		if (self::$database && ! self::$database->open()) {
			throw new db_srv_exception("unable to connect");
		} else {
			new db();
		}
		return self::$database;
	}

	public function __construct() {
		if (!$this->open()) {
			throw new db_srv_exception("unable to connect");
		}
		self::$database = $this;
	}

	public function open() {
		if ($this->isOpen) {
			return true;
		}

		// Get access to the 'main' game
		$main = config::getGame('main');
		if (!$main) {
			if ($this->useExceptions) {
				throw new db_srv_exception("failed to fetch 'main' pseudo-game");
			}
			return false;
		}

		// Generate the connection string
		if ($this->cString == '') {
			$cString = "dbname='{$main->params['dbname']}'";
			if ($main->params['dbhost'] != '') {
				$cString .= " host='{$main->params['dbhost']}' sslmode='disable'";
			}
			if ($main->params['dbuser'] != '') {
				$cString .= " user='{$main->params['dbuser']}'";
			}
			if ($main->params['dbpass'] != '') {
				$cString .= " password='{$main->params['dbpass']}'";
			}
			$this->cString = $cString;
		}

		// Connect to the database
		ob_start();
		$this->conn = pg_connect($this->cString);
		$error = ob_get_contents();
		ob_end_clean();

		// Check status
		if (!$this->conn || pg_connection_status($this->conn) == PGSQL_CONNECTION_BAD) {
			$this->conn = null;
			if ($this->useExceptions) {
				throw new db_srv_exception("database connection failed: $error");
			}
			return false;
		}

		$this->isOpen = true;
		$this->inTrans = false;
		$this->queries = 1;
		if (!@pg_query($this->conn, 'SET search_path=public,main')) {
			$error = pg_last_error();
			@pg_close($this->conn);
			$this->conn = null;
			if ($this->useExceptions) {
				throw new db_sql_exception('SET search_path=public,main',
					"failed to initialise search path: $error");
			}
			return false;
		}
		$this->begin();

		return true;
	}


	public function close() {
		if (! $this->isOpen) {
			return;
		}

		$this->end();
		@pg_close($this->conn);
		$this->isOpen = false;
		self::$accessors = array();

		if ($this->queries >= 20) {
			l::debug("SQL: connection closed after {$this->queries} queries");
		}
	}


	public function enableExceptions() {
		$this->useExceptions ++;
	}

	public function disableExceptions() {
		if ($this->useExceptions > 0) {
			$this->useExceptions --;
		}
	}


	private function fail($logTxt, $query) {
		if ($this->error) {
			if ($this->useExceptions) {
				throw new db_sql_exception($query, $logText);
			}
			return;
		}
		l::error("SQL: $logTxt");
		l::info("QUERY: $query");
		l::backtrace();
		$this->error = true;
		if ($this->useExceptions) {
			throw new db_sql_exception($query, $logTxt);
		}
	}


	public function begin() {
		if ($this->inTrans || $this->__needsReset) {
			return;
		}
		if (@pg_query($this->conn, "BEGIN TRANSACTION")) {
			$this->queries ++;
			$this->inTrans = true;
			$this->error = false;
		} else {
			l::error("SQL: could not start transaction");
			l::info("SQL: error was " . ($error = pg_last_error($this->conn)));
			if (pg_connection_status($this->conn) == PGSQL_CONNECTION_BAD) {
				$this->__needsReset = true;
				if ($this->useExceptions) {
					throw new db_srv_exception($error);
				}
			} else {
				if ($this->useExceptions) {
					throw new db_sql_exception("BEGIN TRANSACTION", $error);
				}
			}
		}
	}

	public function end($rollback = false) {
		if (!$this->inTrans) {
			return true;
		}

		$r = ($rollback || $this->error);
		$query = $r ? "ROLLBACK" : "COMMIT";
		if (@pg_query($this->conn, $query)) {
			$this->queries ++;
			$this->inTrans = false;
		} else {
			$cStat = pg_connection_status($this->conn);
			$error = pg_last_error($this->conn);
			if ($cStat == PGSQL_CONNECTION_BAD) {
				l::notice("SQL: could not end transaction, connection is in an invalid status");
				$this->__needsReset = true;
				if ($this->useExceptions) {
					throw new db_srv_exception($error);
				}
			} else {
				l::error("SQL: could not end transaction while " . ($r ? "rolling back" : "comitting"));
				l::info("SQL ERROR: $error");
				l::backtrace();
				if ($this->useExceptions) {
					throw new db_sql_exception($query, $error);
				}
			}
			return false;
		}
		return true;
	}

	public function query($query) {
		if (!$this->inTrans) {
			$this->fail("query executed outside of transaction", $query);
			return null;
		}

		$this->queries ++;
		if ($this->trace) {
			l::trace("EXECUTE: $query");
		}
		$r = @pg_query($this->conn, $query);
		if (!$r) {
			$cStat = pg_connection_status($this->conn);
			$error = pg_last_error($this->conn);
			if ($cStat == PGSQL_CONNECTION_BAD) {
				l::notice("SQL: connection is gone while executing query");
				l::debug("QUERY: $query");
				$this->__needsReset = true;
				$this->error = true;
				if ($this->useExceptions) {
					throw new db_srv_exception($error);
				}
				return;
			} elseif (preg_match('/deadlock/i', $error)) {
				l::error("SQL: deadlock detected");
				if ($this->useExceptions) {
					throw new db_deadlock_exception($error);
				}
				$this->error = true;
				return;
			} else {
				$this->fail("an error has occured: $error", $query);
			}
		} elseif (preg_match('/^\s*insert\s+into ("?\w+"?)/i', $query, $match)) {
			pg_free_result($r);

			$tn = $match[1];
			if ($tn{0} == '"') {
				$tn = str_replace('"', '', $tn);
			} else {
				$tn = strtolower($tn);
			}

			$r2 = @pg_query("SELECT last_inserted('$tn')");
			if ($r2) {
				$rv = pg_fetch_row($r2);
				if (is_null($rv[0])) {
					$r = true;
				} else {
					$r = $rv[0];
				}
				pg_free_result($r2);
			} elseif (preg_match('/deadlock/i', $error)) {
				l::error("SQL: deadlock detected");
				if ($this->useExceptions) {
					throw new db_deadlock_exception($error);
				}
				$this->error = true;
				return;
			} else {
				$cStat = pg_connection_status($this->conn);
				$error = pg_last_error($this->conn);
				if ($cStat == PGSQL_CONNECTION_BAD) {
					l::notice("SQL: connection is gone while fetching last ID");
					$this->__needsReset = true;
					if ($this->useExceptions) {
						throw new db_srv_exception($error);
					}
				} else {
					$this->fail("failed to fetch last ID: $error", "SELECT last_inserted('$tn')");
				}
				$r = null;
			}
		}
		return $r;
	}

	public function getGameAccess($prefix) {
		if (is_null($this->accessors[$prefix])) {
			$this->accessors[$prefix] = new db_accessor($this, $prefix);
		}
		return $this->accessors[$prefix];
	}

	public function needsReset() {
		return $this->__needsReset;
	}

	public function reset($attempts = 5, $delay = 1) {
		l::notice("SQL: resetting connection after {$this->queries} queries");
		$this->__needsReset = false;
		$i = 0;
		do {
			$rv = @pg_connection_reset($this->conn);
			if (!$rv) {
				$error = pg_last_error($this->conn);
				$i ++;
				l::warn("Reconnection attempt #$i failed" . ($i < $attempts ? ", retrying" : ""));
				sleep($delay);
			}
		} while ($i < $attempts && !$rv);
		if ($rv) {
			$this->error = false;
			$this->inTrans = false;
			$this->queries = 0;
			$this->trace = true;
		} else {
			$this->isOpen = false;
			if ($this->useExceptions) {
				throw new db_srv_exception($error);
			}
		}
		return $rv;
	}


	public function copyTo($table, $data) {
		$actualData = array();
		foreach ($data as $row) {
			$actualRow = array();
			foreach ($row as $column) {
				if (is_null($column)) {
					$output = "\\N";
				} elseif (is_string($column)) {
					$output = preg_replace(
						array('/\\\\/', '/\x08/', '/\f/', '/\r/', '/\\n/', '/\t/', '/\v/'),
						array('\\\\\\', '\\b', '\\f', '\\r', '\\n', '\\t', '\\v'),
						$column
					);
				} else {
					$output = $column;
				}
				array_push($actualRow, $output);
			}
			array_push($actualData, join("\t", $actualRow));
		}

		$result = @pg_copy_from($this->conn, $table, $actualData);
		if (!$result) {
			$cStat = pg_connection_status($this->conn);
			$error = pg_last_error($this->conn);
			if ($cStat == PGSQL_CONNECTION_BAD) {
				l::notice("SQL: connection is gone while copying into table '$table'");
				$this->__needsReset = true;
				$this->error = true;
				if ($this->useExceptions) {
					throw new db_srv_exception($error);
				}
				return false;
			} elseif (preg_match('/deadlock/i', $error)) {
				l::error("SQL: deadlock detected while copying into table '$table'");
				if ($this->useExceptions) {
					throw new db_deadlock_exception($error);
				}
				$this->error = true;
				return false;
			} else {
				$this->fail("an error has occured: $error", "COPY \"$table\" FROM STDIN");
			}
		}

		return true;
	}

	public function copyFrom($table) {
		$result = pg_copy_to($this->conn, $table);
		if ($result === FALSE) {
			$cStat = pg_connection_status($this->conn);
			$error = pg_last_error($this->conn);
			if ($cStat == PGSQL_CONNECTION_BAD) {
				l::notice("SQL: connection is gone while copying from table '$table'");
				$this->__needsReset = true;
				$this->error = true;
				if ($this->useExceptions) {
					throw new db_srv_exception($error);
				}
				return false;
			} elseif (preg_match('/deadlock/i', $error)) {
				l::error("SQL: deadlock detected while copying from table '$table'");
				if ($this->useExceptions) {
					throw new db_deadlock_exception($error);
				}
				$this->error = true;
				return false;
			} else {
				$this->fail("an error has occured: $error", "COPY \"$table\" TO STDOUT");
			}
		}

		$actualData = array();
		foreach ($result as $row) {
			$actualRow = array();
			$splitRow = explode("\t", $row);
			foreach ($splitRow as $column) {
				if ($column == "\\N") {
					$output = null;
				} else {
					$output = preg_replace(
						array('/\\\\b/', '/\\\\f/',
							'/\\\\r/', '/\\\\n/', '/\\\\t/', '/\\\\v/', '/\\\\\\\\/'),
						array("\x08", "\x0c", "\r", "\n", "\t", "\x0b", "\\"),
						$column
					);
				}
				array_push($actualRow, $output);
			}
			array_push($actualData, $actualRow);
		}

		return $actualData;
	}
}


?>

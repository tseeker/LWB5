<?php

class db_accessor {
	private static	$lastNamespace	= null;
	private		$db;
	private		$namespace;

	public function __construct($db, $namespace) {
		$this->db = $db;
		$this->namespace = $namespace;
	}

	public function setNamespace() {
		if (self::$lastNamespace === $this->namespace) {
			return;
		}

		if (is_null($this->namespace) || $this->namespace == '') {
			$this->db->query('SET search_path=public,main');
		} else {
			$this->db->query('SET search_path=public,main,' . $this->namespace);
		}

		self::$lastNamespace = $this->namespace;
	}

	public function enableExceptions() {
		$this->db->enableExceptions();
	}

	public function disableExceptions() {
		$this->db->disableExceptions();
	}

	public function query($q) {
		$this->setNamespace();
		return $this->db->query($q);
	}

	public function begin() {
		$this->db->begin();
	}

	public function end($rb = false) {
		return $this->db->end($rb);
	}

	public function needsReset() {
		return $this->db->needsReset();
	}

	public function reset($attempts = 5, $delay = 1) {
		if ($rv = $this->db->reset($attempts, $delay)) {
			self::$lastNamespace = false;
		}
		return $rv;
	}


	/** This method starts a transaction and runs in safely, trying to reset the connection
	 * if it fails.
	 */
	public function safeTransaction($callback, $args = null, $maxResets = 1,
				$attempts = 5, $delay = 1, $maxDeadlocks = 10) {

		if (is_null($args)) {
			$args = array();
		} elseif (!is_array($args)) {
			$args = array($args);
		}

		$resets = $deadlocks = 0;
		$this->enableExceptions();

		try {
			do {
				$ok = false;
				try {
					$this->begin();
					$returnValue = call_user_func_array($callback, $args);
					$this->end();
					$ok = true;
				} catch (db_deadlock_exception $e) {
/*					$this->end(false);
					self::$lastNamespace = false;
if ($deadlocks == $maxDeadlocks) { */
						throw $e;
//					}
//					$deadlocks ++;
				} catch (db_sql_exception $e) {
					$this->end();
					throw $e;
				} catch (db_srv_exception $e) {
					if ($resets == $maxResets) {
						throw $e;
					}
					$this->reset($attempts, $delay);
					$resets ++;
				}
			} while (!$ok);
		} catch (Exception $e) {
			$this->disableExceptions();
			throw $e;
		}

		$this->disableExceptions();
		return $returnValue;
	}


	/** This method copies raw data into a table.
	 */
	public function copyTo($tableName, $data) {
		$this->setNamespace();
		return $this->db->copyTo($tableName, $data);
	}


	/** This method copies raw data from a table.
	 */
	public function copyFrom($tableName) {
		$this->setNamespace();
		return $this->db->copyFrom($tableName);
	}
}

?>

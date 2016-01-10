<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// lib/db_copy.inc
//
// This library adds support for mass insertions into the database.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class db_copy {

	/* COPY MODES */
	const copyTo		= 0;		// Mass-insert data into the table
	const copyToClean	= 1;		// Similar to copyTo, but delete the table's contents first
	const copyFrom		= 2;		// Mass-reads the table's contents
	/**************/

	private $table;
	private $mode;
	private $db;
	private $data;
	private $colNames;


	/** Initialises the instance by specifying a table name and an optional copy mode.
	 * If no mode is specified, it defaults to copyFrom (mass read).
	 * The constructor can also be called with another db_copy instance as its first
	 * argument; in that case, it copies this instance's data; the mode must be either
	 * copyTo or copyToClean.
	 */
	public function __construct($table, $mode = self::copyFrom) {
		if (is_string($table)) {
			$this->table = $table;
			$this->mode = $mode;
			$this->db = $this->data = $this->colNames = null;
		} elseif (is_object($table) && $table instanceof db_copy) {
			$this->table = $table->table;
			$this->mode = ($mode != self::copyFrom ? $mode : self::copyTo);
			$this->db = $table->db;
			$this->data = $table->data;
			$this->colNames = $table->colNames;
		} else {
			throw new Exception("Invalid parameters to the db_copy constructor");
		}
	}


	/** This method sets the database accessor to use when performing the copy
	 * operation.
	 */
	public function setAccessor($db) {
		if (!is_object($db) || !(($db instanceof db) || ($db instanceof db_accessor))) {
			throw new Exception("Invalid database accessor: " . gettype($db)
				. (is_object($db) ? (" (" . get_class($db) . ")") : "") );
		}
		$this->db = $db;
	}


	/** This method executes the operation. It returns true on success or false
	 * on error. However, if database exceptions are enabled, SQL failure will
	 * cause an exception to be thrown.
	 */
	public function execute() {
		if (is_null($this->db)) {
			l::debug("db_copy::execute() called but no accessor set");
			return false;
		}

		if ($this->mode == self::copyFrom) {
			if (!is_null($this->data)) {
				return true;
			}
			$this->data = $this->db->copyFrom($this->table);
			return !is_null($this->data);
		}

		if (is_null($this->data)) {
			return false;
		}

		if ($this->mode == self::copyToClean) {
			$this->db->query("DELETE FROM \"{$this->table}\"");
		}
		return $this->db->copyTo($this->table, $this->data);
	}


	/** This method is called with an arbitrary number of arguments. It sets
	 * the names for the columns, which is required for some of the operations.
	 */
	public function setColumnNames() {
		$n = func_num_args();
		if ($n == 0) {
			return;
		}

		$args = func_get_args();
		$names = array();
		for ($i = 0; $i < $n; $i ++) {
			if (!is_string($args[$i])) {
				return;
			}
			$names[$args[$i]] = $i;
		}

		$this->colNames = $names;
	}


	/** This method returns the amount of rows in the object.
	 */
	public function rows() {
		return is_null($this->data) ? 0 : count($this->data);
	}


	/** This method appends a row to the current data. The row must be passed
	 * as an array of values. This method will fail and return FALSE if the
	 * current mode is copyFrom or if the data is not an array.
	 */
	public function appendRow($data) {
		if (!is_array($data) || $this->mode == self::copyFrom) {
			return false;
		}
		if (!is_array($this->data)) {
			$this->data = array();
		}
		array_push($this->data, $data);
	}


	/** This method removes a row using its index. It will return FALSE if the
	 * index is invalid or if the current mode is copyFrom. In any other case
	 * it will return the row that has been removed.
	 */
	public function removeRow($index) {
		if ($this->mode == self::copyFrom || !is_int($index) || $index < 0
				|| is_null($this->data) || $index >= count($this->data)) {
			return false;
		}

		return array_splice($this->data, $index, 1);
	}


	/** This method returns a row from the current data. It will return FALSE
	 * if the index is invalid, or the row on success.
	 */
	public function getRow($index) {
		if (!is_int($index) || $index < 0 || is_null($this->data) || $index >= count($this->data)) {
			return false;
		}

		return $this->data[$index];
	}


	/** This method searches for rows using a column identifier (either an
	 * integer or, if the column names have been specified, a column name)
	 * and a value to look for. It returns FALSE if the column identifier
	 * is invalid, or an array of row indices if rows are found. The
	 * additional $unique parameter can specify that the value will be
	 * unique to prevent the method from going through all rows in that
	 * case; it is false by default.
	 */
	public function lookupRow($column, $value, $unique = false) {
		if (is_null($this->data)) {
			return false;
		}

		if (is_int($column)) {
			if ($column < 0) {
				return false;
			}
		} elseif (is_string($column)) {
			if (is_null($this->colNames) || !array_key_exists($column, $this->colNames)) {
				return false;
			}
			$column = $this->colNames[$column];
		} else {
			return false;
		}

		$results = array();
		for ($i = 0; $i < count($this->data); $i ++) {
			if (count($this->data[$i]) < $column) {
				return false;
			}
			if ($this->data[$i][$column] != $value) {
				continue;
			}

			array_push($results, $i);
			if ($unique) {
				break;
			}
		}

		return $results;
	}
}


?>

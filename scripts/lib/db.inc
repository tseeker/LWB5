<?php

/****************************************************************************
 * DATABASE FUNCTIONS
 ****************************************************************************/

function dbConnect($fatal = true) {
	if (is_null(db::$database)) {
		try {
			new db();
			$rv = true;
		} catch (Exception $e) {
			$rv = false;
		}
	} else {
		$rv = db::$database->open();
	}
	if ($fatal && !$rv) {
		l::fatal(1, array("SQL: Database connection failed", pg_last_error()));
	}
	return $rv;
}

function dbClose() {
	db::$database->close();
}

function dbQuery($q) {
	return db::$database->query($q);
}

function dbBegin() {
	db::$database->begin();
}

function dbEnd() {
	db::$database->end();
}



/** This function returns the results count for the $qr database query
 * result
 */
function dbCount($qr) {
	return @pg_num_rows($qr);
}


/** This function returns the next row from the $qr query result as a hash
 * table
 */
function dbFetchHash($qr) {
	return @pg_fetch_assoc($qr);
}


/** This function returns the next row from the $qr query result as an array */
function dbFetchArray($qr) {
	return @pg_fetch_row($qr);
}


/** This function turns a value into a DB-specific boolean value. */
function dbBool($v) {
	return $v ? "true" : "false";
}

?>

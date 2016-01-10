<?php

/****************************************************************************
 * STATIC RESOURCES MANAGEMENT
 ****************************************************************************/

global $resources;


/** This variable contains the list of resources to be loaded.
 */
$resources = array();


/** This function appends a resource of some type to the list of resources
 * to be loaded.
 */
function addFileResource($type, $path) {
	if (!file_exists($path)) {
		//logText("Resource type '$type': '$path' not found", LOG_DEBUG);
		return false;
	}

	global $resources;
	if (!is_array($resources[$type])) {
		$resources[$type] = array();
	}

	$contents = @file_get_contents($path);
	if ($contents === FALSE) {
		l::notice("Resource type '$type': unable to read '$path'");
		return false;
	}
	$md5 = md5($contents);

	array_push($resources[$type], array($path, $md5, $contents));
	return true;
}


/** This function appends a chunk of raw text to a resource output.
 */
function addRawResource($type, $text) {
	global $resources;
	if (!is_array($resources[$type])) {
		$resources[$type] = array();
	}

	$md5 = md5($text);
	array_push($resources[$type], array(null, $md5, $text));

	return true;
}


/** This function generates the cached output for a set of resources.
 */
function generateResourceCache($type, $md5) {
	if (!is_dir(config::$main['cachedir']) && !@mkdir(config::$main['cachedir'])) {
		return false;
	}

	$f = @fopen(config::$main['cachedir'] . "/$md5.$type", "w");
	if (!$f) {
		return false;
	}

	global $resources;
	$c = count($resources[$type]);
	for ($i=0;$i<$c;$i++) {
		fwrite($f, preg_replace('/__STATICURL__/', config::$main['staticurl'], $resources[$type][$i][2]) . "\n");
	}

	fclose($f);

	return true;
}


/** This function stores a ressource into the database so that it may be used
 * from other scripts.
 */
function storeResource($type, $deleteOld = 0, $write = true) {
	global $resources;
	if (!is_array($resources[$type])) {
//		l::notice("No resource of type $type");
		return null;
	}

	// Delete old entries
	if ($deleteOld > 0) {
		if ($write) {
			$q = dbQuery("SELECT md5 FROM web_cache WHERE unix_timestamp(now())-last_used>$deleteOld AND rtype='$type'");
			while ($r = dbFetchArray($q)) {
				@unlink(config::$main['cachedir'] . "/{$r[0]}.$type");
			}
		}
		dbQuery("DELETE FROM web_cache WHERE unix_timestamp(now())-last_used>$deleteOld AND rtype='$type'");
	}

	// Check for an existing entry
	$md5 = md5(serialize($resources[$type]));
	$q = dbQuery("SELECT id FROM web_cache WHERE rtype='$type' AND md5='$md5' FOR SHARE");
	if ($q && dbCount($q)) {
		list($id) = dbFetchArray($q);
		return $id;
	}

	// Generate the output file
	if ($write && !generateResourceCache($type, $md5)) {
		l::warn("Resource file generation failed (type $type)");
		return null;
	}

	// Add the database entry
	return dbQuery("INSERT INTO web_cache(rtype,md5,last_used) VALUES('$type','$md5'," . time() . ")");
}


/** This function reads a resource, identified by its DB identifier, from the
 * database. If the resource is found in the base, it then tries to send the
 * file's contents.
 */
function displayResource($id, $rtype) {
	$q = dbQuery("SELECT rtype,md5 FROM web_cache WHERE id=$id FOR UPDATE");
	if (!($q && dbCount($q) == 1)) {
		l::warn("Resource ID '$id' not in the database");
		return false;
	}

	list($dbtype,$md5) = dbFetchArray($q);
	if ($rtype != $dbtype) {
		l::warn("Resource ID '$id' has wrong type $dbtype (expected $rtype)");
		return false;
	}

	dbQuery("UPDATE web_cache SET last_used=" . time() . " WHERE id=$id");
	endRequest(false);

	$path = config::$main['cachedir'] . "/$md5.$rtype";
	if (readfile($path) === FALSE) {
		l::warn("File not found for resource '$id': $path");
		return false;
	}
	exit(0);
}


?>

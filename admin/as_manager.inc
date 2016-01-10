<?php

function __isManagerRunning() {
	global $aConfig;

	$fName = $aConfig['ctrlPath'] . "/tickManager.pid";
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


?>

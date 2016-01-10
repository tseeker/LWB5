<?php

$aConfig = array(
	"user"		=> 'legacyworlds',
	"password"	=> 'password for the standard database user',
	"adminUser"	=> 'legacyworlds_admin',
	"adminPassword"	=> 'password for the administrative database user',
	"ctrlFifo"	=>'/tmp/.lwFifo',
	"ctrlPath"	=>'/tmp/.lwControl'
);


function __dbConnect($admin = false) {
	global $aConfig;
	$u = $aConfig[$admin ? 'adminUser' : 'user'];
	$p = $aConfig[$admin ? 'adminPassword' : 'password'];
	$cString = "dbname='legacyworlds' host='127.0.0.1' sslmode='prefer' user='$u' password='$p'";
	return pg_connect($cString);
}


function __sendControl($command) {
	global $aConfig;
	$fName = $aConfig['ctrlFifo'];
	if (!file_exists($fName)) {
		return false;
	}

	$fifo = @fopen($fName, "w");
	if ($fifo === false) {
		return false;
	}
	fwrite($fifo, "$command\n");
	fclose($fifo);
	return true;
}


?>

<?php

function thm_getHeaderData() {
	$pLib = input::$game->getLib('beta5/player');
	$mLib = input::$game->getLib('beta5/msg');
	$player = $_SESSION[game::sessName()]['player'];

	$pi = $pLib->call('get', $player);
	$newMsg = ($mLib->call('getNew', $player) > 0) ? 1 : 0;
	return	time() . "#{$pi['name']}#{$pi['cash']}#$newMsg#{$pi['alliance']}";
}

function thm_getHeaderPList() {
	$pLib = input::$game->getLib('beta5/player');
	$pl = $pLib->call('getPlanets', $_SESSION[game::sessName()]['player']);

	$s = "";
	foreach ($pl as $id => $n) {
		$s .= ($s==""?"":"\n")."$id#$n";
	}
	return $s;
}

function thm_getHeaderFolders() {
	$mLib = input::$game->getLib('beta5/msg');
	$fl = $mLib->call('getCustomFolders', $_SESSION[game::sessName()]['player']);
	$s = "";
	foreach ($fl as $id => $n) {
		$s .= ($s == "" ? "" : "\n") . "$id#$n";
	}
	return $s;
}


return	array(
	"func"	=> array('getHeaderData','getHeaderPList','getHeaderFolders'),
	"init"	=> "thminv_writeHeader(document.getElementById('thm-hdr-init').value);thminv_writePlanets(document.getElementById('thm-plist-init').value);thminv_initFolders();"
);

?>

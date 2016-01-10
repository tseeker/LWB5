<?php

function thm_getHeaderData() {
	$pLib = input::$game->getLib('beta5/player');
	$mLib = input::$game->getLib('beta5/msg');
	$player = $_SESSION[game::sessName()]['player'];

	$pi = $pLib->call('get', $player);
	$newMsg = ($mLib->call('getNew', $player) > 0) ? 1 : 0;
	return	time() . "#{$pi['name']}#{$pi['cash']}#$newMsg#{$pi['alliance']}";
}


return	array(
	"func"	=> array('getHeaderData'),
	"init"	=> "thmcls_writeHeader(document.getElementById('thm-hdr-init').value);"
);

?>

<?php

/****************************************************************************
 * ACCOUNT MANAGEMENT FUNCTIONS
 ****************************************************************************/

function accountLog($what, $uid) {
	$game = config::getGame('main');
	$lib = $game->getLib('main/account');
	$lib->call('log', $uid, $what);
}

?>

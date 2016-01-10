<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}


include("config.inc");
include('as_manager.inc');

/* Insert "silent admins" into the new game's player table */
$cnx = __dbConnect();
if (!$cnx) {
	$argh = "Could not connect to the database";
} else {
	$ns = $_SESSION['lw_new_game']['found_id'];
	$error = false;
	foreach ($_SESSION['lw_new_game']['silent'] as $aId) {
		if (!pg_query($cnx, "INSERT INTO \"{$ns}\".player (userid, first_planet, hidden) VALUES ($aId, 1, TRUE)")) {
			$error = true;
			break;
		}
	}
	if ($error) {
		$argh = "Failed to insert silent admin data";
	} else {
		$argh = null;
	}
	pg_close($cnx);
}

if (!is_null($argh)) {
	include('cg_argh.inc');
	exit(0);
}


if (__isManagerRunning()) {
	$op = array(
		"pc"	=> 90,
		"text"	=> "Restarting ticks",
		"delay"	=> 1,
		"to"	=> "cg_step12.php"
	);
} else {
	$op = array(
		"pc"	=> 100,
		"text"	=> "Cleaning up",
		"delay"	=> 1,
		"to"	=> "cg_done.php"
	);
}

include('cg_operation.inc');

?>

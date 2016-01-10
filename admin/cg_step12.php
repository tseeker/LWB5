<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

include("config.inc");
include("as_manager.inc");
include("../scripts/config.inc");

if (!__isManagerRunning() || !file_exists($config['cachedir'] . "/ticks_stopped")) {
	$op = array(
		"pc"	=> 100,
		"text"	=> "Cleaning up",
		"delay"	=> 1,
		"to"	=> "cg_done.php"
	);
} else {
	touch($config['cachedir'] . "/start_ticks");

	$op = array(
		"pc"	=> 95,
		"text"	=> "Waiting for ticks to restart",
		"delay"	=> 1,
		"to"	=> "cg_step13.php"
	);
}

include('cg_operation.inc');

?>

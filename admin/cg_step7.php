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

if (__isManagerRunning() && !file_exists($config['cachedir'] . "/ticks_stopped")) {
	$op = array(
		"pc"	=> 55,
		"text"	=> "Waiting for ticks to stop",
		"delay"	=> 1,
		"to"	=> "cg_step7.php"
	);
} else {
	$op = array(
		"pc"	=> 60,
		"text"	=> "Generating configuration",
		"delay"	=> 1,
		"to"	=> "cg_step8.php"
	);
}

include('cg_operation.inc');

?>

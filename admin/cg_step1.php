<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}
$_SESSION['lw_new_game']['started'] = true;

include("../scripts/config.inc");
if (is_null($config['maintenance'])) {
	$text = "Enabling maintenance mode";
	$next = "cg_step2.php";
} else {
	$text = "Checking existing IDs";
	$next = "cg_step4.php";
}

$op = array(
	"pc"	=> 5,
	"text"	=> $text,
	"delay"	=> 1,
	"to"	=> $next
);

include('cg_operation.inc');

?>

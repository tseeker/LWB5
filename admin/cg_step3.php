<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

$t = time() - $_SESSION['lw_new_game']['menable'];
if ($t < 20) {
	$text = "Waiting ...";
	$next = "cg_step3.php";
} else {
	$text = "Checking existing IDs";
	$next = "cg_step4.php";
}

$op = array(
	"pc"	=> 10 + floor($t/2),
	"text"	=> $text,
	"delay"	=> 1,
	"to"	=> $next
);

include('cg_operation.inc');

?>

<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}


/* Execute an universe tick manually */
$ns = $_SESSION['lw_new_game']['found_id'];
$__runFromAdmin = true;
$__adminParams = array($ns, 'universe');
include("../scripts/ticks.php");


$op = array(
	"pc"	=> 85,
	"text"	=> "Adding silent admins",
	"delay"	=> 1,
	"to"	=> "cg_step11.php"
);

include('cg_operation.inc');

?>

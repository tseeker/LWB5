<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

include("config.inc");
$ns = $_SESSION['lw_new_game']['found_id'];

$file = fopen($aConfig['ctrlFifo'], "w");
fwrite($file, "MERGE $ns\n");
fclose($file);

$op = array(
	"pc"	=> 75,
	"text"	=> "Initialising universe",
	"delay"	=> 1,
	"to"	=> "cg_step10.php"
);

include('cg_operation.inc');

?>

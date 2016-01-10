<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

$maintenance = array(
	"until" => time() + 15 * 60,
	"reason" => "Creating game " . $_SESSION['lw_new_game']['name']
);

include("../scripts/config.inc");
$f = fopen($config['cachedir'] . '/maintenance.ser', "w");
fwrite($f, serialize($maintenance));
fclose($f);

$_SESSION['lw_new_game']['menable'] = time();
$op = array(
	"pc"	=> 10,
	"text"	=> "Waiting ...",
	"delay"	=> 1,
	"to"	=> "cg_step3.php"
);

include('cg_operation.inc');

?>

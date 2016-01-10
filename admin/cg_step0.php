<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || $_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}
$_SESSION['lw_new_game']['started'] = true;

include("as_log.inc");
__logAdmin("is creating game {$_SESSION['lw_new_game']['name']}");

$op = array(
	"pc"	=> 0,
	"text"	=> "Initialising ...",
	"delay"	=> 1,
	"to"	=> "cg_step1.php"
);

include('cg_operation.inc');

?>

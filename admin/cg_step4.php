<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

include("config.inc");
$cnx = __dbConnect();

$minId = 1;
$gameType = "b5" . ($_SESSION['lw_new_game']['game_type'] == 'r' ? 'r' : 'm');

do {
	$minId ++;
	$q = pg_query($cnx, "SELECT nspname FROM pg_namespace WHERE nspname='{$gameType}{$minId}'");
} while ($q && pg_num_rows($q) == 1);
pg_close($cnx);

if (!$q) {
	$argh = "Error while accessing the database";
	include('cg_argh.inc');
	exit(1);
}


$_SESSION['lw_new_game']['found_id'] = $gameType . $minId;

$op = array(
	"pc"	=> 25,
	"text"	=> "Inserting data for ID $gameType$minId",
	"delay"	=> 1,
	"to"	=> "cg_step5.php"
);

include('cg_operation.inc');

?>

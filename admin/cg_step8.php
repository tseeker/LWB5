<?php

set_magic_quotes_runtime(false);

session_start();
if (!is_array($_SESSION['lw_new_game']) || !$_SESSION['lw_new_game']['do_it_now'] || !$_SESSION['lw_new_game']['started']) {
	echo "Sorry, can't do that.";
	exit(0);
}

require_once("config.inc");

function makeCTFParams() {
	$cnx = __dbConnect();
	if (!$cnx) {
		print "<b>DATABASE CONNECTION ERROR</b>";
		exit(1);
	}
	$query = pg_query("SELECT * FROM main.ctf_map_def ORDER BY name");
	if (!$query) {
		print "<b>DATABASE ERROR</b>";
		exit(1);
	}

	$ctfMaps = array();
	while ($r = pg_fetch_assoc($query)) {
		$ctfMaps[$r['id']] = $r;
		$query2 = pg_query("SELECT COUNT(*) FROM main.ctf_map_layout WHERE map={$r['id']} AND spawn_here");
		if (!$query2) {
			print "<b>DATABASE ERROR</b>";
			exit(1);
		}
		list($ctfMaps[$r['id']]['players']) = pg_fetch_array($query2);
		pg_free_result($query2);
	}
	pg_free_result($query);
	pg_close($cnx);

	$map = $ctfMaps[$_SESSION['lw_new_game']['ctfmap']];
	$cParams = $_SESSION['lw_new_game']['ctfparams'];

	$params = array(
		'usemap'	=> $map['id'],
		'maxplayers'	=> $map['players'],
		'norealloc'	=> 1,
		'partialtechs'	=> 0,
		'lockalliances'	=> $map['alliances'],
		'alliancecap'	=> 0,
		'victory'	=> 2,
		'novacation'	=> 1
	);

	foreach ($cParams as $p => $v) {
		$params[$p] = $v;
	}

	return $params;
}


$ns = $_SESSION['lw_new_game']['found_id'];
$newConfig = "\t\t<!-- Game added automatically by the game creation tool -->\n"
	. "\t\t<Game id=\"$ns\" version=\"beta5\" namespace=\"$ns\" text=\""
	. $_SESSION['lw_new_game']['name'] . "\" public=\"0\" canjoin=\"0\">\n"
	. "\t\t\t<!-- Game parameters -->\n";

if ($_SESSION['lw_new_game']['game_type'] == 'c') {
	$_SESSION['lw_new_game']['params'] = makeCTFParams();
}

foreach ($_SESSION['lw_new_game']['params'] as $p => $v) {
	$newConfig .= "\t\t\t<Param name=\"$p\" value=\"$v\" />\n";
}
$newConfig .= "\n"
	. "\t\t\t<!-- Description -->\n"
	. "\t\t\t<Description lang=\"en\">\n"
	. "\t\t\t\t" . $_SESSION['lw_new_game']['descr'] . "\n"
	. "\t\t\t</Description>\n\n"
	. "\t\t\t<!-- Ticks -->\n\n";

require_once("cg_ticks_schedule.inc");
$ticks = __computeTicks($_SESSION['lw_new_game']['ft_y'],
	$_SESSION['lw_new_game']['ft_m'], $_SESSION['lw_new_game']['ft_d'],
	$_SESSION['lw_new_game']['speed'], $_SESSION['lw_new_game']['shift_ticks']);

foreach ($ticks as $tid => $data) {
	$time = $data[1];
	$time = ($time - ($secs = $time % 60)) / 60;
	$time = ($time - ($mins = $time % 60)) / 60;
	$newConfig .= "\t\t\t<!-- \"$tid\" tick: first tick " . gmstrftime("%H:%M:%S on %Y-%m-%d", $data[0])
		. ", interval {$time}h, {$mins}min, {$secs}s -->\n"
		. "\t\t\t<Tick script=\"$tid\" first=\"{$data[0]}\" interval=\"{$data[1]}\" />\n";
}
$newConfig .= "\t\t</Game>\n";

$file = fopen($aConfig['ctrlPath'] . "/config.$ns.xml", "w");
fwrite($file, $newConfig);
fclose($file);

$op = array(
	"pc"	=> 70,
	"text"	=> "Merging configuration",
	"delay"	=> 1,
	"to"	=> "cg_step9.php"
);

include('cg_operation.inc');

?>

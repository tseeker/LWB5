<?php

require_once("cg_ticks_schedule.inc");
require_once("config.inc");
require_once("as_manager.inc");
require_once("../scripts/config.inc");

$cnx = __dbConnect();
if (!$cnx) {
	print "<b>DATABASE CONNECTION ERROR</b>";
	exit(1);
}
$query = pg_query("SELECT id,name FROM main.account WHERE admin AND NOT name LIKE 'AI>%' ORDER BY name");
if (!$query) {
	print "<b>DATABASE CONNECTION ERROR</b>";
	exit(1);
}

$allAdmins = array();
while ($r = pg_fetch_row($query)) {
	$allAdmins[$r[0]] = $r[1];
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


function __start($title) {
?>
<h3><?=htmlentities($title)?></h3>
<table border="1" width="80%" style="margin: 5px 30px">
<?
}

function __line($title, $contents) {
?>
 <tr>
  <th align="right" width="50%"><?=htmlentities($title)?>: </th>
  <td><?=htmlentities($contents)?></td>
 </tr>
<?
}

function __end() {
	echo "</table>\n";
}

$params = array(
	array(
		'id'		=> 'maxplayers',
		'title'		=> 'Maximum amount of players',
	),
	array(
		'id'		=> 'minsystems',
		'title'		=> 'Minimum amount of free systems',
	),
	array(
		'id'		=> 'maxsystems',
		'title'		=> 'Maximum amount of systems',
	),
	array(
		'id'		=> 'partialtechs',
		'title'		=> 'Partial tech graph',
	),
	array(
		'id'		=> 'initialcash',
		'title'		=> 'Initial amount of money',
	),
	array(
		'id'		=> 'zonesize',
		'title'		=> 'Universe generator: zone size',
	),
	array(
		'id'		=> 'nebulaprob',
		'title'		=> 'Probability of a nebula',
	),
	array(
		'id'		=> 'norealloc',
		'title'		=> 'Prevent planet reallocation',
	),
	array(
		'id'		=> 'novacation',
		'title'		=> 'Disable vacation mode',
	),
	array(
		'id'		=> 'victory',
		'title'		=> 'Victory conditions',
	),
	array(
		'id'		=> 'lockalliances',
		'title'		=> 'Use predefined alliances',
	),
	array(
		'id'		=> 'alliancecap',
		'title'		=> 'Alliance capping %',
	)
);

$ctfParams = array(
	array(
		'id'		=> 'initialcash',
		'title'		=> 'Initial amount of money',
	),
	array(
		'id'		=> 'v2time',
		'title'		=> 'Time for victory',
	),
	array(
		'id'		=> 'v2grace',
		'title'		=> 'Grace period',
	),
	array(
		'id'		=> 'v2points',
		'title'		=> 'Points per victory',
	)
);

?>
<h2>Step 6 / 6 : recap</h2>
<p>Please check the information below <b>CAREFULLY</b>.</p>

<?php

$gTypes = array(
	'm'	=> 'Match',
	'r'	=> 'Round',
	'c'	=> 'Kings of the Hill'
);

__start("Main information");
__line("Game type", $gTypes[$_SESSION['lw_new_game']['game_type']]);
__line("Game name", $_SESSION['lw_new_game']['name']);
__line("Description", $_SESSION['lw_new_game']['descr']);
__end();

__start("Parameters");
if ($_SESSION['lw_new_game']['game_type'] == 'c') {
	$map = $ctfMaps[$_SESSION['lw_new_game']['ctfmap']];
	__line("Map name", $map['name']);
	if ($map['description'] != '') {
		__line("Description", $map['description']);
	}
	__line("Players", $map['players']);
	__line("Teams", $map['alliances']);
	__line("Size", $map['width'] . "x" . $map['height']);
	foreach ($ctfParams as $p) {
		__line($p['title'], $_SESSION['lw_new_game']['ctfparams'][$p['id']]);
	}
} else {
	foreach ($params as $p) {
		__line($p['title'], $_SESSION['lw_new_game']['params'][$p['id']]);
	}
}
__end();

$speeds = array("Normal", "2x", "3x", "4x");
__start("Ticks configuration");
__line("Speed", $speeds[$_SESSION['lw_new_game']['speed'] - 1]);
__line("First tick (YYYY-MM-DD)", $_SESSION['lw_new_game']['ft_y'] . "-" .
	$_SESSION['lw_new_game']['ft_m'] . "-" . $_SESSION['lw_new_game']['ft_d']);
__end();

$ticks = __computeTicks($_SESSION['lw_new_game']['ft_y'],
	$_SESSION['lw_new_game']['ft_m'], $_SESSION['lw_new_game']['ft_d'],
	$_SESSION['lw_new_game']['speed'], $_SESSION['lw_new_game']['shift_ticks']);

__start("Ticks schedule");
foreach ($ticks as $tid => $data) {
	__line("\"$tid\" / first tick", gmstrftime("%H:%M:%S on %Y-%m-%d", $data[0]));
	$time = $data[1];
	$time = ($time - ($secs = $time % 60)) / 60;
	$time = ($time - ($mins = $time % 60)) / 60;
	__line("\"$tid\" / interval", "{$time}h, {$mins}min, {$secs}s");
	__line("\"$tid\" / numeric values", $data[0] . " / " . $data[1]);
}
__end();

__start("Silent admins");
foreach ($allAdmins as $id => $name) {
	__line($name, in_array($id, $_SESSION['lw_new_game']['silent']) ? "YES" : "no");
}
__end();

__start("Server information");
__line("Maintenance mode", is_null($config['maintenance']) ? "No - will enable automatically (DANGEROUS)" : "yes");
__line("Ticks manager", __isManagerRunning() ? "running" : "stopped");
__end();

?>
<form method="GET" action="?">
 <p>
  <input type="submit" name="back" value="<< Back" />
  <input type="submit" name="go" value="Proceed >>" />
 </p>
</form>

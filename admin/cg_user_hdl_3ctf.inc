<?php

require_once("config.inc");
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


$pList = array('initialcash', 'v2time','v2grace','v2points');



if (!is_array($_SESSION['lw_new_game']['ctfparams'])) {
	$_SESSION['lw_new_game']['ctfparams'] = array();
}

if ($_POST['go']) {
	$errors = array();

	$mapID = (int) $_POST['map'];
	if (!array_key_exists($mapID, $ctfMaps)) {
		$errors[0] = 'map';
	} else {
		$_SESSION['lw_new_game']['ctfmap'] = $mapID;
	}

	foreach ($pList as $pid) {
		$val = $_POST["p_$pid"];
		if (strcmp($val, (string)(int)$val)) {
			array_push($errors, $pid);
		}
		$_SESSION['lw_new_game']['ctfparams'][$pid] = $val;
	}
	if (! count($errors)) {
		$_SESSION['lw_new_game']['step'] = 4;
	}
} elseif ($_POST['back']) {
	foreach ($pList as $pid) {
		$_SESSION['lw_new_game']['ctfparams'][$pid] = $_POST["p_$pid"];
	}
	$_SESSION['lw_new_game']['ctfmap'] = $_POST['map'];
	$_SESSION['lw_new_game']['step'] = 2;
}

?>

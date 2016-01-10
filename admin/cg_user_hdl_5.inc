<?php

require_once("config.inc");
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

pg_close($cnx);


$sa = $_POST['sa'];
if (!is_array($sa)) {
	$sa = array();
}
if ($_POST['go'] || $_POST['back']) {
	$_SESSION['lw_new_game']['silent'] = array();
	foreach ($sa as $saId) {
		if (array_key_exists($saId, $allAdmins)) {
			array_push($_SESSION['lw_new_game']['silent'], $saId);
		}
	}
} elseif (!is_array($_SESSION['lw_new_game']['silent'])) {
	$_SESSION['lw_new_game']['silent'] = array();
}

if ( $_POST['back']) {
	$_SESSION['lw_new_game']['step'] = 4;
} elseif ($_POST['go']) {
	$_SESSION['lw_new_game']['step'] = 6;
}

?>

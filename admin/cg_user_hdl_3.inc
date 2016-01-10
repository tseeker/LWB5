<?php


if ($_SESSION['lw_new_game']['game_type'] == 'c') {
	include('cg_user_hdl_3ctf.inc');
	return;
}



$pDefaults = array(
	'm' => array(
		'maxplayers'	=> -1,
		'minsystems'	=> 30,
		'maxsystems'	=> 30,
		'norealloc'	=> 1,
		'partialtechs'	=> 0,
		'initialcash'	=> 40000,
		'zonesize'	=> 1,
		'nebulaprob'	=> 15,
		'lockalliances'	=> 4,
		'alliancecap'	=> 0,
		'victory'	=> 1,
		'novacation'	=> 1,
		'prot_after'	=> 0,
		'prot_duration'	=> 0,
	),
	'r' => array(
		'maxplayers'	=> 0,
		'minsystems'	=> 40,
		'maxsystems'	=> 0,
		'norealloc'	=> 0,
		'partialtechs'	=> 1,
		'initialcash'	=> 0,
		'zonesize'	=> 1,
		'nebulaprob'	=> -1,
		'lockalliances'	=> 0,
		'alliancecap'	=> 20,
		'victory'	=> 0,
		'novacation'	=> 0,
		'prot_after'	=> 20,
		'prot_duration'	=> 20,
	)
);
$pList = array_keys($pDefaults['m']);


if (!is_array($_SESSION['lw_new_game']['params'])) {
	$_SESSION['lw_new_game']['params'] = array();
}

if ($_POST['gtdef']) {
	$_SESSION['lw_new_game']['params'] = $pDefaults[$_SESSION['lw_new_game']['game_type']];
} elseif ($_POST['go']) {
	$errors = array();
	foreach ($pList as $pid) {
		$val = $_POST["p_$pid"];
		if (strcmp($val, (string)(int)$val)) {
			array_push($errors, $pid);
		}
		$_SESSION['lw_new_game']['params'][$pid] = $val;
	}
	if (! count($errors)) {
		$_SESSION['lw_new_game']['step'] = 4;
	}
} elseif ($_POST['back']) {
	foreach ($pList as $pid) {
		$_SESSION['lw_new_game']['params'][$pid] = $_POST["p_$pid"];
	}
	$_SESSION['lw_new_game']['step'] = 2;
}

?>

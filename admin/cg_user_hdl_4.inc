<?php

if ($_GET['back'] || $_GET['go']) {
	$_SESSION['lw_new_game']['ft_y'] = $_GET['ft_y'];
	$_SESSION['lw_new_game']['ft_m'] = $_GET['ft_m'];
	$_SESSION['lw_new_game']['ft_d'] = $_GET['ft_d'];
	$_SESSION['lw_new_game']['shift_ticks'] = (int) $_GET['st'];
	$_SESSION['lw_new_game']['speed'] = $_GET['gs'];
}

if ($_GET['back']) {
	$_SESSION['lw_new_game']['step'] = 3;
} elseif ($_GET['go']) {
	$speedOk = in_array($_GET['gs'], array("1","2","3","4"));

	$mkts = mktime(7, 0, 0, (int)$_GET['ft_m'], (int)$_GET['ft_d'], (int)$_GET['ft_y']);
	if ($mkts === FALSE || $mkts <= time()) {
		$ftError = 1;
	}

	if ($_SESSION['lw_new_game']['shift_ticks'] < 0) {
		$_SESSION['lw_new_game']['shift_ticks'] = 0;
	}

	if ($speedOk && !$ftError) {
		$_SESSION['lw_new_game']['step'] = 5;
	}
}

?>

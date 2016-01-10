<?php

if (in_array($_GET['gt'], array('m','r', 'c'))) {
	$_SESSION['lw_new_game']['game_type'] = $_GET['gt'];
	$_SESSION['lw_new_game']['step'] = 2;
}

?>

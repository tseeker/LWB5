<?php

$_SESSION['lw_new_game']['do_it_now'] = false;
if ($_GET['back']) {
	$_SESSION['lw_new_game']['step'] = 5;
} elseif ($_GET['go']) {
	$_SESSION['lw_new_game']['step'] = 7;
}

?>

<?php

function __checktext($text, $minLen, $okChars) {
	if (strlen($text) < $minLen) {
		return 1;
	}
	if (preg_match('/[^' . $okChars . ']/', $text)) {
		return 2;
	}
	return 0;
}

if ($_POST['go']) {
	$n = $_SESSION['lw_new_game']['name'] = stripslashes($_POST['gn']);
	$d = $_SESSION['lw_new_game']['descr'] = stripslashes($_POST['gd']);
	$err1 = __checktext($n, 5, 'A-Za-z0-9\.\-\ \,\#');
	$err2 = __checktext($d, 15, 'A-Za-z0-9\\.\\- ,#!\'%');
	if (!($err1 || $err2)) {
		$_SESSION['lw_new_game']['step'] = 3;
	}
} elseif ($_POST['back']) {
	$_SESSION['lw_new_game']['name'] = stripslashes($_POST['gn']);
	$_SESSION['lw_new_game']['descr'] = stripslashes($_POST['gd']);
	$_SESSION['lw_new_game']['step'] = 1;
}

?>

<?php

class	page_handler
{
	var	$needsAuth = false;
	var	$ajax = array();

	function handle($input) {
		if (!is_null($_SESSION) && !is_null($_SESSION['userid'])) {
			l::notice("Player '{$_SESSION['login']}' (#{$_SESSION['userid']}) logged out");
		}
		session::kill();
		$this->output = "logout";
	}
}

?>

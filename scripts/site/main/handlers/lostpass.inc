<?php

class page_handler {
	var $needsAuth = false;
	var $ajax = array();

	function handle($input) {
		if ($_SESSION['userid']) {
			$this->output = "index";
			return;
		}

		$this->output = "lostpass";
		$this->data = $this->game->action('lostPassword', $input['u'], $input['m'], $input['c']);
	}
}

?>

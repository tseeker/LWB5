<?php

class page_handler {
	var $needsAuth = true;

	function handle($input) {
		$this->output = "play";
		$this->data = $this->game->action('joinGame', $_SESSION['userid'], $input['g'], is_null($input['c']), $input['p'], $input['n']);
	}
}

?>

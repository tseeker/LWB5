<?php

class page_handler {
	var $needsAuth = true;
	var $ajax = array();

	private function getCredits() {
		return dbFetchHash($this->game->getDBAccess()->query("SELECT resources_used, credits_obtained FROM credits WHERE account = {$_SESSION['userid']}"));
	}

	public function handle($input) {
		$this->output = "contrib";
		$this->data = $this->getCredits();
	}
}

?>

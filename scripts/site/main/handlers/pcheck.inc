<?php

class page_handler {
	public $engines = array('page');
	public $noTracking = true;

	public function handle($input) {
		$key = strtolower($input['k']);
		if (!$key || strlen($key) != 32 || preg_match('/[^a-z0-9]/', $key)) {
			$this->data = "X";
		} else {
			$this->data = "Key is $key";
		}
		$this->output = "pcheck";
	}
}

?>

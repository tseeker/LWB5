<?php

class page_handler {

	function handle($input) {
		tracking::$data['readDisclaimer'] = true;
		$this->output = "disclaimer";
		$this->data = false;
	}

}

?>

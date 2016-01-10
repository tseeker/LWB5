<?php

class page_handler {

	var $engines = array('xml');


	function xml($input) {
		$data = new data_leaf('MacWidget', config::getParam('widgetURL'));
		$data->setAttribute('latest', config::getParam('latestWidget'));
		$data->setAttribute('oldestOk', config::getParam('oldestWidget'));

		return $data;
	}

}

?>

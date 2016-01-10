<?php

class beta5_moving_library {
	var $index = array (
		'cloneObject',
		'computeTrajectory',
		'getLocation',
		'getTrajectory',
		'newObject',
		'redirect',
		'stop'
	);

	function beta5_moving_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}
}

?>

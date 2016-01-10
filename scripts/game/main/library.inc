<?php

class main_library {
	var $index	= array(
		'getTick',
		'getTicks',
		'isGameRunning',
		'preJoin',
		'requestGenPlanets',
		'sendMail'
	);


	function main_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function isValidAddress($mAddr) {
		return preg_match(
			'/^[A-Za-z0-9_\.\-\+]+@([A-Za-z0-9_\.\-\+]+)+\.[A-Za-z]{2,6}/',
			$mAddr
		);
	}

	function isValidName($uName) {
		$test = array('/[^A-Za-z0-9_\.\-\+@\/'."'".' ]/', '/\s\s+/', '/^\s/', '/\s$/');
		$r = true;
		for ($i=0;$r&&$i<count($test);$i++) {
			$r &= !preg_match($test[$i], $uName);
		}
		return $r;
	}
}

?>

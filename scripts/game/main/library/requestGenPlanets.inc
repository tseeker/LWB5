<?php

class main_requestGenPlanets {

	function main_requestGenPlanets($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($first, $amount) {
		$dir = config::getParam('pgenreq');
		$game = $this->lib->game->name;
		if (!is_dir($dir)) {
			$base = dirname($dir);
			if (!(is_dir($base) && is_writable($base) && @mkdir($dir))) {
				logText("main::requestGenPlanets($game,$first,$amount): unable to create directory '$dir'", LOG_WARNING);
				return false;
			}
		}

		$r = @touch("$dir/req-$game-$first-$amount");
		if (!$r) {
			logText("main::requestGenPlanets($game,$first,$amount): unable to create file 'req-$game-$first-$amount'", LOG_WARNING);
		}
		return $r;
	}
}

?>

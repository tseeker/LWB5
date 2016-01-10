<?php

class main_isGameRunning {

	function main_isGameRunning($lib) {
		$this->lib	= $lib;
	}

	function run($game) {
		$g = config::getGame($game);
		if (!$g) {
			return false;
		}

		$lib = $g->getLib();
		if ($lib->call('isFinished')) {
			return false;
		}

		$now = time();
		foreach ($g->ticks as $td => $tick) {
			if ($tick->first <= $now && $tick->definition->public) {
				return true;
			}
		}

		return false;
	}
}

?>

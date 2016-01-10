<?php

class main_getTick {

	function main_getTick($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($name, $lang = null) {
		$tInst = $this->lib->game->ticks[$name];
		if (is_null($tInst)) {
			return null;
		}

		$tick = array(
			"game"		=> $tInst->definition->public,
			"first"		=> $tInst->first,
			"last"		=> $tInst->last,
			"interval"	=> $tInst->interval
		);

		if (!is_null($lang)) {
			$tick['name'] = $tInst->definition->getName($lang);
			$tick['description'] = $tInst->definition->getDescription($lang);
		}

		return $tick;
	}
}

?>

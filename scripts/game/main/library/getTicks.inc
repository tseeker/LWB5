<?php

class main_getTicks {

	function main_getTicks($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($lang = null) {
		$ticks = array();
		foreach ($this->lib->game->ticks as $script => $tInst) {
			$tick = array(
				"script"	=> $script,
				"game"		=> $tInst->definition->public,
				"first"		=> $tInst->first,
				"last"		=> $tInst->last,
				"interval"	=> $tInst->interval
			);

			if (!is_null($lang)) {
				$tick['name'] = $tInst->definition->getName($lang);
				$tick['description'] = $tInst->definition->getDescription($lang);
			}
			array_push($ticks, $tick);
		}
		return $ticks;
	}
}

?>

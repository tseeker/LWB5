<?php

// Empty tick, to be run every 5 seconds. Should make the scheduler
// react faster if there are no ticks.

class main_ticks_mark_library {

	public function __construct($lib) { }
	public function runTick() { }
}

?>

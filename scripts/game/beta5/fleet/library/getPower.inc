<?php

class beta5_fleet_getPower {
	var $ePower	= array();

	function beta5_fleet_getPower($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Computes a fleet's power
	function run($pl, $g, $f, $c, $b) {
		if (!is_array($this->ePower[$pl])) {
			$r = $this->rules->call('get', $pl);
			$a = array('gaship','fighter','cruiser','bcruiser');
			$this->ePower[$pl] = array();
			foreach ($a as $st) {
				$this->ePower[$pl][$st] = floor($r[$st."_power"] * $r['effective_fleet_power'] / 100);
			}
		}
		$r = $this->ePower[$pl];
		return $g * $r['gaship'] + $f * $r['fighter'] + $c * $r['cruiser'] + $b * $r['bcruiser'];
	}
}

?>

<?php

class beta5_rules_library {
	var $index = array(
		'change',
		'get'
	);


	function beta5_rules_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// RULE HANDLERS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Handler for modifications on planet_max_pop
	function rhUpdateMaxPopulation($pid, $maxPop) {
		$q = dbQuery("SELECT id FROM planet WHERE owner=$pid AND max_pop<".($maxPop-1500));
		while ($r = dbFetchArray($q)) {
			$this->planets->call('updateMaxPopulation', $r[0], $pid, $pid);
		}
	}

	// Handler for modifications on the unhappiness factor
	function rhUpdateHappiness($pid, $junk) {
		$q = dbQuery("SELECT id FROM planet WHERE owner=$pid");
		while ($r = dbFetchArray($q)) {
			$this->planets->call('updateHappiness', $r[0]);
		}
	}

	// Handler for modifications on ECM or ECCM techs
	function rhUpdateCommTech($pid, $junk) {
		$q = dbQuery("SELECT location FROM fleet WHERE owner=$pid AND location IS NOT NULL GROUP BY location");
		while ($r = dbFetchArray($q)) {
			$this->planets->call('updateMilStatus', $r[0]);
		}
	}

	// Handler for modifications on fleet power rules
	function rhUpdateFleetPower($pid, $junk) {
		$this->rhUpdateHappiness($pid, '');
		$this->rhUpdateCommTech($pid, '');
	}
}

?>

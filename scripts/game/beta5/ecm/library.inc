<?php

class beta5_ecm_library {
	var $index	= array();
	var $ecmTable	= array();
	var $eccmTable	= array();
	var $probTables	= array();

	function beta5_ecm_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function getInformationLevel($ecm, $eccm) {
		// Get the probability table
		$probTable = $this->getProbabilityTable($ecm, $eccm);

		// Randomly selects an information level
		$p = rand(0,9999) / 10000;
		$i = 0;
		while ($p > $probTable[$i]) {
			$p -= $probTable[$i];
			$i ++;
		}

		return $i;
	}


	function getProbabilityTable($ecm, $eccm) {
		if (is_null($this->probTables[$ecm.','.$eccm])) {
			// Read ECM probability table
			$ecmTable = $this->getECMTable($ecm);

			// Read ECCM probability table
			$eccmTable = $this->getECCMTable($eccm);

			// Create combined probabilities
			$probTable = array();
			$tot = 0;
			for ($i=0;$i<4;$i++) {
				$probTable[$i] = 0;
				for ($j=0;$j<=$i;$j++) {
					$probTable[$i] += $ecmTable[$i-$j] * $eccmTable[$j];
				}
				$tot += $probTable[$i];
			}
			$probTable[4] = 1 - $tot;

			$this->probTables[$ecm.','.$eccm] = $probTable;
		}

		return $this->probTables[$ecm.','.$eccm];
	}


	function getECMTable($level) {
		if (is_null($this->ecmTable[$level])) {
			$qEcm = $this->db->query("SELECT probability FROM ecm WHERE ecm_level=$level ORDER BY info_level ASC");
			if (!($qEcm && dbCount($qEcm))) {
				return -1;
			}
			$this->ecmTable[$level] = array();
			while ($r = dbFetchArray($qEcm)) {
				array_push($this->ecmTable[$level], $r[0] / 100);
			}
		}
		return $this->ecmTable[$level];
	}


	function getECCMTable($level) {
		if (is_null($this->eccmTable[$level])) {
			$qEccm = $this->db->query("SELECT probability FROM eccm WHERE eccm_level=$level ORDER BY gain ASC");
			if (!($qEccm && dbCount($qEccm))) {
				return -1;
			}
			$this->eccmTable[$level] = array();
			while ($r = dbFetchArray($qEccm)) {
				array_push($this->eccmTable[$level], $r[0] / 100);
			}
		}
		return $this->eccmTable[$level];
	}


	function scrambleFleetPower($level, $power) {
		switch	($level) :
			case 0: return 'NULL';
			case 1: $rand = rand(0,49) - 25; break;
			case 2: $rand = rand(0,19) - 10; break;
			case 3: case 4: $rand = 0; break;
		endswitch;
		return $power + ($power * $rand / 100);
	}
}

?>

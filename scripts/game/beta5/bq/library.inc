<?php

class beta5_bq_library {
	var $types	= array('turret', 'gaship', 'fighter', 'cruiser', 'bcruiser');
	var $qLen	= array();

	var $index	= array (
		'append',
		'flush',
		'get',
		'getReplacementCost',
		'remove',
		'reorder',
		'replace'
	);

	function beta5_bq_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	// Returns the length of a planet's build queue
	function getLength($pl) {
		if (is_null($this->qLen[$pl])) {
			$q = $this->db->query("SELECT COUNT(*) FROM buildqueue WHERE planet = $pl");
			list($this->qLen[$pl]) = dbFetchArray($q);
		}
		return $this->qLen[$pl];
	}

	// Moves an item down the build queue
	function moveDown($pl, $it) {
		$bql = $this->lib->call('getLength', $pl);
		$np = $it + 1;
		$this->db->query("UPDATE buildqueue SET bq_order = $bql WHERE planet = $pl AND bq_order = $it");
		$this->db->query("UPDATE buildqueue SET bq_order = $it WHERE planet = $pl AND bq_order = $np");
		$this->db->query("UPDATE buildqueue SET bq_order = $np,workunits=0 WHERE planet = $pl AND bq_order = $bql");
	}

	// Moves an item up the build queue
	function moveUp($pl, $it) {
		$bql = $this->lib->call('getLength', $pl);
		$np = $it - 1;
		$this->db->query("UPDATE buildqueue SET bq_order = $bql WHERE planet = $pl AND bq_order = $it");
		$this->db->query("UPDATE buildqueue SET bq_order = $it WHERE planet = $pl AND bq_order = $np");
		$this->db->query("UPDATE buildqueue SET bq_order = $np,workunits=0 WHERE planet = $pl AND bq_order = $bql");
	}
}

?>

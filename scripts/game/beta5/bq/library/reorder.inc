<?php

class beta5_bq_reorder {

	function beta5_bq_reorder($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Reorders the build queue
	function run($pid) {
		$q = $this->db->query("SELECT bq_order FROM buildqueue WHERE planet = $pid ORDER BY bq_order ASC");
		$i = 0;
		while ($r = dbFetchArray($q)) {
			if ($r[0] != $i) {
				$this->db->query("UPDATE buildqueue SET bq_order = $i WHERE planet = $pid AND bq_order = ".$r[0]);
			}
			$i ++;
		}
	}
}

?>

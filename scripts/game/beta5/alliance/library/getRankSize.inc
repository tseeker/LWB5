<?php

class beta5_alliance_getRankSize {

	function beta5_alliance_getRankSize($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the number of alliance members at a certain rank
	function run($aid, $rId) {
		$a = $this->lib->call('get', $aid);
		$qs = "SELECT COUNT(*) FROM player WHERE alliance=$aid AND a_status='IN' AND ";
		if ($rId == $a['default_grade']) {
			$qs .= "(a_grade=$rId OR a_grade IS NULL)";
		} else {
			$qs .= "a_grade=$rId";
		}
		if (!is_null($a['leader'])) {
			$qs .= " AND id<>" . $a['leader'];
		}
		$q = $this->db->query($qs);
		if (!($q && dbCount($q))) {
			return 0;
		}
		list($r) = dbFetchArray($q);
		return $r;
	}
}

?>

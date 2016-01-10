<?php

class beta5_alliance_getForumsComplete {

	function beta5_alliance_getForumsComplete($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Reads the list of an alliance's forums, with details
	function run($aid) {
		$q = $this->db->query("SELECT * FROM af_forum WHERE alliance=$aid ORDER BY forder");
		$a = array();
		while ($rs = dbFetchHash($q)) {
			if ($rs['last_post'] != "") {
				$q2 = $this->db->query("SELECT author,moment FROM af_post WHERE id=".$rs['last_post']);
				list($au,$mo) = dbFetchArray($q2);
				$au = $this->players->call('getName', $au);
				$rs['last'] = array(
					"moment"	=> $mo,
					"author"	=> $au
				);
			} else {
				$rs['last'] = null;
			}
			array_push($a, $rs);
		}
		return $a;
	}
}

?>

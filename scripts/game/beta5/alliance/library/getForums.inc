<?php

class beta5_alliance_getForums {

	function beta5_alliance_getForums($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Reads the list of an alliance's forums
	function run($aid) {
		$q = $this->db->query("SELECT id,forder,title,description,user_post FROM af_forum WHERE alliance=$aid ORDER BY forder");
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0]] = array(
				"order"		=> $r[1],
				"title"		=> $r[2],
				"description"	=> $r[3],
				"user_post"	=> ($r[4] == 't')
			);
		}
		return $rs;
	}
}

?>

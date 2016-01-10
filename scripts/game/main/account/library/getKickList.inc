<?php

class main_account_getKickList {

	function main_account_getKickList($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run() {
		$lists = array(array(),array(),array());
		$types = array("P" => 0, "Y" => 1, "N" => 2);

		$q = $this->db->query("SELECT * FROM adm_kick");
		while ($r = dbFetchHash($q)) {
			array_push($lists[$types[$r['status']]], $r);
		}

		return $lists;
	}
}

?>

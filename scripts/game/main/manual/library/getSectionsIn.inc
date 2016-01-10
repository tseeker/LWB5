<?php

class main_manual_getSectionsIn {

	function main_manual_getSectionsIn($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($rootId, $type = 1) {
		$qs = ($type > 0) ? (" AND is_page" . ($type == 2 ? " AND in_menu" : "")) : " AND NOT is_page";
		$q = $this->db->query("SELECT id,name,title,after_section FROM man_section WHERE in_section=$rootId$qs");
		$sl = array();
		$as = array();
		while ($r = dbFetchArray($q)) {
			$sl[$r[0]] = array(
				"name"	=> $r[1],
				"title"	=> $r[2]
			);
			$as[$r[3]] = $r[0];
		}

		$cid = $rootId;
		$nl = array();
		while (!is_null($as[$cid])) {
			$cid = $as[$cid];
			$nl[$cid] = $sl[$cid];
			$nl[$cid]['subs'] = $this->run($cid, $type);
		}

		return	$nl;
	}
}

?>

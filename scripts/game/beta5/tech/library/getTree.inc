<?php

class beta5_tech_getTree {
	var $data = null;

	function beta5_tech_getTree($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
	}


	function run() {
		if (is_array($this->data)) {
			return $this->data;
		}

		$res = array();
		$noReq = array();
		$q = $this->db->query("SELECT * FROM research");
		while ($r = dbFetchHash($q)) {
			$r['depends_on'] = $r['required_by'] = array();
			$q2 = $this->db->query("SELECT * FROM research_dep WHERE research=".$r['id']." OR depends_on=".$r['id']);
			while ($r2 = dbFetchArray($q2)) {
				$dir = ($r2[0] == $r['id']);
				array_push($r[$dir ? 'depends_on' : 'required_by'], $r2[$dir ? 1 : 0]);
			}

			if (($r['optional'] == 1 && !count($r['required_by'])) || $r['optional'] == 2) {
				array_push($noReq, $r['id']);
			}

			$res[$r['id']] = $r;
		}

		foreach	($noReq as $nrId) {
			$this->lib->mainClass->getAllDependencies($res, $nrId);
		}

		return ($this->data = array($res,$noReq));
	}
}

?>

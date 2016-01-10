<?php

class beta5_alliance_get {

	function beta5_alliance_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get alliance data
	function run($id) {
		$q = $this->db->query("SELECT * FROM alliance WHERE id = $id");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		$r = dbFetchHash($q);
		$q = $this->db->query(
			"SELECT COUNT(*),ROUND(AVG(s.x)),ROUND(AVG(s.y)) FROM player y,planet p,system s "
			. "WHERE y.alliance=$id AND y.a_status='IN' AND p.owner=y.id AND s.id=p.system"
		);
		list($r['nplanets'],$r['avgx'],$r['avgy']) = dbFetchArray($q);
		if (is_null($r['avgx'])) {
			$r['avgx'] = $r['avgy'] = 0;
		}
		return $r;
	}
}

?>

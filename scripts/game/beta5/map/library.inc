<?php

class beta5_map_library {
	var $index = array();


	function beta5_map_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns statistics about the whole universe
	function getUniverse() {
		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE status=0");
		list($np) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE status=0 AND owner IS NULL");
		list($nnp) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM system WHERE nebula>0");
		list($nn) = dbFetchArray($q);
		$q = $this->db->query("SELECT ROUND(AVG(mfact+ifact)),ROUND(AVG(turrets)) FROM planet WHERE status=0");
		list($af,$at) = dbFetchArray($q);
		return array($np,$nnp,$nn,$af,$at);
	}


	// Returns statistics about a protection zone
	function getProtectionZone($pz) {
		$q = $this->db->query("SELECT COUNT(*) FROM planet p,system s WHERE p.status=0 AND s.id=p.system AND s.prot=$pz");
		list($np) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM planet p,system s WHERE p.status=0 AND p.owner IS NULL AND s.id=p.system AND s.prot=$pz");
		list($nnp) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM system WHERE nebula>0 AND prot=$pz");
		list($nn) = dbFetchArray($q);
		$q = $this->db->query("SELECT ROUND(AVG(p.mfact+p.ifact)),ROUND(AVG(p.turrets)) FROM "
			. "planet p,system s WHERE p.status=0 AND s.id=p.system AND s.prot=$pz");
		list($af,$at) = dbFetchArray($q);

		// Protection time left
		$t = "FIXME"; // FIXME

		return array($np,$nnp,$nn,$af,$at,$t);
	}


	// Returns data about a system at specified coordinates
	function at($x, $y) {
		$q = $this->db->query("SELECT * FROM system WHERE x=$x AND y=$y");
		if (!($q && dbCount($q))) {
			return null;
		}
		return dbFetchHash($q);
	}


	// Returns map data for planets in a specified system
	function getSystem($sid) {
		$q = $this->db->query(
			"SELECT id,name,owner,status FROM planet WHERE system = $sid ORDER BY orbit"
		);
		$pl = array();
		$o = array();
		while ($r = dbFetchHash($q)) {
			array_push($pl, $r);
			if ($r['owner'] != "" && !in_array($r['owner'], $o)) {
				array_push($o,$r['owner']);
			}
		}

		if (!count($o)) {
			return $pl;
		}

		$q = $this->db->query(
			"SELECT p.id,a.tag "
			. "FROM player p,alliance a "
			. "WHERE p.a_status = 'IN' AND p.alliance=a.id"
			. " AND p.id IN (".join(',',$o).")"
		);
		$o = array();
		while ($r = dbFetchArray($q)) {
			$o[$r[0]] = $r[1];
		}

		for ($i=0;$i<count($pl);$i++) {
			if ($pl[$i]['owner'] == "") {
				continue;
			}
			$pl[$i]['tag'] = $o[$pl[$i]['owner']];
		}

		return $pl;
	}


	// Get planets within a distance of a set of coordinates
	function getPlanetsAround($x,$y,$d) {
		$d --;
		$q = $this->db->query(
			"SELECT p.id FROM planet p,system s "
			. "WHERE p.system=s.id AND SQRT((s.x-($x))*(s.x-($x))+(s.y-($y))*(s.y-($y)))<=$d"
		);
		$rl = array();
		while ($r = dbFetchArray($q)) {
			array_push($rl,$r[0]);
		}
		return $rl;
	}
}

?>

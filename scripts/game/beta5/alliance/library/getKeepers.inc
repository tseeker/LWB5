<?php

class beta5_alliance_getKeepers {

	function beta5_alliance_getKeepers($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get a list of IDs for the players that can accept new members into an alliance
	function run($a) {
		$q = $this->db->query("SELECT leader FROM alliance WHERE id=$a");
		list($id) = dbFetchArray($q);
		if (!is_null($id)) {
			$l = array($id);
		} else {
			$l = array();
		}

		$q = $this->db->query("SELECT id FROM alliance_grade WHERE alliance=$a AND can_accept");
		if (!$q || dbCount($q) == 0) {
			return $l;
		}
		$gl = array();
		while ($r = dbFetchArray($q)) {
			list($g) = $r;
			array_push($gl, $g);
		}

		$q = $this->db->query(
			"SELECT p.id FROM player p, alliance a WHERE a.id=$a AND p.alliance=a.id AND p.a_status='IN' "
			. "AND (p.a_grade IN (" . join(',',$gl) . ") OR (p.a_grade IS NULL AND a.default_grade IN (".join(',',$gl).")))"
		);
		while ($r = dbFetchArray($q)) {
			list($p) = $r;
			if (is_null($id) || $p != $id) {
				array_push($l, $p);
			}
		}

		return $l;
	}
}

?>

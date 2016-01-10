<?php

class beta5_planet_getStats {

	function beta5_planet_getStats($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns data regarding current planets
	function run($pid) {
		$q = $this->db->query(
			  "SELECT COUNT(*), SUM(pop), SUM(ifact) + SUM(mfact), SUM(turrets), SUM(happiness), SUM(corruption) "
			. " FROM planet WHERE owner = $pid GROUP BY owner"
		);
		$row = dbFetchArray($q);
		if (!$row) {
			return array(0, 'N/A', 0, 'N/A', 0, 'N/A', 0, 'N/A', 0, 'N/A');
		}
		$c = $row[0];
		$pa = floor($row[1] / $c);
		$fa = floor($row[2] / $c);
		$ta = floor($row[3] / $c);
		$ha = floor($row[4] / $c);
		$ca = round($row[5] / ($c*320));
		$q = $this->db->query(
			  "SELECT p.id FROM planet p, fleet f "
			  . "WHERE p.owner = $pid AND f.location = p.id AND f.attacking "
			  . "GROUP BY p.id"
		);
		$ua = dbCount($q);
		return array($c, $ha, $row[1], $pa, $row[2], $fa, $row[3], $ta, $ua, $ca);
	}
}

?>

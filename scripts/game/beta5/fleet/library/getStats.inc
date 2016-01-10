<?php

class beta5_fleet_getStats {

	function beta5_fleet_getStats($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns data regarding current fleets
	function run($pid) {
		// Get count and ship types
		$q = $this->db->query(
			  "SELECT COUNT(*), SUM(gaships), SUM(fighters), SUM(cruisers), SUM(bcruisers)"
			. " FROM fleet WHERE owner = $pid GROUP BY owner"
		);
		$cnt = dbFetchArray($q);
		if (!$cnt) {
			$cnt = array(0, 0, 0, 0, 0);
		}

		// Fleets at home
		$q = $this->db->query(
			"SELECT COUNT(*) FROM fleet f, planet p "
			. "WHERE f.owner = $pid AND f.location = p.id AND p.owner = $pid"
		);
		list($fah) = dbFetchArray($q);
		// Fleets at home, in battle
		$q = $this->db->query(
			"SELECT COUNT(*) FROM fleet f, planet p, fleet f2 "
			. "WHERE f.owner = $pid AND f.location = p.id AND p.owner = $pid "
			. "AND f2.location = p.id AND f2.attacking"
		);
		list($fahb) = dbFetchArray($q);

		// Fleets on foreign planets
		$q = $this->db->query(
			"SELECT COUNT(*) FROM fleet f,planet p "
			. "WHERE f.owner=$pid AND f.location=p.id"
			. " AND (p.owner IS NULL OR p.owner<>$pid)"
		);
		list($af) = dbFetchArray($q);
		// Fleets on foreign planets, in battle
		$q = $this->db->query(
			"SELECT COUNT(*) FROM fleet f,planet p,fleet f2 "
			. "WHERE f.owner=$pid AND f.location=p.id AND (p.owner IS NULL OR p.owner<>$pid) "
			. "AND f2.location=p.id AND (f2.attacking AND NOT f.attacking)"
		);
		list($afb1) = dbFetchArray($q);
		$q = $this->db->query(
			"SELECT COUNT(*) FROM fleet f, planet p "
			. "WHERE f.owner=$pid AND f.location=p.id AND (p.owner IS NULL OR p.owner<>$pid) "
			. "AND f.attacking"
		);
		list($afb2) = dbFetchArray($q);
		$afb = $afb1 + $afb2;

		// Moving fleets
		$q = $this->db->query("SELECT COUNT(*) FROM fleet WHERE owner = $pid AND moving IS NOT NULL");
		list($mf) = dbFetchArray($q);
		// Waiting fleets
		$q = $this->db->query("SELECT COUNT(*) FROM fleet WHERE owner = $pid AND waiting IS NOT NULL");
		list($wf) = dbFetchArray($q);

		return array(
			"fleets"		=> $cnt[0],
			"battle"		=> $fahb+$afb,
			"upkeep"		=> $this->lib->call('getUpkeep', $pid, $cnt[1], $cnt[2], $cnt[3], $cnt[4]),
			"power"			=> $this->lib->call('getPower', $pid, $cnt[1], $cnt[2], $cnt[3], $cnt[4]),
			"at_home"		=> $fah,
			"home_battle"		=> $fahb,
			"foreign"		=> $af,
			"foreign_battle"	=> $afb,
			"moving"		=> $mf,
			"waiting"		=> $wf,
			"gaships"		=> $cnt[1],
			"fighters"		=> $cnt[2],
			"cruisers"		=> $cnt[3],
			"bcruisers"		=> $cnt[4]
		);
	}
}

?>

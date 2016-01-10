<?php

class beta5_alliance_getMilitary {

	function beta5_alliance_getMilitary($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the list of planets under attack in an alliance
	function run($aid) {
		$q = $this->db->query(
			"SELECT p.id AS id,p.name AS name,l.id AS owner,a.v_players AS pllist,"
				. "a.v_friendly AS friendly,a.v_enemy AS enemy,s.x AS x,s.y AS y,p.orbit AS orbit "
			. " FROM planet p,attacks a,player l,system s "
			. "WHERE l.alliance=$aid AND l.a_status='IN' AND p.owner=l.id AND a.planet=p.id AND s.id=p.system"
		);
		$rs = array();
		while ($r = dbFetchHash($q)) {
			$r['f_list'] = $r['e_list'] = array();
			if ($r['pllist'] == 't') {
				$q2 = $this->db->query("SELECT DISTINCT owner,attacking FROM fleet WHERE location=".$r['id']);
				while ($r2=dbFetchArray($q2)) {
					array_push($r[($r2[1] == 't' ? "e" : "f").'_list'], $r2[0]);
				}
			}
			$rs[$r['id']] = $r;
		}
		return $rs;
	}
}

?>

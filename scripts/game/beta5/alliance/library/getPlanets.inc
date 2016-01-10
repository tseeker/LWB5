<?php

class beta5_alliance_getPlanets {

	function beta5_alliance_getPlanets($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Returns the list of planets belonging to players of the alliance
	function run($aid) {
		$rs = array();
		$q = $this->db->query("SELECT p.id,p.name,a.name FROM player p,account a WHERE a.id=p.userid AND p.alliance=$aid AND p.a_status='IN'");
		while ($r = dbFetchArray($q)) {
			$oid = $r[0]; $oname = ($r[1] == "") ? $r[2] : $r[1];
			$ppl = $this->players->call('getPlanets', $oid);

			foreach	($ppl as $pid => $pname) {
				$pinf = $this->planets->call('byId', $pid);
				$rs[$pid] = array(
					"name"		=> $pname,
					"ownerId"	=> $oid,
					"owner"		=> $oname,
					"x"		=> $pinf['x'],
					"y"		=> $pinf['y'],
					"orbit"		=> $pinf['orbit'] + 1,
					"fact"		=> $pinf['ifact'] + $pinf['mfact'],
					"turrets"	=> $pinf['turrets'],
				);
				$q2 = $this->db->query("SELECT COUNT(*) FROM fleet WHERE location=$pid AND attacking");
				list($ua) = dbFetchArray($q2);
				$rs[$pid]['attack'] = ($ua != 0);
			}
		}
		return $rs;
	}
}

?>

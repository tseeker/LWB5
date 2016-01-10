<?php

class beta5_player_makeEnemies {

	function beta5_player_makeEnemies($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
	}


	// Sets players' fleets to attack mode when they're added to an enemy list
	function run($pid, $elist) {
		$lmsg = array();
		$q = $this->db->query(
			"SELECT f.id,p.id,f.owner FROM fleet f,planet p "
			. "WHERE p.owner=$pid AND f.location=p.id AND NOT f.attacking AND f.owner IN (".join(',',$elist).")"
		);
		while ($r = dbFetchArray($q)) {
			$this->db->query("UPDATE fleet SET attacking=".dbBool(1).",can_move='B' WHERE id=".$r[0]);
			if (!is_array($lmsg[$r[1]])) {
				$lmsg[$r[1]] = array();
			}
			if (!is_array($lmsg[$r[1]][$r[2]])) {
				$lmsg[$r[1]][$r[2]] = array();
			}
			array_push($lmsg[$r[1]][$r[2]], $r[0]);
		}

		$pids = array_keys($lmsg);
		$tm = time();
		foreach	($pids as $pl) {
			$this->planets->call('updateMilStatus', $pl);
			$pinf = $this->planets->call('byId', $pl);
			$pn = addslashes($pinf['name']);

			foreach	($lmsg[$pl] AS $plid => $fl) {
				$q = $this->db->query("SELECT COUNT(*),SUM(gaships),SUM(fighters),SUM(cruisers),SUM(bcruisers) FROM fleet"
					. " WHERE id IN (".join(',',$fl).")");
				list($nf,$g,$f,$c,$b) = dbFetchArray($q);
				$fp = $this->fleets->call('getPower', $plid, $g,$f,$c,$b);

				$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($plid,$tm,'flswitch','INT',TRUE)");
				$q = $this->db->query("SELECT id FROM message WHERE player=$plid AND sent_on=$tm AND mtype='flswitch' ORDER BY id DESC LIMIT 1");
				list($mid) = dbFetchArray($q);
				$this->db->query("INSERT INTO msg_flswitch VALUES($mid,$pl,'$pn',$nf,$fp)");
			}
		}
	}
}

?>

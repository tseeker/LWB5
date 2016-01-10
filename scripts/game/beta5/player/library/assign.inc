<?php

class beta5_player_assign {

	function beta5_player_assign($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->planets	= $this->game->getLib('beta5/planet');
	}


	// Assign a planet to a player
	function run($pid, $planet) {
		$q = $this->db->query("SELECT id FROM system WHERE NOT assigned ORDER BY RANDOM() LIMIT 1");
		list($sid) = dbFetchArray($q);

		$npl = 6;
		$porb = rand(0, $npl - 1);

		$tm = time();
		$pQuery = "";
		if ($this->game->params['victory'] == 0) {
			$fTick = $this->game->ticks['day']->first;
			$this->db->query("DELETE FROM pk_enemy WHERE until < UNIX_TIMESTAMP(NOW())");
			$q = $this->db->query("SELECT * FROM pk_enemy WHERE player = $pid");
			if ($tm - $fTick >= $this->game->params['prot_after'] * 86400 && dbCount($q) == 0) {
				$pQuery = ",prot = " . $this->game->params['prot_duration'];
			}
		}

		$p = addslashes($planet);
		$this->db->query(
			"UPDATE planet SET name = '$p', owner = $pid, renamed = $tm,mod_check = FALSE "
				. "WHERE system = $sid AND orbit = $porb"
		);
		$this->db->query("UPDATE system SET assigned = TRUE$pQuery WHERE id=$sid");

		$q = $this->db->query("SELECT id,pop FROM planet WHERE system=$sid AND orbit=$porb FOR UPDATE");
		list($plid, $cPop) = dbFetchArray($q);

		$this->planets->call('updateHappiness', $plid);
		$this->planets->call('updateMaxPopulation', $plid, null, $pid);
		$this->db->query(
			"INSERT INTO planet_abandon_time (id, time_required) "
				. "VALUES ($plid, 6)"
		);

		return $plid;
	}
}

?>

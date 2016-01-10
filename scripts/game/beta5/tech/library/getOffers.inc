<?php

class beta5_tech_getOffers {

	function beta5_tech_getOffers($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Get a list of scientific assistance offers sent and received by a player
	function run($pid) {
		$pi = $this->players->call('get', $pid);
		$interval = $this->lib->game->ticks['day']->interval - 2 * $this->lib->game->ticks['hour']->interval;
		$q = $this->db->query("SELECT id FROM research_assistance WHERE offer_to=$pid AND accepted AND (UNIX_TIMESTAMP(NOW()) - moment) < $interval");
		$accToday = (dbCount($q) > 0);

		$q = $this->db->query("SELECT * FROM research_assistance WHERE player=$pid OR offer_to=$pid ORDER BY moment DESC");
		$ol = array();
		$t = time();
		while ($r = dbFetchHash($q)) {
			$o = array();
			$o['id'] = $r["id"];
			$o['type'] = ($pid == $r['player']) ? 'S' : 'R';
			$o['time'] = $r['moment'];
			$o['tech'] = $r['technology'];
			$o['price'] = $r['price'];
			$o['status'] = is_null($r['accepted']) ? ($t - $r['moment'] > $interval ? 'E' : 'P') : ($r['accepted'] == 't' ? 'A' : 'R');
			if ($o['status'] == 'P' && $o['type'] == 'R') {
				if ($o['price'] > $pi['cash']) {
					$o['pending'] = 1;
				} elseif ($accToday) {
					$o['pending'] = 2;
				} elseif ($o['tech'] != "" && $this->lib->call('has', $pid, $o['tech'], true)) {
					$o['pending'] = 3;
				} elseif ($o['tech'] != "" && !$this->lib->call('checkDependencies', $pid, $o['tech'])) {
					$o['pending'] = 4;
				} else {
					$o['pending'] = 0;
				}
			}
			$pinf = $this->players->call('getName', ($pid == $r['player']) ? $r['offer_to'] : $r['player']);
			$o['player'] = $pinf;
			array_push($ol, $o);
		}
		return $ol;
	}
}

?>

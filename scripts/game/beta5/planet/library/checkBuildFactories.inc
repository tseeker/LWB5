<?php

class beta5_planet_checkBuildFactories {

	function beta5_planet_checkBuildFactories($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->players	= $this->game->getLib('beta5/player');
		$this->rules	= $this->game->getLib('beta5/rules');
	}


	// Check a planet to see whether it's possible to build factories
	function run($id, $nb, $t) {
		// Get planet data
		$q = $this->db->query("SELECT ifact+mfact,sale,pop,owner FROM planet WHERE id=$id");
		list($nf, $sale, $pop, $owner) = dbFetchArray($q);
		logText("checkBuildFact($id, $nb, $t) : nf=$nf, sale=$sale owner=$owner");

		// Get owner rules
		$ru = $this->rules->call('get', $owner);
		$cost = $ru["{$t}f_cost"] * $nb;
		$pinf = $this->players->call('get', $owner);
		if ($pinf['cash'] < $cost) {
			return 1;
		}

		// Prevent building more factories if the planet's for sale
		if (!is_null($sale)) {
			return 2;
		}

		// Get the amount of factories built / destroyed in the past "24h" (actually, the interval between
		// day ticks)
		$interval = $this->game->ticks['day']->interval;
		$q = $this->db->query(
			"SELECT SUM(change) FROM facthist "
				. "WHERE planet=$id AND UNIX_TIMESTAMP(NOW()) - moment <= $interval"
		);
		if (dbCount($q)) {
			list($ch24) = dbFetchArray($q);
		} else {
			$ch24 = 0;
		}
		$onf = $nf - $ch24;

		// We can't build more than optFact*1.5 factories every 24h
		$x = ($pop - 2000) / 48000; $allowed = round(1.5 * (($pop / 30) - 754 * $x * $x));
		if ($nf + $nb > $onf + $allowed) {
			return 3;
		}

		// FIXME : siege

		return 0;
	}
}

?>

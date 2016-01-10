<?php

class beta5_player_reassign {

	function beta5_player_reassign($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
		$this->sales	= $this->lib->game->getLib('beta5/sale');
	}


	// Assigns the player a new planet after he loses all of his
	function run($player, $name) {
		// Delete fleets
		$list = array_keys($this->lib->call('getFleets', $player));
		foreach	($list as $fId) {
			$this->fleets->call('disband', $fId, true);
		}

		// Cancel all sales from this player
		$q = $this->db->query("SELECT id,player,finalized,sold_to FROM sale WHERE player=$player OR sold_to=$player");
		while ($r = dbFetchArray($q)) {
			list($sid,$seller,$fin,$buyer) = $r;
			if (is_null($fin)) {
				$ga = 'cancel';
			} else {
				$ga = 'cancelTransfer';

				if ($seller == $id) {
					$tInc = 0;
					$fInc = 1;
					$t = $buyer;
				} else {
					$tInc = 1;
					$fInc = 0;
					$t = $seller;
				}
			}
			$this->sales->call($ga, $seller, $sid);
		}

		// Delete probes
		// FIXME

		// Assign new planet
		$n = addslashes($name);
		$plid = $this->lib->call('assign', $player, $n);
		$this->db->query("UPDATE player SET first_planet=$plid WHERE id=$player");
		return $plid;
	}
}

?>

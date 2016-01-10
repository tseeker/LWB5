<?php

class beta5_fleet_sendMoveMessages {

	function beta5_fleet_sendMoveMessages($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->msg	= $this->lib->game->getLib('beta5/msg');
	}


	// Sends messages related to fleet movements
	function run() {
		$locs = array_unique(array_merge(array_keys($this->lib->mainClass->fleetDepartures), array_keys($this->lib->mainClass->fleetArrivals)));
		if (count($locs) == 0) {
			return;
		}

		// Get all fleet or planet owners for the affected locations and their status
		$q = $this->db->query(
			"SELECT owner AS id,FALSE AS att,id AS loc FROM planet WHERE owner IS NOT NULL AND id IN (".join(',',$locs).") "
			. "UNION SELECT owner AS id,attacking AS att,location AS loc FROM fleet WHERE location IN (".join(',',$locs).")"
		);

		$fMove = array();
		$ownArrivals = array();
		while ($r = dbFetchArray($q)) {
			list($player,$status,$location) = $r;
			$oa = $ha = $hd = $fa = $fd = array();

			// Generate the list of fleet departures
			if (is_array($this->lib->mainClass->fleetDepartures[$location])) {
				foreach	($this->lib->mainClass->fleetDepartures[$location][(int)$status] as $fId) {
					$f = $this->lib->call('get', $fId);
					if ($f['owner'] == $player) {
						continue;
					}
					array_push($fd, $f);
				}
				foreach	($this->lib->mainClass->fleetDepartures[$location][1-$status] as $fId) {
					$f = $this->lib->call('get', $fId);
					if ($f['owner'] == $player) {
						continue;
					}
					array_push($hd, $f);
				}
			}

			// Generate the list of fleet arrivals
			if (is_array($this->lib->mainClass->fleetArrivals[$location])) {
				foreach	($this->lib->mainClass->fleetArrivals[$location][(int)$status] as $fdt) {
					list($fId, $from) = $fdt;
					$f = $this->lib->call('get', $fId);
					$f['from'] = $from;
					if ($f['owner'] == $player) {
						if (!is_array($ownArrivals[$player])) {
							$ownArrivals[$player] = array();
						}
						if (!is_array($ownArrivals[$player][$location])) {
							$ownArrivals[$player][$location] = array();
						}
						array_push($ownArrivals[$player][$location], $f);
					} else {
						array_push($fa, $f);
					}
				}
				foreach	($this->lib->mainClass->fleetArrivals[$location][1-$status] as $fdt) {
					list($fId, $from) = $fdt;
					$f = $this->lib->call('get', $fId);
					$f['from'] = $from;
					if ($f['owner'] == $player) {
						l::warn("beta5/sendFleetMoveMessages(): fleet $fId owned by player $player hostile to its owner");
						continue;
					}
					array_push($ha, $f);
				}
			}

			// Add the data to the list of fleet movements
			if (!(count($fa)||count($fd)||count($ha)||count($hd))) {
				continue;
			}
			if (!is_array($fMove[$player])) {
				$fMove[$player] = array($location => array($fa, $fd, $ha, $hd));
			} else {
				$fMove[$player][$location] = array($fa, $fd, $ha, $hd);
			}
		}

		$pnames = array();
		$fpowers = array();

		// Send messages for own fleets arrivals
		$tm = time() - 1;
		foreach	($ownArrivals as $player => $locs) {
			foreach	($locs as $loc => $flist) {
				// Get planet name
				if (is_null($pnames[$loc])) {
					$q = $this->db->query("SELECT name FROM planet WHERE id=$loc");
					list($pnames[$loc]) = dbFetchArray($q);
				}
				$pname = $pnames[$loc];

				// Generate a message
				$mid = $this->msg->call('send', $player, 'flmove', array(
					'p_id'		=> $loc,
					'p_name'	=> $pname
				));

				// Insert fleet data
				foreach	($flist as $fleet) {
					// Get origin planet name
					if (is_null($pnames[$fleet['from']])) {
						$q = $this->db->query("SELECT name FROM planet WHERE id={$fleet['from']}");
						list($pnames[$fleet['from']]) = dbFetchArray($q);
					}
					$pname = $pnames[$fleet['from']];

					$fpowers[$fleet['id']] = $fPower = $this->lib->call('getPower',
						$player, $fleet['gaships'], $fleet['fighters'], $fleet['cruisers'], $fleet['bcruisers']);
					$this->db->query("INSERT INTO flmove_data VALUES ($mid,'".addslashes($fleet['name'])."',$player,"
						. "{$fleet['gaships']},{$fleet['fighters']},{$fleet['cruisers']},{$fleet['bcruisers']},$fPower,"
						. "FALSE,TRUE,{$fleet['from']},'".addslashes($pname)."')");
				}
			}
		}

		// Send messages for other fleets
		$tm++;
		foreach	($fMove as $player => $locs) {
			foreach	($locs as $loc => $flists) {
				$flist = array();
				foreach	($flists[0] as $f) {
					$f['hostile'] = 0;
					array_push($flist, $f);
				}
				foreach	($flists[1] as $f) {
					$f['hostile'] = 0;
					array_push($flist, $f);
				}
				foreach	($flists[2] as $f) {
					$f['hostile'] = 1;
					array_push($flist, $f);
				}
				foreach	($flists[3] as $f) {
					$f['hostile'] = 1;
					array_push($flist, $f);
				}

				// Get planet name
				if (is_null($pnames[$loc])) {
					$q = $this->db->query("SELECT name FROM planet WHERE id=$loc");
					list($pnames[$loc]) = dbFetchArray($q);
				}
				$pname = $pnames[$loc];

				// Generate a message
				$mid = $this->msg->call('send', $player, 'flmove', array(
					'p_id'		=> $loc,
					'p_name'	=> $pname
				));

				// Insert fleet data
				foreach	($flist as $fleet) {
					if (is_null($fleet['from'])) {
						$arrived = 0;
					} else {
						// Get origin planet name
						if (is_null($pnames[$fleet['from']])) {
							$q = $this->db->query("SELECT name FROM planet WHERE id={$fleet['from']}");
							list($pnames[$fleet['from']]) = dbFetchArray($q);
						}
						$pname = $pnames[$fleet['from']];
						$arrived = 1;
					}

					if (is_null($fpowers[$fleet['id']])) {
						$fpowers[$fleet['id']] = $this->lib->call('getPower',
							$fleet['owner'], $fleet['gaships'], $fleet['fighters'], $fleet['cruisers'], $fleet['bcruisers']);
					}
					$fPower = $fpowers[$fleet['id']];
					l::trace("beta5/sendFleetMoveMessages: inserting message for player $player, location $loc, fleet {$fleet['id']}");
					$this->db->query("INSERT INTO flmove_data VALUES ($mid,'".addslashes($fleet['name'])."',{$fleet['owner']},"
						. "{$fleet['gaships']},{$fleet['fighters']},{$fleet['cruisers']},{$fleet['bcruisers']},$fPower,"
						. ($fleet['hostile'] ? "TRUE" : "FALSE") . "," . ($arrived ? "TRUE" : "FALSE") . ","
						. ($arrived ? ("{$fleet['from']},'".addslashes($pname)."'") : "NULL,NULL") . ")");
				}
			}
		}

		$this->lib->mainClass->fleetArrivals = $this->lib->mainClass->fleetDepartures = array();
	}
}

?>

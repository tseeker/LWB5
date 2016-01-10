<?php

class beta5_fleet_merge {

	function beta5_fleet_merge($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->moving	= $this->lib->game->getLib('beta5/moving');
	}


	// Merge fleets
	function run($fIds, $okOwners, $newName) {
		// Did we get fleet IDs?
		if (!count($fIds)) {
			return array();
		}

		// Get fleets to merge
		$q = $this->db->query(
			"SELECT * FROM fleet "
			."WHERE id IN (".join(',',$fIds).") AND owner IN (".join(',',$okOwners)
			.") AND can_move='Y' AND sale IS NULL "
			."ORDER BY location,owner"
		);
		$q2 = $this->db->query("SELECT id FROM sale WHERE fleet IN (".join(',',$fIds).")");
		if (!$q || !$q2 || dbCount($q) != count($fIds) || dbCount($q2)) {
			return array();
		}

		// Generate an array from the fleets read and extract
		// movement / stand-by information
		$fleets = array(); $mIds = array(); $wIds = array();
		while ($r = dbFetchHash($q)) {
			$fleets[$r['id']] = $r;
			if (!is_null($r['moving'])) {
				array_push($mIds, $r['moving']);
			} elseif (!is_null($r['waiting'])) {
				array_push($wIds, $r['waiting']);
			}
		}

		// Extract movement information
		$move = array();
		if (count($mIds)) {
			$q = $this->db->query("SELECT id,m_to,time_left,wait_order,changed FROM moving_object "
				."WHERE id IN (".join(',',$mIds).")");

			while ($r = dbFetchHash($q)) {
				$move[$r['id']] = $r;
				if (!is_null($r['wait_order']))
					array_push($wIds, $r['wait_order']);
			}
		}

		// Extract stand-by information
		$wait = array();
		if (count($wIds)) {
			$q = $this->db->query("SELECT id,drop_point,time_left FROM hs_wait "
				."WHERE id IN (".join(',',$wIds).")");
			while ($r = dbFetchHash($q)) {
				$wait[$r['id']] = $r;
			}
		}

		// Group fleets by location / status / owner
		$gFleets = array();
		foreach	($fIds as $i) {
			// Generate identifier and time to wait
			$lid = $fleets[$i]['owner'].":";
			$am = ($fleets[$i]['attacking'] == 't');
			if (!is_null($fleets[$i]['waiting'])) {
				$lid .= "W:".$wait[$fleets[$i]['waiting']]['drop_point'];
				$tl = $wait[$fleets[$i]['waiting']]['time_left'];
			} elseif (!is_null($moid = $fleets[$i]['moving'])) {
				$mwo = $move[$moid]['wait_order'];
				$lid .= "M:".$move[$moid]['m_to'].':'.$move[$moid]['time_left'].':'
					.$move[$moid]['changed'].":".$this->moving->call('getLocation', $moid);
				$tl = is_null($mwo) ? 0 : $wait[$mwo]['time_left'];
			} else {
				$tl = 0;
				$lid .= "L:".$fleets[$i]['location'];
			}

			// Generate / update container
			if (!is_array($gFleets[$lid])) {
				$gFleets[$lid] = array('t' => $tl, 'a' => $am, 'l' => array());
			} else {
				if ($tl > $gFleets[$lid]['t']) {
					$gFleets[$lid]['t'] = $tl;
				}
				$gFleets[$lid]['a'] |= $am;
			}

			// Add fleet
			array_push($gFleets[$lid]['l'], $i);
		}

		// Merge groups into single fleets
		$nfl = array();
		foreach	($gFleets as $fg) {
			// Compute total amount of ships
			$sums = array(0,0,0,0);
			$minSpent = null;
			foreach	($fg['l'] as $i) {
				$sums[0] += $fleets[$i]['gaships'];
				$sums[1] += $fleets[$i]['fighters'];
				$sums[2] += $fleets[$i]['cruisers'];
				$sums[3] += $fleets[$i]['bcruisers'];

				if (is_null($minSpent) || $minSpent > $fleets[$i]['time_spent']) {
					$minSpent = $fleets[$i]['time_spent'];
				}
			}

			// Update merged fleet
			$nId = array_shift($fg['l']);
			$name = addslashes(($newName == "") ? $fleets[$nId]['name'] : $newName);
			$this->db->query("UPDATE fleet SET name='$name',gaships=".$sums[0].",fighters=".$sums[1]
				. ",cruisers=".$sums[2].",bcruisers=".$sums[3].",attacking="
				. dbBool($fg['a']) . ",time_spent=$minSpent WHERE id=$nId");

			// Delete unneeded data
			if (count($fg['l'])) {
				$dMids = array(); $dWids = array();
				foreach	($fg['l'] as $i) {
					if (!is_null($fleets[$i]['waiting'])) {
						array_push($dWids, $fleets[$i]['waiting']);
					} elseif (!is_null($fleets[$i]['moving'])) {
						array_push($dMids, $fleets[$i]['moving']);
					}
				}
				foreach	($dMids as $i) {
					if (!is_null($move[$i]['wait_order'])) {
						array_push($dWids, $move[$i]['wait_order']);
					}
				}
				$this->db->query("DELETE FROM fleet WHERE id IN (".join(',',$fg['l']).")");
				if (count($dMids)) {
					$this->db->query("DELETE FROM moving_object WHERE id IN (".join(',',$dMids).")");
				}
				if (count($dWids)) {
					$this->db->query("DELETE FROM hs_wait WHERE id IN (".join(',',$dWids).")");
				}
			}

			// Update detection status
			$this->db->query("DELETE FROM beacon_detection WHERE fleet = $nId");

			// Make sure orders are up-to-date
			$mwo = $fleets[$nId]['moving'];
			$dest = is_null($mwo) ? null : $move[$mwo]['m_to'];
			$wait = $fg['t'] ? $fg['t'] : null;
			$this->lib->call('setOrders', $nId, $dest, $wait);

			array_push($nfl, $nId);
		}

		return $nfl;
	}
}

?>

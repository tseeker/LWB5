<?php

class beta5_fleet_setOrders {

	function beta5_fleet_setOrders($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->moving	= $this->lib->game->getLib('beta5/moving');
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->rules	= $this->lib->game->getLib('beta5/rules');
		$this->standby	= $this->lib->game->getLib('beta5/standby');
	}


	// Changes a fleet's orders
	function run($fid, $newDest, $newDelay, $attack = null) {
		// Get fleet data
		$fleet = $this->lib->call('get', $fid);
		if (is_null($fleet) || !is_null($fleet['sale_info']) || $fleet['can_move'] != 'Y') {
			return false;
		}
		if (is_null($attack)) {
			$attack = ($fleet['attacking'] == 't');
		}

		// Check for hyperspace capabilities
		$r  = $this->rules->call('get', $fleet['owner']);
		$used = $r['gaship_space'] * $fleet['gaships'] + $r['fighter_space'] * $fleet['fighters'];
		$avail = $r['cruiser_haul'] * $fleet['cruisers'] + $r['bcruiser_haul'] * $fleet['bcruisers'];
		$hsOk = ($used <= $avail);

		// Get cruisers status / Capital ships speed
		$cru = ($fleet['cruisers'] > 0);
		$csp = $r['capital_ship_speed'];

		// Check for an implicit "null" move order
		$cLoc = is_null($fleet['move'])
			? (is_null($fleet['wait'])
				? $fleet['location']
				: $fleet['wait']['drop_point'])
			: $this->moving->call('getLocation', $fleet['move']['id']);
		if ($cLoc == $newDest)
			$newDest = null;

		// Some checks must be performed if the fleet is going out of system
		if (!(is_null($cLoc) || is_null($newDest))) {
			$cLocInfo = $this->planets->call('byId', $cLoc);
			$nDestInfo = $this->planets->call('byId', $newDest);
			if ($cLocInfo['x'] != $nDestInfo['x'] || $cLocInfo['y'] != $nDestInfo['y']) {
				// Fleet is not hyperspace-capable
				if (!$hsOk) {
					return false;
				}
				// Check protection
				if ($this->lib->game->params['victory'] == 0) {
					$protected = $this->players->call('getProtectionLevel', $fleet['owner']);
					if ($protected) {
						$this->players->call('breakProtection', $fleet['owner'], 'ACT');
					}
				}
			}
		}

		// Identify movement-related actions
		$newMove = is_null($fleet['move']) && !is_null($newDest);
		$rmMove  = !is_null($fleet['move']) && is_null($newDest);
		$chgMove = !is_null($fleet['move']) && !is_null($newDest) && ($newDest != $fleet['move']['m_to']);
		$mNoAct  = !($newMove||$rmMove||$chgMove);

		// Identify HS standby-related actions
		$newWait = $hsOk && is_null($fleet['wait']) && !is_null($newDelay);
		$rmWait  = $hsOk && !is_null($fleet['wait']) && is_null($newDelay);
		$chgWait = $hsOk && !is_null($fleet['wait']) && !is_null($newDelay) && ($newDelay != $fleet['wait']['time_left']);
		$wNoAct  = !($newWait||$rmWait||$chgWait);


		logText("newMove $newMove; rmMove $rmMove; chgMove $chgMove; mNoAct $mNoAct");
		logText("newWait $newWait; rmWait $rmWait; chgWait $chgWait; wNoAct $wNoAct");

		// No actions are to be taken, return
		if ($mNoAct && $wNoAct) {
			return true;
		}

		// Start moving
		$fleetArrived = false;
		if ($newMove) {
			$departure = false;

			// HS orders haven't changed.
			if ($wNoAct) {
				// Are we already waiting?
				if (is_null($fleet['wait'])) {
					$wo = null;
					$sl = $fleet['location'];
					$departure = true;
				} else {
					$wo = $fleet['wait']['id'];
					$sl = $fleet['wait']['drop_point'];
					logText("Adding new move orders, initial location: $sl");
					$this->db->query("UPDATE hs_wait SET time_left=$newDelay,time_spent=0,origin=NULL WHERE id=$wo");
				}
			} elseif ($newWait) {
				// New HS stand-by order
				$wo = $this->standby->call('create', $newDelay, $newDest);
				$sl = $fleet['location'];
				$departure = true;
			} elseif ($rmWait) {
				// Delete current HS stand-by order
				$wo = null;
				$sl = $fleet['wait']['drop_point'];
				$this->db->query("DELETE FROM hs_wait WHERE id=".$fleet['wait']['id']);
			} elseif ($chgWait) {
			// Change HS stand-by order
				$wo = $fleet['wait']['id'];
				$sl = $fleet['wait']['drop_point'];
				$this->db->query("UPDATE hs_wait SET time_left=$newDelay,time_spent=0,origin=NULL,drop_point=$newDest WHERE id=".$fleet['wait']['id']);
			}

			// Create movement entry
			$this->db->query("DELETE FROM beacon_detection WHERE fleet = $fid");
			$mo = $this->moving->call('newObject', $sl, $newDest, $csp, $cru, $wo);
			if (is_null($mo)) {
				logText("beta5/setFleetOrders($fid,$newDest,$newDelay,".($attack?1:0)."): unable to create a new moving_object entry", LOG_ERR);
				return false;
			}
			$this->db->query("UPDATE fleet SET location=NULL,moving=$mo,waiting=NULL WHERE id=$fid");
			if ($departure) {
				$this->planets->call('updateMilStatus', $sl);
				$this->planets->call('updateHappiness', $sl);
				$this->addDeparture($sl, $fid, $fleet['attacking'] == 't');
			}
		} elseif ($rmMove) {
			// Fleet stop requested
			$mo = $fleet['move']['id'];
			$nloc = $this->moving->call('getLocation', $mo);

			if ($wNoAct) {
				// HS orders haven't changed.
				// Do we have stand-by orders?
				if (is_null($fleet['wait'])) {
					$wo = null;
				} else {
					$wo = $fleet['wait']['id'];
					$this->db->query("UPDATE hs_wait SET drop_point=$nloc WHERE id=$wo");
				}
			}
			elseif ($newWait) {
				// New HS stand-by order
				$wo = $this->standby->call('create', $newDelay, $nloc);
			} elseif ($rmWait) {
				// Delete current HS stand-by order
				$wo = null;
				$this->db->query("DELETE FROM hs_wait WHERE id=".$fleet['wait']['id']);
			} elseif ($chgWait) {
				// Change HS stand-by order
				$wo = $fleet['wait']['id'];
				$this->db->query("UPDATE hs_wait SET time_left=$newDelay,time_spent=0,drop_point=$nloc WHERE id=".$fleet['wait']['id']);
			}

			// Stop movement
			$this->moving->call('stop', $mo, $wo);
		} elseif ($chgMove) {
			// Fleet destination changed
			$mo = $fleet['move']['id'];

			if ($wNoAct) {
				// HS orders haven't changed.
				// Do we have stand-by orders?
				if (is_null($fleet['wait'])) {
					$wo = null;
				} else {
					$wo = $fleet['wait']['id'];
					$this->db->query("UPDATE hs_wait SET drop_point=$newDest WHERE id=$wo");
				}
			}
			elseif ($newWait) {
				// New HS stand-by order
				$wo = $this->standby->call('create', $newDelay, $newDest);
			} elseif ($rmWait) {
				// Delete current HS stand-by order
				$wo = null;
				$this->db->query("DELETE FROM hs_wait WHERE id=".$fleet['wait']['id']);
			} elseif ($chgWait) {
				// Change HS stand-by order
				$wo = $fleet['wait']['id'];
				$this->db->query("UPDATE hs_wait SET time_left=$newDelay,time_spent=0,drop_point=$newDest WHERE id=$wo");
			}

			// Redirect fleet
			$this->moving->call('redirect', $mo, $newDest, $csp, $cru, $wo);
		} elseif ($newWait) {
			// No destination change, but stand-by orders changed
			// New HS stand-by order
			if (is_null($fleet['move'])) {
				$loc = $fleet['location'];
				$wo = $this->standby->call('create', $newDelay, $loc, $loc, null);

				$this->db->query("UPDATE fleet SET waiting=$wo,location=NULL WHERE id=$fid");
				$this->db->query("UPDATE hs_wait SET origin=$loc WHERE id=$wo");

				$this->planets->call('updateMilStatus', $loc);
				$this->planets->call('updateHappiness', $loc);
				$this->addDeparture($loc, $fid, $fleet['attacking'] == 't');
				$this->planets->call('detectFleets', $loc);
			} else {
				$wo = $this->standby->call('create', $newDelay, $fleet['move']['m_to']);
				$this->db->query("UPDATE moving_object SET wait_order=$wo WHERE id=".$fleet['move']['id']);
			}
		} elseif ($rmWait) {
			// Delete current HS stand-by order
			$this->db->query("DELETE FROM hs_wait WHERE id=".$fleet['wait']['id']);
			if (is_null($fleet['move'])) {
				$fleetArrived = true;
				$this->lib->call('arrival', $fid, $fleet['wait']['drop_point'], $fleet['wait']['origin'], $attack);
				$this->planets->call('updateMilStatus', $fleet['wait']['drop_point']);
				$this->planets->call('updateHappiness', $fleet['wait']['drop_point']);
				$this->db->query("DELETE FROM beacon_detection WHERE fleet = $fid");
			}
		} elseif ($chgWait) {
			// Change HS stand-by order
			$this->db->query("UPDATE hs_wait SET time_left=$newDelay WHERE id=".$fleet['wait']['id']);
		}

		// If the fleet hasn't arrived, set its status
		if (!$fleetArrived) {
			$this->db->query("UPDATE fleet SET attacking=".dbBool($attack)." WHERE id=$fid");
		}

		$this->lib->call('invCache', $fid);
		return true;
	}


	// Adds an entry to the list of fleet departures
	function addDeparture($location, $fid, $status) {
		if (!is_array($this->lib->mainClass->fleetDepartures[$location])) {
			$this->lib->mainClass->fleetDepartures[$location] = array(array(), array());
		}
		array_push($this->lib->mainClass->fleetDepartures[$location][$status?1:0], $fid);
	}
}

?>

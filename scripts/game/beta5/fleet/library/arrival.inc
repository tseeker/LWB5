<?php

class beta5_fleet_arrival {

	function beta5_fleet_arrival($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->move	= $this->lib->game->getLib('beta5/moving');
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Handles a fleet's arrival
	function run($fid, $dest, $from, $nStatus = null) {
		// Get complete fleet data
		$q = $this->db->query("SELECT * FROM fleet WHERE id=$fid");
		$f = dbFetchHash($q);
		if (is_null($f['owner'])) {
			logText("beta5/fleetArrival($fid,$dest,$from): BUG! Fleet has no owner!", LOG_ERR);
			return;
		}
		if (is_null($nStatus)) {
			$nStatus = ($f['attacking'] == 't');
		}

		// Get destination planet owner
		$po = $this->planets->call('getOwner', $dest);
		if (!is_null($po) && $po != $f['owner']) {
			// Is the fleet owner an enemy of the planet owner?
			$isEnemy = $this->players->call('isEnemy', $po, $f['owner']);
			if (!$isEnemy) {
				// Get fleet owner data
				$foi = $this->players->call('get', $f['owner']);
				// Check for enemy alliance
				$isEnemy = (!is_null($foi['aid']) && $this->players->call('isAllianceEnemy', $po, $foi['aid']));
			}
		} else {
			$isEnemy = false;
		}

		// Check whether the player already has fleets at that location,
		// and if he does, get their current status
		if (!$isEnemy) {
			$q = $this->db->query("SELECT attacking FROM fleet WHERE location=$dest AND owner=".$f['owner']." LIMIT 1");
			if ($q && dbCount($q)) {
				list($aa) = dbFetchArray($q);
				$isEnemy = ($aa == 't');
			}
		}

		// Set attack status
		$att = ($po != $f['owner']) && ($isEnemy || $nStatus);
		logText("beta5/fleetArrival($fid,$dest,$from,{$f['owner']}): Attack=".($att?1:0), LOG_DEBUG);
		if (is_array($_SESSION[game::sessName()])) {
			logText("Fleet $fid was being controlled by player #{$_SESSION[game::sessName()]['player']}");
		}

		if ($att) {
			if (($split = $this->hsWindowCollapsing($po, $f, $dest, $from)) === true) {
				return;
			}

			// Switch the player's fleets to attack at that location if the fleet arriving is attacking
			$this->db->query("UPDATE fleet SET attacking=TRUE,can_move='B' WHERE location=$dest AND NOT attacking AND owner=".$f['owner']);
		} else {
			$split = "";
		}

		// Update the fleet's record
		$this->db->query("UPDATE fleet SET location=$dest,time_spent=0,attacking=".dbBool($att).",can_move='".($att?'B':'H')."'$split WHERE id=$fid");

		// Make sure the system the fleet has arrived in can't be assigned to a new player
		$pinf = $this->planets->call('byId', $dest);
		$this->db->query("UPDATE system SET assigned=TRUE WHERE id=".$pinf['system']);

		// Add a fleet arrival entry to the list
		if (!is_array($this->lib->mainClass->fleetArrivals[$dest])) {
			$this->lib->mainClass->fleetArrivals[$dest] = array(array(), array());
		}
		array_push($this->lib->mainClass->fleetArrivals[$dest][$att?1:0], array($fid, $from));

		// Clear the fleet cache
		$this->lib->call('invCache', $fid);
	}


	function hsWindowCollapsing($po, $f, $dst, $ori) {
		// Apply HS window collapsing
		$r = $this->rules->call('get', $po);
		$rnd = rand(0,$r['prevent_hs_exit']*10);
		$splitG = floor($rnd * $f['gaships'] / 100);
		$splitF = floor($rnd * $f['fighters'] / 100);
		$splitC = ceil($rnd * $f['cruisers'] / 100);
		$splitB = ceil($rnd * $f['bcruisers'] / 100);
		if (!($rnd && ($splitC || $splitB))) {
			return "";
		}

		// WE HAVE A WINNER!

		if (is_null($f['moving'])) {
			$or = $this->rules->call('get', $f['owner']);
		}

		if ($f['gaships'] == $splitG && $f['fighters'] == $splitF && $f['cruisers'] == $splitC && $f['bcruisers'] == $splitB) {
			// The complete fleet has to be delayed
			logText("Fleet #{$f['id']} was prevented from dropping out of HS", LOG_DEBUG);
			if (is_null($f['moving'])) {
				// The fleet dropped out of Hyperspace, create a move order
				$fmo = $this->move->call('newObject', $ori, $dst, $or['capital_ship_speed'], ($f['cruisers'] > 0), null);
				$this->db->query("UPDATE moving_object SET changed=60,time_left=1 WHERE id=$fmo");
				$this->db->query("UPDATE fleet SET moving=$fmo,waiting=NULL WHERE id={$f['id']}");
				logText("Fleet #{$f['id']} -> created new moving object", LOG_DEBUG);
			} else {
				// The fleet was moving, just modify the order
				$this->db->query("UPDATE moving_object SET changed=60,time_left=1 WHERE id={$f['moving']}");
				logText("Fleet #{$f['id']} -> modified existing moving object", LOG_DEBUG);
			}
			$fullFleet = true;
		} else {
			logText("Fleet {$f['id']} got split by HS windows collapsing ($splitG/$splitF/$splitC/$splitB out of {$f['gaships']}/{$f['fighters']}/{$f['cruisers']}/{$f['bcruisers']})", LOG_DEBUG);

			// Split fleet
			$fullFleet  = ",gaships=" . ($f['gaships'] - $splitG);
			$fullFleet .= ",fighters=" . ($f['fighters'] - $splitF);
			$fullFleet .= ",cruisers=" . ($f['cruisers'] - $splitC);
			$fullFleet .= ",bcruisers=" . ($f['bcruisers'] - $splitB);

			if (is_null($f['moving'])) {
				// The fleet dropped out of Hyperspace, create a move order
				$fmo = $this->move->call('newObject', $ori, $dst, $or['capital_ship_speed'], ($f['cruisers'] > 0), null);
				logText("Fleet #{$f['id']} -> created new moving object", LOG_DEBUG);
			} else {
				// The fleet was moving, duplicate the order
				$fmo = $this->move->call('cloneObject', $f['moving']);
				logText("Fleet #{$f['id']} -> cloned existing moving object", LOG_DEBUG);
			}
			$this->db->query("UPDATE moving_object SET changed=60,time_left=1 WHERE id=$fmo");

			// Generate new fleet
			$this->db->query("INSERT INTO fleet(owner,gaships,fighters,cruisers,bcruisers,attacking,moving) VALUES ("
				. $f['owner'] . ",$splitG,$splitF,$splitC,$splitB,TRUE,$fmo)");
		}

		return $fullFleet;
	}
}

?>

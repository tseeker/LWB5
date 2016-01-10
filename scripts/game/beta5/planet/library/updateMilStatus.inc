<?php

class beta5_planet_updateMilStatus {

	function beta5_planet_updateMilStatus($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->ecm	= $this->lib->game->getLib('beta5/ecm');
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
	}

	function run($planet) {
		// Get previous status
		$q = $this->db->query("SELECT * FROM attacks WHERE planet=$planet");
		if ($q && dbCount($q)) {
			$attack = dbFetchHash($q);
		} else {
			$attack = null;
		}

		// Get current planet owner
		$ownId = $this->lib->call('getOwner', $planet);

		// If it's neutral, we're done
		if (is_null($ownId)) {
			if (is_array($attack)) {
				// No attack status for neutrals
				$this->db->query("DELETE FROM attacks WHERE planet=$planet");
			}
			return;
		}

		// Select all fleets on the planet
		$q = $this->db->query("SELECT owner,attacking,gaships,fighters,cruisers,bcruisers FROM fleet WHERE location=$planet");
		$fList = array(); $attPlayers = array(); $defPlayers = array();
		while ($r = dbFetchHash($q)) {
			array_push($fList, $r);
			if ($r['attacking'] == 't' && !in_array($r['owner'], $attPlayers)) {
				array_push($attPlayers, $r['owner']);
			} elseif ($r['attacking'] == 'f' && !in_array($r['owner'], $defPlayers)) {
				array_push($defPlayers, $r['owner']);
			}
		}

		// Not under attack
		if (!count($attPlayers)) {
			// ... but were we before?
			if (is_array($attack)) {
				$this->db->query("DELETE FROM attacks WHERE planet=$planet");
			}
			return;
		}

		// Add planet owner to defending players if he's not there already
		if (!in_array($ownId, $defPlayers)) {
			array_push($defPlayers, $ownId);
		}

		// Get max. ECM and ECCM levels
		$q = $this->db->query("SELECT MAX(value) FROM rule WHERE name='ecm_level' AND player IN (".join(',',$attPlayers).")");
		list($ecm) = dbFetchArray($q);
		$q = $this->db->query("SELECT MAX(value) FROM rule WHERE name='eccm_level' AND player IN (".join(',',$defPlayers).")");
		list($eccm) = dbFetchArray($q);

		// Compute fleet powers
		$attPower = $defPower = 0;
		foreach	($fList as $fl) {
			$pow = $this->fleets->call('getPower', $fl['owner'], $fl['gaships'], $fl['fighters'], $fl['cruisers'], $fl['bcruisers']);
			if ($fl['attacking'] == 'f') {
				$defPower += $pow;
			} else {
				$attPower += $pow;
			}
		}
		$defPower += $this->lib->call('getPower', $planet);

		// If situation hasn't changed, we're done
		if (is_array($attack) && $attack['ecm_level'] == $ecm && $attack['eccm_level'] == $eccm && $attack['friendly'] == $defPower && $attack['enemy'] == $attPower) {
			return;
		}

		// Generate new information level
		$ifl = $this->ecm->call('getInformationLevel', $ecm, $eccm);

		// If we weren't under attack before, we must add an entry
		if (is_null($attack)) {
			$qs  = "INSERT INTO attacks VALUES($planet,$ecm,$eccm,$defPower,$attPower,";
			$qs .= dbBool($ifl == 4);
			$vDP = $this->ecm->call('scrambleFleetPower', $ifl, $defPower);
			$vAP = $this->ecm->call('scrambleFleetPower', $ifl, $attPower);
			$qs .= ",$vDP,$vAP";
			$this->db->query("$qs)");
			return;
		}

		// Update attack status
		$qs = "UPDATE attacks SET v_players=" . dbBool($ifl == 4) . ",friendly=$defPower,enemy=$attPower,";
		$vDP = $this->ecm->call('scrambleFleetPower', $ifl, $defPower);
		$vAP = $this->ecm->call('scrambleFleetPower', $ifl, $attPower);
		$qs .= "v_friendly=$vDP,v_enemy=$vAP,ecm_level=$ecm,eccm_level=$eccm WHERE planet=$planet";
		$this->db->query($qs);
	}


}

?>

<?php

class beta5_fleet_split {

	function beta5_fleet_split($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
		$this->standby	= $this->lib->game->getLib('beta5/standby');
		$this->moving	= $this->lib->game->getLib('beta5/moving');
	}


	// Manually split a fleet in player-specified fleets
	function run($fid, $newName, $count, $sg, $sf, $sc, $sb) {
		// Get fleet data
		$f = $this->lib->call('get', $fid, true);
		if (is_null($f) || $f['can_move'] != 'Y' || !is_null($f['sale_info'])) {
			return 1;
		}

		// Check for enough ships in the original fleet
		$mg = $count * $sg; $mf = $count * $sf;
		$mc = $count * $sc; $mb = $count * $sb;
		if ($f['gaships'] < $mg || $f['fighters'] < $mf || $f['cruisers'] < $mc || $f['bcruisers'] < $mb
		 || $f['gaships'] + $f['fighters'] + $f['cruisers'] + $f['bcruisers'] - ($mg+$mf+$mc+$mb) == 0) {
			return 2;
		}

		// If we're moving or standing by in Hyperspace, we need to make sure both
		// the new and old fleets are HS-capable
		if ((!is_null($f['move']) && $f['move']['hyperspace'] == 't') || !is_null($f['wait'])) {
			$r = $this->rules->call('get', $f['owner']);
			$nu = $r['gaship_space'] * $sg + $r['fighter_space'] * $sf;
			$na = $r['cruiser_haul'] * $sc + $r['bcruiser_haul'] * $sb;
			$ou = $r['gaship_space'] * ($f['gaships'] - $mg) + $r['fighter_space'] * ($f['fighters'] - $mf);
			$oa = $r['cruiser_haul'] * ($f['cruisers'] - $mc) + $r['bcruiser_haul'] * ($f['bcruisers'] - $mb);
			if ($nu > $na || $ou > $oa) {
				return 3;
			}
		}

		// Generate code that will set the new fleets' orders
		if (is_null($f['location'])) {
			$location = "NULL";
			if (is_null($f['moving'])) {
				$moving = "NULL";
				$oCode = '$waiting = $this->standby->call("create", '.$f['wait']['time_left'].",".$f['wait']['drop_point']
					. ','.$f['wait']['origin'].','.$f['wait']['time_spent'].');';
			} else {
				$waiting = "NULL";
				$oCode = '$moving = $this->moving->call("cloneObject", '.$f['moving'].');';
			}
		} else {
			$location = $f['location'];
			$moving = $waiting = 'NULL';
			$oCode = null;
		}

		// Generate new fleets
		$nn = addslashes($newName == "" ? preg_replace('/ [0-9]+$/', '', $f['name']) : $newName);
		for ($i=0;$i<$count;$i++) {
			if ($oCode != "") {
				eval($oCode);
			}
			$nnb = $count > 1 ? (" " . ($i + 1)) : "";
			$this->db->query("INSERT INTO fleet(owner,name,location,gaships,fighters,cruisers,bcruisers,attacking,moving,waiting,time_spent) VALUES("
				.$f['owner'].",'$nn$nnb',$location,$sg,$sf,$sc,$sb,"
				.dbBool($f['attacking'] == 't').",$moving,$waiting,{$f['time_spent']})");
		}

		// Update original fleet
		$this->db->query("UPDATE fleet SET gaships=gaships-$mg,fighters=fighters-$mf,cruisers=cruisers-$mc,bcruisers=bcruisers-$mb "
			."WHERE id=$fid");
		$this->db->query("DELETE FROM beacon_detection WHERE fleet = $fid");

		$this->lib->call('invCache', $fid);
		return 0;
	}
}

?>

<?php

class beta5_moving_cloneObject {

	function beta5_moving_cloneObject($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->standby	= $this->lib->game->getLib('beta5/standby');
	}

	// Clone a moving object
	function run($oid) {
		// Get object record
		$q = $this->db->query("SELECT * FROM moving_object WHERE id=$oid");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		$mo = dbFetchHash($q);

		// Duplicate stand-by order if any
		if (!is_null($mo['wait_order'])) {
			$q = $this->db->query("SELECT * FROM hs_wait WHERE id=".$mo['wait_order']);
			$r = dbFetchHash($q);
			$wo = $this->standby->call('create', $r['time_left'],$r['drop_point'],$r['origin'],$r['time_spent']);
		} else {
			$wo = 'NULL';
		}

		// Insert clone with a random value for time_left
		$rnd = rand($mo['time_left'],999999);
		$this->db->query("INSERT INTO moving_object(m_from,m_to,changed,time_left,hyperspace,wait_order) VALUES ("
			. $mo['m_from'] . "," . $mo['m_to'] . "," . $mo['changed'] . ",$rnd,"
			.dbBool($mo['hyperspace'] == 't').",$wo)");

		// Get new entry's ID
		$q = $this->db->query("SELECT id FROM moving_object WHERE m_from=".$mo['m_from']." AND m_to=".$mo['m_to']." AND time_left=$rnd");
		list($moid) = dbFetchArray($q);

		// Reset time_left to its real value
		$this->db->query("UPDATE moving_object SET time_left=".$mo['time_left']." WHERE id=$moid");

		// Get trajectory data
		$q = $this->db->query("SELECT * FROM waypoint WHERE move_id=$oid");
		$qsl = array();
		while ($r = dbFetchArray($q)) {
			array_push($qsl, "($moid,{$r[1]},{$r[2]})");
		}
		logText(count($qsl) . " waypoint(s) to be copied from object #$oid to object #$moid", LOG_DEBUG);
		foreach ($qsl as $qs) {
			if (!$this->db->query("INSERT INTO waypoint VALUES $qs")) {
				$this->db->query("DELETE FROM moving_object WHERE id=$moid");
				return null;
			}
		}

		return $moid;
	}
}

?>

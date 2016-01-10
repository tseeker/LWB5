<?php

class beta5_fleet_switchStatus {

	function beta5_fleet_switchStatus($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Switches a fleet's status
	function run($id) {
		$f = $this->lib->call('get', $id);
		if ($f['attacking'] == 't') {
			$this->db->query("UPDATE fleet SET attacking=".dbBool(0)." WHERE id=$id");
		} else {
			$this->db->query("UPDATE fleet SET attacking=".dbBool(1)
				.(is_null($f['location'])?"":",can_move='B',time_spent=0")." WHERE id=$id");
		}

		// FIXME: messages

		$this->lib->mainClass->fleets[$id]['attacking'] = ($this->lib->mainClass->fleets[$id]['attacking'] == 't') ? 'f' : 't';
		if ($this->lib->mainClass->fleets[$id]['attacking'] == 't' && !is_null($f['location'])) {
			$this->lib->mainClass->fleets[$id]['can_move'] = 'B';
		}
		logText("beta5/fleet/switchStatus($id): fleet owner {$f['owner']}, switched to " . ($f['attacking'] == 't' ? "def" : "att"), LOG_DEBUG);
	}
}

?>

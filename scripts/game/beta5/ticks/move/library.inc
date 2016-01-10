<?php

//-------------------------------------
// "Move tick": handles moving objects
//-------------------------------------

class beta5_ticks_move_library {

	private $arrivals = array();

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
		$this->fleets	= $lib->game->getLib('beta5/fleet');
		$this->planets	= $lib->game->getLib('beta5/planet');
	}


	public function runTick() {
		// Increase time spent for fleets
		$this->db->safeTransaction(array($this, 'increaseTimeSpent'));

		// Advance moving objects and get the list of arrivals
		$this->db->safeTransaction(array($this, 'advanceMovingObjects'));

		// Fleet arrivals
		if (count($this->arrivals)) {
			$this->db->safeTransaction(array($this, 'fleetArrivals'));
			$this->db->safeTransaction(array($this, 'fleetArrivalUpdates'));
		}

		// Remove completed trajectories
		$this->db->safeTransaction(array($this, 'deleteOldTrajectories'));
	}


	public function deleteOldTrajectories() {
		$this->db->query("DELETE FROM moving_object WHERE time_left = 0");
	}


	public function increaseTimeSpent() {
		// Increase time spent on planets for fleets that are not moving
		$this->db->query("UPDATE fleet SET time_spent = time_spent + 1 WHERE location IS NOT NULL");
	}


	public function advanceMovingObjects() {
		// Advances all moving objects that don't suffer a destination change penalty
		$this->db->query("UPDATE moving_object SET time_left = time_left - 1 WHERE changed=0 AND time_left > 0");

		// Decreases penalty
		$this->db->query("UPDATE moving_object SET changed = 61 WHERE changed > 60");
		$this->db->query("UPDATE moving_object SET changed = changed - 1 WHERE changed > 0");

		// Get all moving objects arriving at destination
		$q = $this->db->query("SELECT id, m_to, m_from, wait_order FROM moving_object WHERE time_left = 0");
		if (!dbCount($q)) {
			return;
		}
		$mobjs = array();
		while ($r = dbFetchArray($q)) {
			$mobjs[$r[0]] = array($r[1], $r[2], $r[3]);
		}

		$this->arrivals = $mobjs;
	}


	function fleetArrivals() {
		// Find moving fleets
		$q = $this->db->query(
			"SELECT id, moving FROM fleet WHERE moving IN (" . join(',', array_keys($this->arrivals)) . ")"
		);
		$planets = array(); $plDetect = array();
		while ($r = dbFetchArray($q)) {
			list($fId,$mId) = $r;

			// Does it have an HS stand-by order?
			if (!is_null($this->arrivals[$mId][2])) {
				$this->db->query(
					"UPDATE hs_wait SET origin={$this->arrivals[$mId][1]}, "
							. "drop_point={$this->arrivals[$mId][0]}"
						. " WHERE id={$this->arrivals[$mId][2]}"); 
				$this->db->query("UPDATE fleet SET moving=NULL,waiting="
					. $this->arrivals[$mId][2] . " WHERE id=$fId");

				// Add the planet to the list of planets where detection is to be run
				if (!in_array($this->arrivals[$mId][0], $plDetect)) {
					array_push($plDetect, $this->arrivals[$mId][0]);
				}
			} else {
				// If there is no HS stand-by order, the fleet has arrived.
				$this->fleets->call('arrival', $fId, $this->arrivals[$mId][0], $this->arrivals[$mId][1]);

				// Add the planet to the list of planets to be updated
				if (!in_array($this->arrivals[$mId][0], $planets)) {
					array_push($planets, $this->arrivals[$mId][0]);
				}
			}
		}

		$this->plDetect = $plDetect;
		$this->plUpdate = $planets;
	}

	public function fleetArrivalUpdates() {
		// Update planet status
		foreach	($this->plDetect as $pid) {
			$this->planets->call('detectFleets', $pid);
		}
		foreach	($this->plUpdate as $pid) {
			$this->planets->call('updateHappiness', $pid);
			$this->planets->call('updateMilStatus', $pid);
		}
		$this->fleets->call('sendMoveMessages');
	}
}


?>

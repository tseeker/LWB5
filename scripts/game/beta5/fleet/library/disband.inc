<?php

class beta5_fleet_disband {

	function beta5_fleet_disband($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->sales	= $this->lib->game->getLib('beta5/sale');
	}


	// Disbands a fleet and removes any sales / movement / stand-by
	// entries associated with it
	function run($fId, $final = false) {
		// Get fleet data
		$f = $this->lib->call('get', $fId);
		if (is_null($f)) {
			return 1;
		}

		if (!is_null($f['sale_info'])) {
			// It is for sale
			$r = $this->sales->call('cancel', $f['owner'], $f['sale_info']['sale']['id']);
			if (!($r || $final)) {
				return 2;
			} elseif (!$r) {
				// Sale was finalized and we can't cancel; assume no planet is for sale.
				// FIXME: send message
				$this->db->query("UPDATE fleet SET sale=NULL,owner=".$f['sale_info']['sale']['sold_to']." WHERE id=$fId");
				// FIXME: add history
				$this->db->query("DELETE FROM sale WHERE id=".$f['sale_info']['sale']['id']);
			}
		} elseif (!is_null($f['waiting'])) {
			// It is standing by
			$this->db->query("DELETE FROM hs_wait WHERE id=".$f['waiting']);
		} elseif (!is_null($f['moving'])) {
			// It's moving?
			$this->db->query("DELETE FROM moving_object WHERE id=".$f['moving']);
			if (!is_null($f['move']['wait_order'])) {
				$this->db->query("DELETE FROM hs_wait WHERE id=".$f['move']['wait_order']);
			}
		}

		// Remove this fleet
		$this->db->query("DELETE FROM fleet WHERE id=$fId");

		// Update planet status where the fleet was
		if (!is_null($f['location'])) {
			$this->planets->call('updateMilStatus', $f['location']);
			$this->planets->call('updateHappiness', $f['location']);
		}

		return 0;
	}
}

?>

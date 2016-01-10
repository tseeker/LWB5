<?php

class beta5_fleet_get {

	function beta5_fleet_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns data regarding a fleet
	function run($id, $update = false) {
		if ($update) {
			$uqs = " FOR UPDATE";
		} else {
			$uqs = "";
		}

		// Return the fleet cache's contents if the fleet is in there
		if (!is_null($this->lib->mainClass->fleets[$id])) {
			return $this->lib->mainClass->fleets[$id];
		}

		// Get the complete row
		$q = $this->db->query("SELECT * FROM fleet WHERE id = $id $uqs");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		$fdata = dbFetchHash($q);

		// Extract movement data
		if (!is_null($fdata['moving'])) {
			$q = $this->db->query("SELECT * FROM moving_object WHERE id=" . $fdata['moving'] . $uqs);
			if ($q && dbCount($q) == 1) {
				$fdata['move'] = dbFetchHash($q);
				if (!is_null($fdata['move']['wait_order'])) {
					$fdata['waiting'] = $fdata['move']['wait_order'];
				}
			}
		}

		// Extract HS standby orders
		if (!is_null($fdata['waiting'])) {
			$q = $this->db->query("SELECT * FROM hs_wait WHERE id=" . $fdata['waiting'] . $uqs);
			if ($q && dbCount($q) == 1) {
				$fdata['wait'] = dbFetchHash($q);
			}
		}

		// Extract sales data
		$q = $this->db->query("SELECT * FROM sale WHERE fleet=" . $fdata['id'] . $uqs);
		if (dbCount($q)) {
			$a = array('sale' => dbFetchHash($q));
			$q = $this->db->query("SELECT * FROM public_offer WHERE offer=" . $a['sale']['id'] . $uqs);
			if ($q && dbCount($q) == 1) {
				$a['public'] = dbFetchHash($q);
			}
			$q = $this->db->query("SELECT * FROM private_offer WHERE offer=" . $a['sale']['id'] . $uqs);
			if ($q && dbCount($q) == 1) {
				$a['private'] = dbFetchHash($q);
			}
			$fdata['sale_info'] = $a;
		}

		return ($this->lib->mainClass->fleets[$id] = $fdata);
	}
}

?>

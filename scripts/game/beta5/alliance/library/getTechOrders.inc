<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/getTechOrders.inc
//
// This function retrieves all technology trading orders for an alliance
// from the database.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_getTechOrders {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($alliance) {
		// Get the orders for all players
		$q = $this->db->query(
			"SELECT player, send_to, tech, submitted, obeyed FROM tech_trade_order "
				. "WHERE alliance = $alliance"
		);

		// Create the list of orders
		$orders = array();
		while ($r = dbFetchArray($q)) {
			if (is_null($r[4])) {
				$r[4] = 0;
			}

			if (!is_array($orders[$r[0]])) {
				$orders[$r[0]] = array();
			}
			$orders[$r[0]]['sendTo'] = $r[1];
			$orders[$r[0]]['sendTech'] = $r[2];
			$orders[$r[0]]['sendSub'] = $r[3];
			$orders[$r[0]]['sendDone'] = $r[4];

			if (!is_array($orders[$r[1]])) {
				$orders[$r[1]] = array();
			}
			$orders[$r[1]]['recvFrom'] = $r[0];
			$orders[$r[1]]['recvTech'] = $r[2];
			$orders[$r[1]]['recvSub'] = $r[3];
			$orders[$r[1]]['recvDone'] = $r[4];
		}

		return $orders;
	}

}

?>

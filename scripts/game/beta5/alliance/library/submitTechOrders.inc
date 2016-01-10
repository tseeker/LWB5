<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/submitTechOrders.inc
//
// This function updates the alliance's orders for the next 24 hours.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_submitTechOrders {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->msgs	= $this->game->getLib('beta5/msg');
	}

	public function run($alliance, $orders) {
		// List all players included in the orders
		$players = array_keys($orders);
		foreach ($orders as $oPid => $order) {
			if (!in_array($order[1], $players)) {
				array_push($players, $order[1]);
			}
		}

		// Check if the players are in the alliance
		$q = $this->db->query(
			"SELECT id FROM player WHERE id IN (" . join(',', $players) . ") "
				. "AND (alliance <> $alliance OR a_status <> 'IN ')"
		);
		if (dbCount($q)) {
			return false;
		}

		// Get the tech lists for all players
		$techList = $this->lib->call('getTechList', $alliance, false);

		// Get all dependencies
		$dependencies = $this->getDependencies();

		// For each order, check if the sender had submitted the technology as
		// either New, Implemented or Law, and whether he can actually send
		// technologies. Also check if the receiver had submitted his list, if
		// he can receive techs and if the list doesn't contain the technology
		// to be sent. Oh, and if he has the dependencies.
		foreach ($orders as $sender => $order) {
			list($tech, $receiver) = $order;

			if (!is_array($techList[$sender]) || !is_array($techList[$receiver])
					|| $sender == $receiver || !$techList[$sender]['submitted']
					|| $techList[$sender]['vacation'] || $techList[$sender]['restrict']
					|| !$techList[$receiver]['submitted'] || $techList[$receiver]['vacation']
					|| $techList[$receiver]['restrict']
					|| !array_key_exists($tech, $techList[$sender]['list'])
					|| $techList[$sender]['list'][$tech] == 'F'
					|| array_key_exists($tech, $techList[$receiver]['list']) ) {
				return false;
			}

			if (is_array($dependencies[$tech])) {
				foreach ($dependencies[$tech] as $dep) {
					if (!array_key_exists($dep, $techList[$receiver]['list'])) {
						l::trace("dep $dep not found");
						return false;
					}
					$dStatus = $techList[$receiver]['list'][$dep];
					if ($dStatus != 'I' && $dStatus != 'L') {
						return false;
					}
				}
			}
		}

		// Delete current orders
		$this->db->query("DELETE FROM tech_trade_order WHERE alliance = $alliance");

		// Insert new orders
		foreach ($orders as $sender => $order) {
			list($tech, $receiver) = $order;
			$this->db->query(
				"INSERT INTO tech_trade_order (alliance, player, send_to, tech) "
					. "VALUES($alliance, $sender, $receiver, $tech)"
			);
		}

		// Send private message to all players included in the exchange
		$q = $this->db->query("SELECT tag FROM alliance WHERE id = $alliance");
		list($tag) = dbFetchArray($q);
		foreach ($players as $player) {
			$this->msgs->call('send', $player, 'alint', array(
				"msg_type"	=> 20,
				"tag"		=> $tag,
				"alliance"	=> $alliance
			));
		}
		return true;
	}


	private function getDependencies() {
		$q = $this->db->query("SELECT research, depends_on FROM research_dep");
		$deps = array();
		while ($r = dbFetchArray($q)) {
			if (!is_array($deps[$r[0]])) {
				$deps[$r[0]] = array();
			}
			array_push($deps[$r[0]], $r[1]);
		}
		return $deps;
	}
}

?>

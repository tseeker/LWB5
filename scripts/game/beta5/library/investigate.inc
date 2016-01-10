<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/library/investigate.inc
//
// This function investigates two accounts in a game, checking for
// suspicious events such as donations, sales, tech exchange, or planets
// retaken by one of the players after having been abandonned by the
// other.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_investigate {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($player1, $player2, $now, $delay) {
		// Create the events record, generate timing part of the queries
		$events = array(
			"LSE"	=> 0,
			"SE"	=> 0,
			"HSE"	=> 0,
			"VHSE"	=> 0
		);
		$tQuery = "BETWEEN " . (1 + $now - $delay) . " AND $now";

		// Get the amount of donations in the interval
		$q = $this->db->query(
			"SELECT amount FROM donation_log "
				. "WHERE \"time\" $tQuery "
				  . "AND (source = $player1 AND target = $player2 "
					. "OR source = $player2 AND target = $player1)"
		);
		while ($r = dbFetchArray($q)) {
			list($amount) = $r;
			if ($amount > 10000000) {
				$et = "VHSE";
			} elseif ($amount > 1000000) {
				$et = "HSE";
			} elseif ($amount > 100000) {
				$et = "SE";
			} else {
				$et = "LSE";
			}
			$events[$et] ++;
		}

		// Get the amount of sales in the interval
		$q = $this->db->query(
			"SELECT sell_price FROM sale_history "
				. "WHERE ended $tQuery "
				  . "AND (from_player = $player1 AND to_player = $player2 "
					. "OR from_player = $player2 AND to_player = $player1)"
		);
		while ($r = dbFetchArray($q)) {
			list($price) = $r;
			if (is_null($price)) {
				continue;
			}

			if ($price == 0) {
				$et = "HSE";
			} else {
				$et = "SE";
			}
			$events[$et] ++;
		}

		// Get planets that were abandonned by one and retaken by the other
		$q = $this->db->query(
			"SELECT planet FROM abandon_log "
				. "WHERE retake_time $tQuery AND retake_time - abandon_time < 5 * 86400 "
				  . "AND (former_owner = $player1 AND retake_owner = $player2 "
					. "OR former_owner = $player2 AND retake_owner = $player1)"
		);
		while ($r = dbFetchArray($q)) {
			$events["SE"] ++;
		}

		// Get tech exchanges
		$q = $this->db->query(
			"SELECT accepted, price FROM research_assistance "
				. "WHERE moment $tQuery "
				  . "AND (player = $player1 AND offer_to = $player2 "
					. "OR player = $player2 AND offer_to = $player1)"
		);
		while ($r = dbFetchArray($q)) {
			list($accepted, $price) = $r;
			if ($accepted == 'f') {
				$et = "LSE";
			} elseif ($price > 1000 || is_null($accepted)) {
				$et = "SE";
			} elseif ($price < 1000) {
				$et = "HSE";
			}
			$events[$et] ++;
		}

		// Generate the actual array of events
		$actualEvents = array("CHECK");
		foreach ($events as $eType => $eCount) {
			if ($eCount > 0) {
				array_push($actualEvents, "$eType-$eCount");
			}
		}
		return $actualEvents;
	}
}


?>

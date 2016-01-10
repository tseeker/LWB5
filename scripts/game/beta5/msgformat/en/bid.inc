<?php

class msgformat_bid {

	function msgformat_bid($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Foreign Minister';
	}

	function getSLink() {
		return "";
	}

	function getRecipient() {
		return utf8entities($this->players->call('getName', $this->player));
	}

	function getRLink() {
		return "";
	}

	function getSubject() {
		$s = $this->data['is_planet'] == 't' ? "planet" : "fleet at";
		return "New bid on $s " . utf8entities($this->data['pname']);
	}

	function getReplyLink() {
		return "";
	}

	function getFleetText() {
		$a = array();
		$ids = array(
			'gas'		=> "G.A. Ship",
			'fighters'	=> "Fighter",
			'cruisers'	=> "Cruiser",
			'bcruisers'	=> "Battle Cruiser",
		);
		foreach	($ids as $i => $n) {
			if ($this->data["f_$i"] == 0) {
				continue;
			}
			$nb = $this->data["f_$i"];
			array_push($a, "<b>$nb</b> $n" . ($nb > 1 ? "s" : ""));
		}
		return join(', ', $a);
	}

	function getContents() {
		if (is_null($this->data['last_bidder'])) {
			$str  = "Sir, we had placed a bid on " . ($this->data['is_planet'] == 't' ? "planet" : "a fleet in orbit around") . " ";
			if (!is_null($this->data['planet'])) {
				$str .= "<a href='planet?id=" . $this->data['planet'] . "'>";
			}
			$str .= "<b>" . utf8entities($this->data['pname']) . "</b>" . (is_null($this->data['planet']) ? "" : "</a>");
			$fs = $this->getFleetText();
			if ($this->data['is_planet'] == 'f') {
				$str .= " ($fs)";
			} elseif ($fs != '') {
				$str .= " (along with the following fleet: $fs)";
			}
			$str .= ".<br/>We have been informed that someone just placed a higher bid. The new price is <b>&euro;";
			$str .= number_format($this->data['new_price']) . "</b>.";
			if (!is_null($this->data['offer'])) {
				$str .= '<br/>We should <a href="market?p=p&po='.$this->data['offer'].'">place a higher bid</a>.';
			}
		} else {
			$str  = "Sir, we have received an offer for " . ($this->data['is_planet'] == 't' ? "planet" : "the fleet in orbit around") . " ";
			if (!is_null($this->data['planet'])) {
				$str .= "<a href='planet?id=" . $this->data['planet'] . "'>";
			}
			$str .= "<b>" . utf8entities($this->data['pname']) . "</b>" . (is_null($this->data['planet']) ? "" : "</a>");
			$fs = $this->getFleetText();
			if ($this->data['is_planet'] == 'f') {
				$str .= " ($fs)";
			} elseif ($fs != '') {
				$str .= " (being sold with the following fleet: $fs)";
			}
			$str .= ".<br/>";

			$pinf = $this->players->call('get', $this->data['last_bidder'], true);
			if (!$pinf['quit']) {
				$str .= '<a href="message?a=c&ct=0&id='.$this->data['last_bidder'].'">';
			}
			$str .= utf8entities($pinf['name']);
			if (!$pinf['quit']) {
				$str .= '</a>';
			}
			$str .= " is offering to buy the " . ($this->data['is_planet'] == 'f' ? "fleet" : "planet") . " for <b>&euro;";
			$str .= number_format($this->data['new_price']) . "</b>.";
		}
		return $str;
	}
}

?>

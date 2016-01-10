<?php

class msgformat_kfleet {

	function msgformat_kfleet($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Military Advisor';
	}

	function getSLink() {
		return "";
	}

	function getRecipient() {
		$pinf = $this->players->call('getName', $this->player);
		return utf8entities($pinf);
	}

	function getRLink() {
		return "";
	}

	function getSubject() {
		return "Fleets lost due to insufficient funds";
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$str = "Sir! Due to insufficient funds, we have been unable to pay for our fleets' upkeep. ";

		$types = array('gaships','fighters','cruisers','bcruisers');
		$names = array('G.A. ship', 'fighter', 'cruiser', 'battle cruiser');
		$sum = 0;
		for ($i=0;$i<4;$i++) {
			$sum += $this->data[$types[$i]];
		}

		$add = 0;
		for ($i=0;$i<4;$i++) {
			$n = $this->data[$types[$i]];
			if ($n == 0) {
				continue;
			}
			$sum -= $n;
			if ($sum == 0 && $add != 0) {
				$str .= ' and ';
			} elseif ($add != 0) {
				$str .= ', ';
			}
			$add += $n;

			$str .= "<b>".number_format($n)."</b> " . $names[$i] . ($n > 1 ? 's' : '');
		}
		$str .= " ha" . ($add > 1 ? "ve" : "s") . " been lost.";

		return $str;
	}
}

?>

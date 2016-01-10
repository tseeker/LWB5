<?php

class msgformat_kimpr {

	function msgformat_kimpr($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Civilian Advisor';
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
		return "Improvements lost due to insufficient funds";
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$str = "Sir! Due to insufficient funds, we have been unable to pay for our planetary improvements' upkeep. ";
		$tl = $this->data['turrets']; $ml = $this->data['factories'];
		if ($tl > 0) {
			$str .= "<b>".number_format($tl)."</b> turret" . ($tl>1?'s':'');
			if ($ml > 0) {
				$str .= " and ";
			}
		}
		if ($ml > 0) {
			$str .= "<b>".number_format($ml)."</b> military factor" . ($ml>1?"ies":"y");
		}
		$str .= " ha" . ($ml+$tl>1?"ve":"s") . " been destroyed!";

		return $str;
	}
}

?>

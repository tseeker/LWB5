<?php

class msgformat_revolt {

	function msgformat_revolt($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Governor of ' . utf8entities($this->data['pname']);
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
		if ($this->data['started'] == 't') {
			return "Civil unrest on " . utf8entities($this->data['pname']);
		}
		return "End of the uprising on " . utf8entities($this->data['pname']);
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$planet = '<a href="planet?id='.$this->data['planet'].'"><b>'.utf8entities($this->data['pname'])."</b></a>";
		if ($this->data['started'] == 't') {
			$str  = "Sir! The citizens of $planet are most displeased with their treatment! "
				. "They are protesting in the streets at being so unhappy! The riots are intensifying, "
				. "and we suspect that they will soon start destroying our infrastructure if no action "
				. "is taken!";
		} else {
			$str = "Sir, the riots on planet $planet have ended.";
		}
		return $str;
	}
}

?>

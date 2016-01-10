<?php

class msgformat_leave {

	function msgformat_leave($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Legacy Worlds';
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
		$pinf = $this->players->call('get', $this->data['player'], true);
		$str = utf8entities($pinf['name']) . " has ";
		switch ($this->data['reason']) :
			case 'QUIT': $str .= "quit the game"; break;
			case 'INAC': $str .= "become inactive"; break;
			case 'KICK': $str .= 'been kicked from the game'; break;
		endswitch;
		return $str;
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$pinf = $this->players->call('get', $this->data['player'], true);
		$str = "<b>" . utf8entities($pinf['name']) . "</b> ";
		switch ($this->data['reason']) :
			case 'QUIT': $str .= "has decided he had enough of this world and has just left the game."; break;
			case 'INAC': $str .= "has disappeared, it would seem. He hasn't logged on for 30 days."; break;
			case 'KICK': $str .= 'probably did something very, very bad and got kicked from the game.'; break;
		endswitch;

		if (!is_null($this->data['tag']))
			$str .= " He has been removed from the members of the <b>" . utf8entities($this->data['tag'])
				. "</b> alliance.";
		if ($this->data['ally'] == 't')
			$str .= " Since you had him in your Trusted Allies list, he has been removed from it.";
		$str .= "<br/>";

		if ($this->data['sales_from'] + $this->data['sales_to'] > 0) {
			$str .= "<br/>";
			if ($this->data['sales_from'] > 0)
				$str .= "<b>" . number_format($this->data['sales_from']) . "</b> sale"
					. ($this->data['sales_from']>1?"s":"") . " from this player ha"
					. ($this->data['sales_from']>1?"ve":"s") . " been cancelled.<br/>";
			if ($this->data['sales_to'] > 0)
				$str .= "<b>" . number_format($this->data['sales_to']) . "</b> sale"
					. ($this->data['sales_to']>1?"s":"") . " to this player ha"
					. ($this->data['sales_to']>1?"ve":"s") . " been cancelled.<br/>";
		}

		$str .= "<br/><b>" . utf8entities($pinf['name']) . "</b> will"
			. ($this->data['reason'] == 'KICK' ? " <i>NOT</i>" : "")
			. " be missed.";

		return $str;
	}
}

?>

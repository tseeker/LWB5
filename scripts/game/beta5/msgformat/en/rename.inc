<?php

class msgformat_rename {

	function msgformat_rename($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Head of Intelligence';
	}

	function getSLink() {
		return "";
	}

	function getRecipient() {
		$pinf = $this->players->call('getName', $this->player);
		return utf8entities($pinf,ENT_COMPAT);
	}

	function getRLink() {
		return "";
	}

	function getSubject() {
		return "Planet " . utf8entities($this->data['old_name'],ENT_COMPAT) . " has been renamed";
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$str = "Sir! A planet we were ";
		switch ($this->data['status']) :
			case 'ORBIT': $str .= "orbitting"; break;
			case 'MOVE': $str .= "moving towards"; break;
			case 'PROBE': $str .= "shamelessly spying on with our probes"; break;
		endswitch;
		$str .= ", <b>" . utf8entities($this->data['old_name'],ENT_COMPAT) . "</b>, has been renamed "
			. "by its owner to <a href='planet?id={$this->data['planet']}'><b>"
			. utf8entities($this->data['new_name'],ENT_COMPAT) . "</b></a>.";

		return $str;
	}
}

?>

<?php

class msgformat_alliance {

	function msgformat_alliance($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return utf8entities($this->players->call('getName', $this->data['sender']));
	}

	function getSLink() {
		if ($this->data['sender'] == $this->player) {
			return "";
		}
		$pinf = $this->players->call('get', $this->data['sender']);
		if (is_null($pinf)) {
			return "";
		}
		return "0,".$this->data['sender'];
	}

	function getRecipient() {
		return "Alliance [".utf8entities($this->data['tag']) . "]";
	}

	function getRLink() {
		if (is_null($this->data['alliance']))
			return "";
		return "2,".$this->data['alliance'];
	}

	function getSubject() {
		return utf8entities($this->data['subject']);
	}

	function getReplyLink() {
		if ($this->data['sender'] == $this->player || is_null($this->data['alliance'])) {
			return "";
		}
		return "2,".$this->data['alliance'].",".$this->data['id'];
	}

	function getContents() {
		return preg_replace('/\n/', '<br/>', utf8entities($this->data['message']));
	}
}

?>

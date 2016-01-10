<?php

class msgformat_std {

	function msgformat_std($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		$pinf = $this->players->call('get', $this->data['sender'], true);
		return utf8entities($pinf['name']);
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
		$pinf = $this->players->call('get', $this->data['recipient'], true);
		return utf8entities($pinf['name']);
	}

	function getRLink() {
		if ($this->data['recipient'] == $this->player) {
			return "";
		}
		$pinf = $this->players->call('get', $this->data['recipient']);
		if (is_null($pinf)) {
			return "";
		}
		return "0,".$this->data['recipient'];
	}

	function getSubject() {
		return utf8entities($this->data['subject']);
	}

	function getReplyLink() {
		if ($this->data['sender'] == $this->player) {
			return "";
		}
		$pinf = $this->players->call('get', $this->data['sender']);
		if (is_null($pinf)) {
			return "";
		}
		return "0,".$this->data['sender'].",".$this->data['id'];
	}

	function getContents() {
		return preg_replace('/\n/', '<br/>', utf8entities($this->data['message']));
	}
}

?>

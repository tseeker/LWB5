<?php

class msgformat_planet {

	function msgformat_planet($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
		$this->planets	= $game->getLib('beta5/planet');
	}

	function getSender() {
		$pinf = $this->players->call('getName', $this->data['sender']);
		return utf8entities($pinf);
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
		return "Planet ".utf8entities($this->data['pname']);
	}

	function getRLink() {
		if (is_null($this->data['planet'])) {
			return "";
		}
		$pinf = $this->planets->call('byId', $this->data['planet']);
		if ($pinf['owner'] == $this->player) {
			return "";
		}
		return "1,".$this->data['planet'];
	}

	function getSubject() {
		return utf8entities($this->data['subject']);
	}

	function getReplyLink() {
		if ( $this->data['sender'] == $this->player || is_null($this->data['planet'])) {
			return "";
		}
		return "0,".$this->data['sender'].",".$this->data['id'];
	}

	function getContents() {
		return preg_replace('/\n/', '<br/>', utf8entities($this->data['message']));
	}
}

?>

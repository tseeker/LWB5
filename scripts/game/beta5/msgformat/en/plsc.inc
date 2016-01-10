<?php

class msgformat_plsc {

	function msgformat_plsc($game) {
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
		return "Transfer of planet " . utf8entities($this->data['p_name']) . " cancelled!";
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$p1 = $this->players->call('get', $this->data['seller'], true);
		$p1s = ($p1['quit'] == 0 ? "<a href='message?a=c&ct=0&id={$this->data['seller']}'>" : "")
			. "<b>" . utf8entities($p1['name']) . "</b>" . ($p1['quit'] == 0 ? "</a>" : "");

		$p2 = $this->players->call('get', $this->data['taker'], true);
		$p2s = ($p2['quit'] == 0 ? "<a href='message?a=c&ct=0&id={$this->data['taker']}'>" : "")
			. "<b>" . utf8entities($p2['name']) . "</b>" . ($p2['quit'] == 0 ? "</a>" : "");

		return "The sale of planet <a href='planet?id={$this->data['p_id']}'><b>"
				. utf8entities($this->data['p_name']) . "</b></a> has been cancelled.<br/>"
				. "The seller, $p1s, lost the planet to the forces of $p2s.";
	}
}

?>

<?php

class msgformat_cash {

	function msgformat_cash($game) {
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
		return utf8entities($this->players->call('getName', $this->player),ENT_COMPAT);
	}

	function getRLink() {
		return "";
	}

	function getSubject() {
		return	"Cash donation received!";
	}

	function getReplyLink() {
		return	"";
	}

	function getContents() {
		$pinf = $this->players->call('get', $this->data['p_id'], true);
		return "Sir! We just received a cash donation amounting to <b>&euro;"
			. number_format($this->data['amount']) . "</b> from player "
			. ($pinf['quit'] == 1 ? '' : "<a href='message?a=c&ct=0&id={$this->data['p_id']}'>")
			. "<b>" . utf8entities($pinf['name'],ENT_COMPAT) . "</b>" . ($pinf['quit'] == 1 ? "" : "</a>") . "!";
	}
}

?>

<?php

class msgformat_warnname {

	function msgformat_warnname($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'LW Moderators';
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
		switch ($this->data['event']) :
			case 'WARN ': $s = '/!\\ PLANET NAME WARNING /!\\'; break;
			default: $s = '/!\\ PLANET RESET /!\\'; break;
		endswitch;
		return $s;
	}

	function getReplyLink() {
		return	"";
	}

	function getContents() {
		if ($this->data['event'] == 'WARN ') {
			$t = "This is an <b>official warning</b> from the Legacy Worlds moderation team.<br/>"
				. "One of the planets you own, <a href='planet?id={$this->data['planet']}'><b>"
				. utf8entities($this->data['p_name'],ENT_COMPAT) . "</b></a>, has a name that is considered either "
				. "vulgar, disrespectful or discriminating (please check the <a target='_blank' href='"
				. "manual?p=good_practices'>manual</a> for more details).<br/>"
				. "The planet will be made neutral and reset to its initial state (2k population, 3 factories "
				. "of each type and 3 turrets) if you don't rename it within 24h or if the new name is "
				. "found to be offensive as well.";
		} elseif ($this->data['event'] == 'NEUT ') {
			$t = "Your planet <b>" . utf8entities($this->data['p_name'],ENT_COMPAT) . "</b> has been neutralized and reset "
				. "by the Legacy Worlds moderation team; you had been warned regarding its name and renamed it "
				. "to something offensive.";
		} else {
			$t = "Your planet <b>" . utf8entities($this->data['p_name'],ENT_COMPAT) . "</b> has been automatically neutralized "
				. "and reset; you had been warned regarding its name and did not rename it within 24h.";
		}
		return $t;
	}
}

?>

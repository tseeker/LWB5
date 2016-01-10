<?php

class msgformat_flswitch {

	function msgformat_flswitch($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Military Advisor';
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
		return "Fleets forced to attack on " . utf8entities($this->data['pname']);
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		if (!is_null($this->data['planet'])) {
			$a1 = '<a href="planet?id='.$this->data['planet'].'"><b>';
			$a2 = '</b></a>';
		} else {
			$a1="<b>";$a2="</b>";
		}
		$str  = "Sir, the owner of planet $a1".utf8entities($this->data['pname'])."$a2 has added us to his enemy list.<br/>";
		$str .= "Our fleets have been forced to attack the planet as a consequence and it will not be possible to switch them back ";
		$str .= "unless the owner removes us from his enemy list. Our fleets are stuck there until a Battle Tick happens.<br/>";

		$nf = $this->data['n_fleets'];
		$str .= "<b>$nf</b> of our fleets " . ($nf>1?'are':'is') . " in orbit around the planet, with a total fleet power of <b>";
		$str .= number_format($this->data['fpower']) . "</b>.";
		return $str;
	}
}

?>

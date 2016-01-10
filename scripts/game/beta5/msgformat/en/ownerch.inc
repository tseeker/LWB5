<?php

class msgformat_ownerch {

	function msgformat_ownerch($game) {
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
		switch ($this->data['status']) :
			case 'TAKE': $s = "We have taken control of "; break;
			case 'LOSE': $s = "We have lost control of "; break;
			case 'VIEW': $s = "Owner change on "; break;
		endswitch;
		$s .= utf8entities($this->data['p_name']);
		return $s;
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$planet = '<a href="planet?id='.$this->data['p_id'].'"><b>'.utf8entities($this->data['p_name'])."</b></a>";
		switch ($this->data['status']) :
		case 'TAKE':
			$s = "Sir! We have seized control of planet $planet";
			if (!is_null($this->data['owner'])) {
				$pid = $this->data['owner'];
				$pn = utf8entities($this->players->call('getName', $pid));
				$s .= " from the clutches of <a href='message?a=c&ct=0&id=$pid'><b>$pn</b></a>, "
					. "its former owner";
			} else {
				$s .= " from the local (lack of) government.";
			}
			$s .= ".";
			break;
		case 'LOSE':
			$pid = $this->data['owner'];
			$pn = utf8entities($this->players->call('getName', $pid));
			$s = "We have lost control of planet $planet, which was taken from us by "
				. "<a href='message?a=c&ct=0&id=$pid'><b>$pn</b></a>.";
			break;
		case 'VIEW':
			$pid = $this->data['owner'];
			$pn = utf8entities($this->players->call('getName', $pid));
			$s = "Sir! Our forces orbitting $planet report that "
				. "<a href='message?a=c&ct=0&id=$pid'><b>$pn</b></a> has taken "
				. "control of the planet.";
			break;
		endswitch;
		return $s;
	}
}

?>

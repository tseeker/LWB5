<?php

class msgformat_alint {

	function msgformat_alint($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getPlayer() {
		if (is_null($this->data['player']))
			return null;
		return $this->players->call('get', $this->data['player']);
	}

	function getSender() {
		return "Alliance [".utf8entities($this->data['tag']) . "]";
	}

	function getSLink() {
		if (is_null($this->data['alliance'])) {
			return "";
		}
		return "2,".$this->data['alliance'];
	}

	function getRecipient() {
		$name = $this->players->call('getName', $this->player);
		return utf8entities($name);
	}

	function getRLink() {
		return "";
	}

	function getSubject() {
		$p = $this->getPlayer();
		switch ($this->data['msg_type']) :
			case 0: $s = "A player has requested to join the alliance!"; break;
			case 1:  $s = "A player has cancelled his request to join!"; break;
			case 2:  $s = "You have been removed as " . utf8entities($p['name']) ."'s successor"; break;
			case 3:  $s = "You have been made " . utf8entities($p['name']) . "'s successor"; break;
			case 4:  $s = "[".utf8entities($this->data['tag'])."] has become a democracy"; break;
			case 5:  $s = "[".utf8entities($this->data['tag'])."] has become a dictature"; break;
			case 6:  $s = "You have stepped down from the head of [".utf8entities($this->data['tag'])."]"; break;
			case 7:  $s = "You are the new leader of [".utf8entities($this->data['tag'])."]"; break;
			case 8:  $s = "Leadership change in [".utf8entities($this->data['tag'])."]"; break;
			case 9:  $s = "A player has left the alliance"; break;
			case 10: $s = "Request granted!"; break;
			case 11: $s = "A player has been accepted into the alliance"; break;
			case 12: $s = "Request rejected!"; break;
			case 13: $s = "A request to join has been denied"; break;
			case 14: $s = "Leadership change in [".utf8entities($this->data['tag'])."]"; break;
			case 15: $s = "[".utf8entities($this->data['tag'])."] no longer has a leader!"; break;
			case 16: $s = "New candidate for presidency"; break;
			case 17: $s = "A candidate is no longer running for the alliance's presidency"; break;
			case 18: $s = "You have taken the alliance's presidency"; break;
			case 19: $s = "Leadership change in [".utf8entities($this->data['tag'])."]"; break;
			case 20: $s = "New technology trading orders issued"; break;
		endswitch;
		return $s;
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$p = $this->getPlayer();
		switch ($this->data['msg_type']) :
		case 0:
			$s  = "Player <b>".utf8entities($p['name'])."</b> has sent a request to join the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
			// FIXME: link to admission page
		break;
		case 1:
			$s  = "Player <b>".utf8entities($p['name'])."</b> has cancelled his request to join the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
		break;
		case 2:
			$s  = "You are no longer <b>".utf8entities($p['name'])."</b>'s successor at the head of <b>[".utf8entities($this->data['tag'])."]</b>.";
		break;
		case 3:
			$s  = "You have been designated by <b>".utf8entities($p['name'])."</b> to be his successor at the head of <b>[".utf8entities($this->data['tag'])."]</b>. ";
			$s .= "If <b>".utf8entities($p['name'])."</b> leaves the alliance, you will automatically be in charge.";
		break;
		case 4:
			$s  = "By orders of <b>".utf8entities($p['name'])."</b>, the <b>[".utf8entities($this->data['tag'])."]</b> alliance has become a democracy.";
		break;
		case 5:
			$s  = "By orders of <b>".utf8entities($p['name'])."</b>, the <b>[".utf8entities($this->data['tag'])."]</b> alliance has become a dictature.";
		break;
		case 6:
			$s  = "You have stepped down from the head of the <b>[".utf8entities($this->data['tag'])."]</b> alliance.<br/><b>".utf8entities($p['name'])."</b>,";
			$s .= "your successor, has assumed control of the alliance.";
		break;
		case 7:
			$s  = "<b>".utf8entities($p['name'])."</b>, who was the leader of the <b>[".utf8entities($this->data['tag'])."]</b> alliance, has stepped down from ";
			$s .= "power.<br/>As his successor, you are the new leader of the alliance.";
		break;
		case 8:
			$s  = "The leader of the <b>[".utf8entities($this->data['tag'])."]</b> alliance has stepped down from power.<br/>";
			$s .= "<b>".utf8entities($p['name'])."</b>, his successor, is the new leader of the alliance.";
		break;
		case 9:
			$s  = "<b>".utf8entities($p['name'])."</b> has left the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
		break;
		case 10:
			$s  = "You have been accepted into the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
		break;
		case 11:
			$s  = "Player <b>".utf8entities($p['name'])."</b> has been accepted into the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
		break;
		case 12:
			$s  = "Your request to join the <b>[".utf8entities($this->data['tag'])."]</b> alliance has been rejected.";
		break;
		case 13:
			$s  = "The request from player <b>".utf8entities($p['name'])."</b> to join the <b>[".utf8entities($this->data['tag'])."]</b> alliance has been rejected.";
		break;
		case 14:
			$s  = "<b>[".utf8entities($this->data['tag'])."]</b>'s leader has left the alliance. His successor, <b>".utf8entities($p['name'])."</b>,";
			$s .= " has taken control of the alliance.";
		break;
		case 15:
			$s  = "<b>[".utf8entities($this->data['tag'])."]</b>'s leader, <b>".utf8entities($p['name'])."</b>, has left the alliance. Since ";
			$s .= "<b>[".utf8entities($this->data['tag'])."]</b> no longer has a leader, all members can now vote to elect his successor.";
		break;
		case 16:
			$s  = "<b>".utf8entities($p['name'])."</b> is now running for <b>[".utf8entities($this->data['tag'])."]</b>'s presidency.";
		break;
		case 17:
			$s  = "<b>".utf8entities($p['name'])."</b> has cancelled his candidacy to <b>[".utf8entities($this->data['tag'])."]</b>'s presidency.";
		break;
		case 18:
			$s  = "You have taken presidency of the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
		break;
		case 19:
			$s  = "<b>".utf8entities($p['name'])."</b> has taken presidency of the <b>[".utf8entities($this->data['tag'])."]</b> alliance.";
		break;
		case 20:
			$s = "New technology trading orders have been issued!<br/>You should check the alliance's <a href='techtrade'>technology trading</a> page to find out how they affect you.";
		break;
		endswitch;
		return $s;
	}
}

?>

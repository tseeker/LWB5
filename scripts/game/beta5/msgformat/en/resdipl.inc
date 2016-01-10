<?php

class msgformat_resdipl {

	function msgformat_resdipl($game) {
		$this->game	= $game;
		$this->db	= $game->db;
		$this->players	= $game->getLib('beta5/player');
	}

	function getOffer() {
		$q = $this->db->query("SELECT * FROM research_assistance WHERE id=".$this->data['offer']);
		return dbFetchHash($q);
	}

	function getSender() {
		return "Science Minister";
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
		$o = $this->getOffer();
		$p1 = $this->players->call('get', $o['player'], true);
		$p2 = $this->players->call('get', $o['offer_to'], true);
		switch	($this->data['msg_id']) :
		case 0:
			$s = "Scientific assistance offer from " . utf8entities($p1['name']);
		break;
		case 1: case 2:
			$s = "[UPDATE] Scientific assistance to " . utf8entities($p2['name']);
		break;
		endswitch;
		return $s;
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$o = $this->getOffer();
		$p1 = $this->players->call('get', $o['player'], true);
		$p2 = $this->players->call('get', $o['offer_to'], true);
		switch	($this->data['msg_id']) :
		case 0:
			$s  = "We have received an offer of scientific assistance from <b>" . utf8entities($p1['name']) . "</b>.";
		break;
		case 1:
			$s = "Our offer to <b>" . utf8entities($p2['name']) . "</b> has been accepted";
			if ($o['price'] > 0) {
				$s .= " and you have therefore received <b>&euro;" . number_format($o['price']) . "</b>";
			}
			$s .= ".";
		break;
		case 2:
			$s  = "Our offer of scientific assistance to <b>" . utf8entities($p2['name']) . "</b> has been declined.";
		break;
		endswitch;
		$s .= " <a href='research?p=d'>More details ...</a>";
		return $s;
	}
}

?>

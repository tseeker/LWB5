<?php

class msgformat_flmove {
	var	$compn = array('G.A. ship', 'G.A. ships', 'fighter', 'fighters', 'cruiser', 'cruisers', 'batlle cruiser', 'battle cruisers');
	var	$compi = array('gaships', 'fighters', 'cruisers', 'bcruisers');
	var	$fmData = null;
	var	$fmId = null;

	function msgformat_flmove($game) {
		$this->game	= $game;
		$this->db	= $game->db;
		$this->players	= $game->getLib('beta5/player');
	}

	function getMovementData() {
		if (!is_null($this->fmData) && $this->fmId == $this->data['id']) {
			return;
		}

		$q = $this->db->query("SELECT * FROM flmove_data WHERE id={$this->data['id']}");
		$this->fmData = array();
		$isMe = true;
		while ($r = dbFetchHash($q)) {
			$isMe = $isMe && ($r['f_owner'] == $this->player);
			array_push($this->fmData, $r);
		}
		$this->isOwnArrival = $isMe;
		$this->fmId = $this->data['id'];
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
		$this->getMovementData();
		if ($this->isOwnArrival) {
			if (count($this->fmData) > 1) {
				$str = "Our fleets have";
			} else {
				$str = "Our fleet has";
			}
			$str .= " arrived at ";
		} else {
			$str .= "Fleet movement at ";
		}
		$str .= utf8entities($this->data['p_name']);
		return $str;
	}

	function getReplyLink() {
		return "";
	}

	function getComposition($r) {
		$rv = array();
		for ($i=0;$i<4;$i++) {
			$n = $r['f_' . $this->compi[$i]];
			if ($n == 0) {
				continue;
			}
			array_push($rv, "<b>" . number_format($n) . "</b> " . $this->compn[$i*2 + ($n>1 ? 1 : 0)]);
		}
		return join(', ', $rv);
	}

	function getContents() {
		$this->getMovementData();
		$pl = (count($this->fmData) > 1);
		if ($this->isOwnArrival) {
			$str = "Sir! The following fleet" . ($pl ? "s have" : " has") . " arrived at " . ($pl ? "their" : "its")
				. " destination, <a href='planet?id={$this->data['p_id']}'><b>" . utf8entities($this->data['p_name'])
				. "</b></a>:<br/>";
			foreach	($this->fmData as $r) {
				$str .= "<br/>Fleet <b>" . utf8entities($r['f_name']) . "</b> coming from <a href='planet?id={$r['from_id']}'><b>"
					. utf8entities($r['from_name']) . "</b></a> (composition: ";
				$str .= $this->getComposition($r);
				$str .= "; power: <b>" . number_format($r['f_power']) . "</b>)";
			}
		} else {
			$str = "Sir! The fleets stationed in orbit around <a href='planet?id={$this->data['p_id']}'><b>" . utf8entities($this->data['p_name'])
				. "</b></a> report the following event" . ($pl ? "s" : "") . ":<br/>";
			foreach	($this->fmData as $r) {
				$str .= "<br/>A " . ($r['hostile'] == 'f' ? "friendly" : "hostile") . " fleet, <b>" . utf8entities($r['f_name']) . "</b>, owned by "
					. "<a href='message?a=c&ct=0&id={$r['f_owner']}'><b>" . utf8entities($this->players->call('getName', $r['f_owner']))
					. "</b></a>, has ";
				if ($r['arrived'] == 'f') {
					$str .= "left orbit";
				} else {
					$str .= "entered orbit, coming from <a href='planet?id={$r['from_id']}'><b>" . utf8entities($r['from_name']) . "</b></a>";
				}
				$str .= " (composition: ";
				$str .= $this->getComposition($r);
				$str .= "; power: <b>" . number_format($r['f_power']) . "</b>)";
			}
		}
		return $str;
	}
}

?>

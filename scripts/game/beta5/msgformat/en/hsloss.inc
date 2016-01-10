<?php

class msgformat_hsloss {
	var	$compn = array('G.A. ship', 'G.A. ships', 'fighter', 'fighters', 'cruiser', 'cruisers', 'batlle cruiser', 'battle cruisers');
	var	$compi = array('gaships', 'fighters', 'cruisers', 'bcruisers');

	function msgformat_hsloss($game) {
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
		if (is_array($this->data[0])) {
			$data = $this->data[0];
		} else {
			$data = $this->data;
		}
		return "Ships lost in Hyperspace around " . utf8entities($data['p_name']);
	}

	function getReplyLink() {
		return "";
	}

	function getFleetComposition($r) {
		$rv = array();
		for ($i=0;$i<4;$i++) {
			$n = $r[$this->compi[$i]];
			if ($n == 0) {
				continue;
			}
			array_push($rv, "<b>" . number_format($n) . "</b> " . $this->compn[$i*2 + ($n>1 ? 1 : 0)]);
		}
		return join(', ', $rv);
	}

	function getContents() {
		if (is_array($this->data[0])) {
			$data = $this->data;
		} else {
			$data = array($this->data);
		}

		$str = "Sir! Some of our ships standing by in Hyperspace in the vicinity of <a href='"
			. "planet?id={$data['p_id']}'><b>" . utf8entities($data['p_name'])
			. "</b></a> were lost; a ceremony will be held today in their honor while these "
			. "poor guys drift in Hyperspace until they starve to death.<br/><br/>"
			. "The following report indicates the amount of ships lost.<br/>";

		foreach	($data as $f) {
			$str .= "<br/>Fleet <b>" . utf8entities($f['f_name']) . "</b> lost "
				. $this->getFleetComposition($f) . " (lost fleet power: <b>"
				. number_format($f['power']) . "</b>)";
		}
		return $str;
	}
}

?>

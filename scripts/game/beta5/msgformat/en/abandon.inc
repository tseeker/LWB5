<?php

class msgformat_abandon {

	function msgformat_abandon($game) {
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
		$name = $this->players->call('getName', $this->player);
		return utf8entities($name);
	}

	function getRLink() {
		return "";
	}

	function getSubject() {
		if (is_array($this->data[0])) {
			$c = count($this->data);
		} else {
			$c = 1;
		}
		return "$c planet" . ($c>1?"s":"") . " abandoned";
	}

	function getReplyLink() {
		return	"";
	}

	function getContents() {
		if (is_array($this->data[0])) {
			$data = $this->data;
		} else {
			$data = array($this->data);
		}
		$c = count($data);

		$str = "Sir! Our forces have completed the evacuation of planet" . ($c>1?"s":"") . " ";

		for ($i=0;$i<$c;$i++) {
			if ($i > 0) {
				if ($i == $c - 1) {
					$str .= ' and ';
				} else {
					$str .= ', ';
				}
			}
			$str .= "<a href='planet?id={$data[$i]['p_id']}'><b>"
				. utf8entities($data[$i]['p_name']) . "</b></a>";
		}

		$str .= ". The citizens of th" . ($c>1?"ese":"is") . " world" . ($c>1?"s":"")
			. " are left to fend for themselves.";

		return $str;
	}
}

?>

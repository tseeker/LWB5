<?php

class msgformat_sale {

	function msgformat_sale($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return "Economic Advisor";
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
		$v = ($this->data['is_sale'] == 'f') ? "bought" : "sold";
		if (is_null($this->data['p_name'])) {
			return "Fleet '" . utf8entities($this->data['f_name']) . " has been $v";
		}
		return "Planet " . utf8entities($this->data['p_name']) . " has been $v";
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$p = $this->players->call('get', $this->data['pl_id'], true);
		$sale = ($this->data['is_sale'] == 'f');
		$pq = ($p['quit'] == 0);
		$fp = ($this->data['f_power'] != "") ? number_format($this->data['f_power']) : "";

		if (is_null($this->data['p_name'])) {
			if ($this->data['is_sale'] == 't') {
				$s = "We just lost control of our fleet <b>" . utf8entities($this->data['f_name'])
					. "</b> (fleet power: <b>$fp</b>).<br/>It has been sold to "
					. ($pq ? "<a href='message?a=c&ct=0&id={$this->data['pl_id']}'>" : "")
					. "<b>" . utf8entities($this->data['pl_name']) . "</b>" . ($pq?"</a>":"") . ".";
			} else {
				$s = "We just gained control of the fleet <b>" . utf8entities($this->data['f_name'])
					. "</b> (fleet power: <b>$fp</b>).<br/>It has been obtained from "
					. ($pq ? "<a href='message?a=c&ct=0&id={$this->data['pl_id']}'>" : "")
					. "<b>" . utf8entities($this->data['pl_name']) . "</b>" . ($pq?"</a>":"") . ".";
			}
		} elseif ($this->data['is_sale'] == 't') {
			$s = "We just lost control of our planet <a href='planet?id="
				. $this->data['p_id'] . "'><b>" . utf8entities($this->data['p_name'])
				. "</b></a>";
			if (!is_null($this->data['f_name'])) {
				$s .= " along with the <b>" . utf8entities($this->data['f_name'])
					. "</b> fleet (power: <b>$fp</b>)";
			}

			$s .= ".<br/>It has been sold to "
				. ($pq ? "<a href='message?a=c&ct=0&id={$this->data['pl_id']}'>" : "")
				. "<b>" . utf8entities($this->data['pl_name']) . "</b>" . ($pq?"</a>":"") . ".";
		} else {
			$s = "We just gained control on planet <a href='planet?id="
				. $this->data['p_id'] . "'><b>" . utf8entities($this->data['p_name'])
				. "</b></a>";
			if (!is_null($this->data['f_name'])) {
				$s .= " and on the <b>" . utf8entities($this->data['f_name'])
					. "</b> fleet (power: <b>$fp</b>)";
			}

			$s .= ".<br/>We bought it from "
				. ($pq ? "<a href='message?a=c&ct=0&id={$this->data['pl_id']}'>" : "")
				. "<b>" . utf8entities($this->data['pl_name']) . "</b>" . ($pq?"</a>":"") . ".";
		}
		return $s;
	}
}

?>

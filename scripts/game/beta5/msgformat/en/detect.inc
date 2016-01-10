<?php

class msgformat_detect {

	private $contents = array();

	public function __construct($game) {
		$this->game	= $game;
		$this->db	= $game->db;
		$this->players	= $game->getLib('beta5/player');
		$this->forums	= $game->getLib('main/forums');
	}

	public function getRecipient() {
		return utf8entities($this->players->call('getName', $this->player),ENT_COMPAT);
	}
	public function getSender() {
		return ($this->data['is_owner'] == 't' ? 'Military Advisor' : ('Governor of ' . $this->data['p_name']));
	}

	public function getSLink() { return ""; }
	public function getRLink() { return ""; }
	public function getReplyLink() { return ""; }

	public function getSubject() {
		if ($this->data['is_owner'] == 't') {
			return "Fleet detected at {$this->data['p_name']}";
		}
		return "Fleet detected in hyperspace";
	}

	public function getContents() {
		$func = ($this->data['is_owner'] == 't') ? "ownFleet" : "otherFleet";

		ob_start();
		$this->$func();
		$x = ob_get_contents();
		ob_end_clean();

		return $x;
	}

	private function ownFleet() {
?>
One of our fleets in Hyperspace stand-by around planet <b><?=utf8entities($this->data['p_name'])?></b>
has been detected by the planet's hyperspace beacon.<br/>
<br/>
<?
		switch ($this->data['i_level']) :
		case 0:
			echo "The beacon didn't manage to gather any information regarding the fleet, tho.";
			break;
		case 1:
			echo "The beacon managed to obtain a very rough estimate of our fleet's size.";
			break;
		case 2:
			echo "The beacon managed to obtain a rough estimate of our fleet's size.";
			break;
		case 3:
			echo "The beacon managed to obtain our fleet's size.";
			break;
		case 4:
			echo "The beacon identified our fleet!";
			break;
		endswitch;
	}

	private function otherFleet() {
?>
Our hyperspace beacon around planet <b><?=utf8entities($this->data['p_name'])?></b> has detected a fleet
in Hyperspace.<br/>
<br/>
<?
		switch ($this->data['i_level']) :
		case 0:
			echo "However, no additional information could be gathered due to heavy jamming.";
			break;
		case 1:
			echo "Jamming prevented us from obtaining much information, however the beacon "
				. "transmitted a very rough estimate of the fleet's size. Its power is "
				. "estimated at <b>" . number_format($this->data['fl_size'])
				. "</b> (+/- 25%).";
			break;
		case 2:
			echo "Jamming prevented us from obtaining much information, however the beacon "
				. "transmitted a rough estimate of the fleet's size. Its power is "
				. "estimated at <b>" . number_format($this->data['fl_size'])
				. "</b> (+/- 7.5%).";
			break;
		case 3:
			echo "The beacon was able to compute the fleet's size; its power is <b>"
				. number_format($this->data['fl_size']) . "</b>. However, because of jamming, "
				. "the beacon could not identify the fleet.";
			break;
		case 4:
			echo "The beacon completely identified the fleet! Its power is <b>"
				. number_format($this->data['fl_size']) . "</b>, and it is owned by <b>"
				. utf8entities($this->data['flo_name']) . "</b>.";
			break;
		endswitch;
	}
}

?>

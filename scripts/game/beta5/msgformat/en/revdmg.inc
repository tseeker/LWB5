<?php

class msgformat_revdmg {

	function msgformat_revdmg($game) {
		$this->game	= $game;
		$this->players	= $game->getLib('beta5/player');
	}

	function getSender() {
		return 'Damage Control Squad';
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
		return "Damage to infrastructure due to riots";
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		if (is_array($this->data[0])) {
			$data = $this->data;
		} else {
			$data = array($this->data);
		}

		$str = "Sir! Citizens in the empire are riotting; we have been able to contain most of the acts of "
			. "violence. Here is our report.<br/><br/>";

		$n = 0;
		foreach	($data as $p) {
			$str .= 'Planet <a href="planet?id='.$p['planet'].'"><b>'.utf8entities($p['pname'])."</b></a>: ";
			if ($p['mfact'] + $p['ifact'] + $p['turrets'] == 0) {
				$str .= "no damage reported.";
			} else {
				$nd = 0;
				if ($p['ifact'] != 0) {
					$str .= "<b>" . $p['ifact'] . "</b> industrial factor";
					if ($p['ifact'] > 1) {
						$str .= "ies";
					} else {
						$str .= "y";
					}
					$nd += $p['ifact'];
				}
				if ($p['mfact'] != 0) {
					if ($nd) {
						if ($p['turrets'] > 0) {
							$str .= ", ";
						} else {
							$str .= " and ";
						}
					}
					$str .= "<b>" . $p['mfact'] . "</b> military factor";
					if ($p['mfact'] > 1) {
						$str .= "ies";
					} else {
						$str .= "y";
					}
					$nd += $p['mfact'];
				}
				if ($p['turrets'] != 0) {
					if ($nd) {
						$str .= " and ";
					}
					$str .= "<b>" . $p['turrets'] . "</b> turret" . ($p['turrets'] > 1 ? "s" : "");
				}
				$str .= " ha" . ($nd > 1 ? "ve" : "s") . " been destroyed.";
			}

			$n ++;
			if ($n < count($data)) {
				$str .= "<br/>";
			}
		}
		return $str;
	}
}

?>

<?php

class msgformat_battle {
	var $sfn = array('gaships','fighters','cruisers','bcruisers');
	var $sdn = array('GA Ships','Fighters','Cruisers','Battle Cruisers');

	function msgformat_battle($game) {
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
		return "Battle report for " . utf8entities($this->data['planet']);
	}

	function getReplyLink() {
		return "";
	}

	function getContents() {
		$vacation = ($this->data['o_power'] == 0);

		$str  = 'Sir! ' . ($vacation ? "Forces " : "Our forces ")
			. 'have been fighting in orbit around <a href="planet?id='.$this->data['planet_id'].'"><b>'
		      . utf8entities($this->data['planet']) . "</b></a>. "
		      . ($this->data['heroic_def'] == -1 ? 'The defending forces, overwhelmed, resisted to the last man. Their heroic actions have caused us unexpected losses. ' : '')
		      . ($this->data['heroic_def'] == 1 ? 'While our forces were greatly outnumbered, they opposed the attacking forces heroically. ' : '')
		      . "We have received an update on the latest casualties:</p>"
		      . "<table class='breport'><tr><th class='sname' rowspan='2'>&nbsp;</th><th colspan='2' class='fown'>Own Troops</th>";
		if ($this->data['a_power'] > 0) {
			$str .= "<th colspan='2' class='fally'>Allied Troops</th>";
		}
		$str .= "<th colspan='2' class='fenemy'>Enemy Troops</th></tr><tr><th>Start</th><th>Lost</th><th>Start</th><th>Lost</th>";
		if ($this->data['a_power'] > 0) {
			$str .= "<th>Start</th><th>Lost</th>";
		}
		$str .= "</th>";

		for ($i=0;$i<4;$i++) {
			$n = $this->sfn[$i];
			if ($this->data["o_$n"] + $this->data["a_$n"] + $this->data["e_$n"] == 0) {
				continue;
			}
			$str .= "<tr><th class='sname'>" . $this->sdn[$i] . "</th><td>" . number_format($this->data["o_$n"])
				. "</td><td>" . number_format($this->data["ol_$n"]) . "</td>";
			if ($this->data['a_power'] > 0)
				$str .= "<td>" . number_format($this->data["a_$n"]) . "</td><td>" . number_format($this->data["al_$n"]) . "</td>";
			$str .= "<td>" . number_format($this->data["e_$n"]) . "</td><td>" . number_format($this->data["el_$n"]) . "</td>";
			$str .= "</tr>";
		}

		$tmode = $this->data['tmode'];
		if ($tmode != 0) {
			$str .= "<tr><th class='sname'>Turrets</th><td>";
			if ($tmode == 1)
				$str .= number_format($this->data["turrets"]) . "</td><td>" . number_format($this->data["l_turrets"]);
			else
				$str .= "-</td><td>-";

			$str .= "</td><td>";
			if ($tmode == 2)
				$str .= number_format($this->data["turrets"]) . "</td><td>" . number_format($this->data["l_turrets"]) . "</td><td>";
			elseif ($this->data['a_power'] > 0)
				$str .= "-</td><td>-</td><td>";

			if ($tmode == 3)
				$str .= number_format($this->data["turrets"]) . "</td><td>" . number_format($this->data["l_turrets"]);
			else
				$str .= "-</td><td>-";
			$str .= "</td></tr>";
		}

		$str .= "<tr><th class='sname'>Fleet Power</th><td>" . number_format($this->data['o_power']) . "</td><td>" . number_format($this->data['ol_power']) . "</td>";
		if ($this->data['a_power'] > 0)
			$str .= "<td>" . number_format($this->data['a_power']) . "</td><td>" . number_format($this->data['al_power']) . "</td>";
		$str .= "<td>" . number_format($this->data['e_power']) . "</td><td>" . number_format($this->data['el_power']) . "</td></tr></table><p>";

		$ePower = $this->data['e_power']; $eLoss = $this->data['el_power'];
		$aPower = $this->data['a_power'] + $this->data['o_power'];
		$aLoss = $this->data['al_power'] + $this->data['ol_power'];

		if ($aPower == $aLoss && $ePower == $eLoss) {
			$str .= "The fleets annihilated each other.";
		} elseif ($ePower == $eLoss) {
			$str .= "Victory is ours! The enemy has been annihilated!";
		} elseif ($aPower == $aLoss) {
			$str .= "Our forces were annihilated!";
		} else {
			$ePLoss = $eLoss / $ePower;
			$aPLoss = $aLoss / $aPower;
			if ($ePower - $eLoss > $aPower - $aLoss) {
				$ratio = $ePLoss ? ($aPLoss / $ePLoss) : 11;
				if ($ratio > 10) {
					$str .= "Sir, our forces <i>have</i> to retreat from this losing battle!";
				} elseif ($ratio > 3) {
					$str .= "We are getting whacked. We need reinforcements!";
				} elseif ($ratio > 1.2) {
					$str .= "The situation doesn't look too good for us.";
				} else {
					$str .= "Our forces and the enemy's forces are a close match.";
				}
			} elseif ($ePower - $eLoss < $aPower - $aLoss) {
				$ratio = $aPLoss ? ($ePLoss / $aPLoss) : 11;
				if ($ratio > 10) {
					$str .= "The enemy will soon be crushed.";
				} else if ($ratio > 3) {
					$str .= "We are clearly winning this fight.";
				} else if ($ratio > 1.2) {
					$str .= "The enemy is bound to lose this fight in the long run.";
				} else {
					$str .= "Our forces and the enemy's forces are a close match.";
				}
			} else {
				$str .= "Our forces and the enemy's forces are a close match.";
			}
		}

		return $str;
	}
}

?>

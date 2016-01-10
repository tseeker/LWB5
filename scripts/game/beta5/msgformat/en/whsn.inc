<?php

class msgformat_whsn {
	var	$compn = array('G.A. ship', 'G.A. ships', 'fighter', 'fighters', 'cruiser', 'cruisers', 'batlle cruiser', 'battle cruisers');
	var	$compi = array('gaships', 'fighters', 'cruisers', 'bcruisers');

	function msgformat_whsn($game) {
		$this->game	= $game;
		$this->db	= $game->getDBAccess();
		$this->players	= $game->getLib('beta5/player');
		$this->planets	= $game->getLib('beta5/planet');
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
		return "Planet " . utf8entities($this->data['p_name']) . " has been destroyed!";
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
		$q = $this->db->query("SELECT * FROM whsn_fleet WHERE id={$this->data['id']}");
		$fList = array();
		$fPower = 0;
		while ($r = dbFetchHash($q)) {
			$fPower += $r['power'];
			array_push($fList, $r);
		}

		if ($this->data['was_owner'] == 'f') {
			$str = "Sir! A planet we were orbitting, <b>" . utf8entities($this->data['p_name'])
				. "</b>, has self-destructed using a wormhole supernova!<br/>";

			$ffp = $this->data['f_power']; $efp = $this->data['e_power'];
			if ($ffp != 0) {
				$str .= "Allied forces lost a total of <b>" . number_format($ffp)
					. "</b> fleet power";
				if ($efp != 0) {
					$str .= ", while enemy forces lost a total of <b>"
						. number_format($efp) . "</b> fleet power";
				}
				$str .= ".";
			} else if ($efp != 0) {
				$str .= "A total fleet power of <b>" . number_format($efp)
					. "</b> was lost by the enemy when the planet exploded.";
			}
			if (count($fList)) {
				$str .= " We lost the following fleet" . ((count($fList)>1) ? "s" : "")
					. " (representing a total fleet power of <b>" . number_format($fPower)
					. "</b>):<br/><br/>";
			}
		} else {
			$str = "Sir! Our planet <b>" . utf8entities($this->data['p_name'])
				. "</b>, has successfully self-destructed using a wormhole supernova!<br/>";

			$ffp = $this->data['f_power']; $efp = $this->data['e_power'];
			if ($ffp != 0) {
				$str .= "Allied forces lost a total of <b>" . number_format($ffp)
					. "</b> fleet power";
				if ($efp != 0) {
					$str .= ", while enemy forces lost a total of <b>" . number_format($efp) . "</b> fleet power";
				}
				$str .= ".";
			} else if ($efp != 0) {
				$str .= "A total fleet power of <b>" . number_format($efp) . "</b> was lost by the enemy when the planet exploded.";
			}
			if (count($fList)) {
				$str .= " We lost the following fleet" . ((count($fList)>1) ? "s" : "")
					. " (representing a total fleet power of <b>" . number_format($fPower)
					. "</b>) during the event:<br/><br/>";
			}
		}

		foreach ($fList as $f) {
			$str .= "Fleet <b>" . utf8entities($f['name']) . "</b>: "
				. $this->getFleetComposition($f) . "; fleet power: <b>"
				. number_format($f['power']) . "</b><br/>";
		}

		$pinfo = $this->planets->call('byId', $this->data['p_id']);
		$str .= "<br/>The remains of the planet have been named <a href='planet?id={$this->data['p_id']}'><b>"
			. utf8entities($pinfo['name']) . "</b></a>.";

		return $str;
	}
}

?>

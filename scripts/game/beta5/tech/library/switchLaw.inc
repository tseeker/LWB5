<?php

class beta5_tech_switchLaw {

	function beta5_tech_switchLaw($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->rules	= $this->lib->game->getLib('beta5/rules');
	}


	// Enacts or revoke a law
	function run($pl, $id) {
		$q = $this->db->query(
			"SELECT p.in_effect, r.cost FROM research_player p, research r "
			. "WHERE p.player = $pl AND p.research = $id AND r.id = $id "
			. "AND p.points = r.points AND r.is_law AND p.in_effect IN (0,1)"
		);
		if (!($q && dbCount($q) == 1)) {
			return false;
		}

		list($ie, $cost) = dbFetchArray($q);
		$q = $this->db->query("SELECT cash FROM player WHERE id = $pl");
		list($cash) = dbFetchArray($q);
		if ($cash < $cost) {
			return false;
		}

		$this->db->query("UPDATE player SET cash = cash - $cost WHERE id = $pl");
		$this->db->query("UPDATE research_player SET in_effect = ".($ie==0?"6":"-5")." WHERE player = $pl AND research = $id");

		// Change the player's rules
		$q = $this->db->query("SELECT rule,modifier FROM research_effect WHERE research = $id");
		$rules = array();
		while ($r = dbFetchArray($q)) {
			$rules[$r[0]] = ($ie ? -1 : 1) * $r[1];
		}
		$this->rules->call('change', $pl, $rules);

		return true;
	}
}

?>

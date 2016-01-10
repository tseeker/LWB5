<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/tech/library/getAvailableTechs.inc
//
// This function checks whether a player's tech trading requests are ok.
// If they are not, it either deletes or re-arranges them.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_tech_getAvailableTechs {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($player) {
		// Get all techs the player has implemented
		$q = $this->db->query(
			"SELECT r.id FROM research r, research_player p "
				. "WHERE r.id = p.research AND p.points = r.points AND p.player = $player "
				  . "AND (p.in_effect <> 0 OR r.is_law)"
		);
		$gotTechs = array();
		while ($r = dbFetchArray($q)) {
			array_push($gotTechs, $r[0]);
		}

		// If we don't have techs, return
		if (empty($gotTechs)) {
			return $gotTechs;
		}

		// Get all techs the player sees
		$q = $this->db->query(
			"SELECT r.id FROM research r, research_player p "
				. "WHERE r.id = p.research AND p.points >= 75 * r.points / 100 AND p.player = $player"
		);
		$seeTechs = array();
		while ($r = dbFetchArray($q)) {
			array_push($seeTechs, $r[0]);
		}

		// Get the amount of dependencies for each tech not in the list
		$q = $this->db->query(
			"SELECT research,COUNT(*) FROM research_dep "
				. "WHERE research NOT IN (" . join(',',$seeTechs) . ") "
				. "GROUP BY research"
		);
		if (!dbCount($q)) {
			return array();
		}
		$depCount = array();
		while ($r = dbFetchArray($q)) {
			$depCount[$r[0]] = $r[1];
		}

		// Now get all techs that depend on one of these techs
		$q = $this->db->query(
			"SELECT research,COUNT(*) FROM research_dep "
				. "WHERE research NOT IN (" . join(',',$seeTechs) . ") "
				  . "AND depends_on IN (" . join(',',$gotTechs) . ")"
				. "GROUP BY research"
		);
		$okTechs = array();
		while ($r = dbFetchArray($q)) {
			if ($depCount[$r[0]] == $r[1]) {
				array_push($okTechs, $r[0]);
			}
		}

		return $okTechs;
	}

}

?>

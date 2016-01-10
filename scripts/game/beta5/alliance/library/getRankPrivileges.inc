<?php

class beta5_alliance_getRankPrivileges {

	function beta5_alliance_getRankPrivileges($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns the privileges associated with a rank
	function run($id) {
		$q = $this->db->query("SELECT * FROM alliance_grade WHERE id=$id");
		$pr = dbFetchHash($q);
		$pr['is_leader'] = 0;

		// Transform list access
		switch	($pr['list_access']) :
			case 'NO ': $pr['list_access'] = 0; break;
			case 'PY ': $pr['list_access'] = 1; break;
			case 'PL ': $pr['list_access'] = 2; break;
			case 'PLD': $pr['list_access'] = 3; break;
		endswitch;

		// Transform tech trade tool access
		$q = $this->db->query("SELECT enable_tt FROM alliance WHERE id = {$pr['alliance']}");
		list($enableTT) = dbFetchArray($q);
		switch ($pr['tech_trade']) :
			case 'NO': $pr['tech_trade'] = 0; break;
			case 'SL': $pr['tech_trade'] = ($enableTT != 'N' ? 1 : 0); break;
			case 'SR': $pr['tech_trade'] = ($enableTT == 'N' ? 0 : ($enableTT == 'S' ? 1 : 2)); break;
			case 'VL': $pr['tech_trade'] = ($enableTT != 'N' ? 3 : 0); break;
			case 'MR': $pr['tech_trade'] = ($enableTT != 'N' ? 4 : 0); break;
		endswitch;

		// Transform PostgreSQL booleans into something usable by the current scripts
		$fields = array("attacks", "can_set_grades", "can_kick", "can_accept",
			"can_be_kicked", "forum_admin", "dipl_contact", "can_vote", "can_be_cand");
		foreach ($fields as $f) {
			$pr[$f] = ($pr[$f] == 't') ? 1 : 0;
		}

		// Get kickable ranks
		$a = array();
		if ($pr['can_kick']) {
			$q = $this->db->query("SELECT kick FROM algr_kick WHERE grade = $id AND kick <> $id");
			if (!dbCount($q)) {
				$q = $this->db->query(
					"SELECT id FROM alliance_grade "
						. "WHERE alliance = {$pr['alliance']} AND id <> $id"
					);
			}
			while ($r = dbFetchArray($q)) {
				array_push($a, $r[0]);
			}
		}
		$pr['kick_ranks'] = $a;

		// Get changeable ranks
		$a = array();
		if ($pr['can_set_grades']) {
			$q = $this->db->query("SELECT can_change FROM algr_chgrade WHERE grade=$id AND can_change!=$id");
			if (!dbCount($q)) {
				$q = $this->db->query(
					"SELECT id FROM alliance_grade "
						. "WHERE alliance={$pr['alliance']} AND id <> $id"
				);
			}
			while ($r = dbFetchArray($q)) {
				array_push($a, $r[0]);
			}
		}
		$pr['change_ranks'] = $a;

		// Forum privileges
		$pr['f_read'] = $pr['f_mod'] = array();
		if ($pr['forum_admin'] == 0) {
			$q = $this->db->query("SELECT forum,is_mod FROM algr_forums WHERE grade=$id");
			while ($r = dbFetchArray($q)) {
				array_push($pr[($r[1] == 't') ? "f_mod" : "f_read"], $r[0]);
			}
		} else {
			$q = $this->db->query(
				"SELECT f.id FROM af_forum f,alliance_grade r "
					. "WHERE r.id = $id AND f.alliance = r.alliance"
			);
			while ($r = dbFetchArray($q)) {
				array_push($pr['f_mod'], $r[0]);
			}
		}

		return $pr;
	}
}

?>

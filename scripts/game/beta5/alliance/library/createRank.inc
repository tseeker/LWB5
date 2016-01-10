<?php

class beta5_alliance_createRank {

	function beta5_alliance_createRank($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Creates a new rank within the alliance
	function run($aid, $name, $privs, $kick, $change, $fread, $fmod) {
		$n = addslashes($name);
		$ll = array('NO', 'PY', 'PL', 'PLD');
		$lt = array('NO', 'SL', 'SR', 'VL', 'MR');
		$this->db->query(
			"INSERT INTO alliance_grade(alliance,name,list_access,attacks,can_set_grades,"
				. "can_kick,can_accept,forum_admin,dipl_contact,can_vote,can_be_cand,tech_trade)"
			. " VALUES($aid,'$n','".$ll[$privs['list_access']]."',".dbBool($privs['attacks'])
			. ",".dbBool($privs['can_set_grades']).",".dbBool($privs['can_kick']).","
			. dbBool($privs['can_accept']).','.dbBool($privs['forum_admin']).","
			. dbBool($privs['dipl_contact']).",".dbBool($privs['can_vote']).","
			. dbBool($privs['can_be_cand']).",'".$lt[$privs['tech_trade']] . "')"
		);
		$q = $this->db->query("SELECT id FROM alliance_grade WHERE alliance=$aid AND name='$n'");
		if (!($q && dbCount($q))) {
			return;
		}
		list($rid) = dbFetchArray($q);

		foreach	($kick as $id) {
			$this->db->query("INSERT INTO algr_kick VALUES($rid,$id)");
		}
		foreach	($change as $id) {
			$this->db->query("INSERT INTO algr_chgrade VALUES($rid,$id)");
		}

		foreach	($fread as $id) {
			$this->db->query("INSERT INTO algr_forums VALUES($rid,$id,FALSE)");
		}
		foreach	($fmod as $id) {
			$this->db->query("INSERT INTO algr_forums VALUES($rid,$id,TRUE)");
		}
	}
}

?>

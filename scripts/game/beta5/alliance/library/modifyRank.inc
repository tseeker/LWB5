<?php

class beta5_alliance_modifyRank {

	function beta5_alliance_modifyRank($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Modifies a new rank within the alliance
	function run($rid, $name, $privs, $kick, $change, $fread, $fmod) {
		$ll = array('NO ','PY ','PL ','PLD');
		$lt = array('NO', 'SL', 'SR', 'VL', 'MR');
		if (is_null($name)) {
			$n = "NULL";
		} else {
			$n = "'".addslashes($name)."'";
		}
		$qs = "UPDATE alliance_grade SET name=$n";
		foreach	($privs as $p => $v) {
			$qs .= ",$p=";
			if ($p == "list_access") {
				$qs .= "'" . $ll[$v] . "'";
			} elseif ($p == "tech_trade") {
				$qs .= "'" . $lt[$v] . "'";
			} else {
				$qs .= dbBool($v);
			}
		}
		$qs .= " WHERE id=$rid";
		$this->db->query($qs);

		$q = $this->db->query("SELECT alliance FROM alliance_grade WHERE id=$rid");
		list($aid) = dbFetchArray($q);
		$q = $this->db->query("SELECT default_grade,leader FROM alliance WHERE id=$aid");
		list($did, $lid) = dbFetchArray($q);
		if ($rid == $did) {
			$n = " IS NULL";
		} else {
			$n = "=$rid";
		}

		if ($privs['can_vote'] == 0) {
			$this->db->query("UPDATE player SET a_vote=NULL WHERE a_grade$n AND alliance=$aid AND a_status='IN'" . (is_null($lid) ? "": " AND id<>$lid"));
		}
		if ($privs['can_be_cand'] == 0) {
			$q = $this->db->query("SELECT id FROM player WHERE a_grade$n AND alliance=$aid AND a_status='IN'"  . (is_null($lid) ? "": " AND id<>$lid"));
			$a = array();
			while ($r = dbFetchArray($q)) {
				array_push($a, $r[0]);
			}
			if (count($a)) {
				$this->db->query("DELETE FROM alliance_candidate WHERE candidate IN (".join(',',$a).")");
			}
		}

		$this->db->query("DELETE FROM algr_kick WHERE grade=$rid");
		foreach	($kick as $id) {
			$this->db->query("INSERT INTO algr_kick VALUES($rid,$id)");
		}
		$this->db->query("DELETE FROM algr_chgrade WHERE grade=$rid");
		foreach	($change as $id) {
			$this->db->query("INSERT INTO algr_chgrade VALUES($rid,$id)");
		}

		$this->db->query("DELETE FROM algr_forums WHERE grade=$rid");
		foreach	($fread as $id) {
			$this->db->query("INSERT INTO algr_forums VALUES($rid,$id,".dbBool(0).")");
		}
		foreach	($fmod as $id) {
			$this->db->query("INSERT INTO algr_forums VALUES($rid,$id,".dbBool(1).")");
		}
	}
}

?>

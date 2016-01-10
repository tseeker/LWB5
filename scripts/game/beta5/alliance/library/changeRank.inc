<?php

class beta5_alliance_changeRank {

	function beta5_alliance_changeRank($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	// Change an alliance member's rank
	function run($pid, $rid) {
		$this->db->query("UPDATE player SET a_grade=$rid WHERE alliance IS NOT NULL AND a_status='IN' AND id=$pid");
		$q = $this->db->query(
			"SELECT r.can_vote,r.can_be_cand,a.leader FROM player p,alliance a,alliance_grade r "
			. "WHERE p.id=$pid AND r.alliance=p.alliance AND a.id=p.alliance AND "
			. "((p.a_grade IS NOT NULL AND p.a_grade=r.id) OR (p.a_grade IS NULL AND r.id=a.default_grade))"
		);
		list($cv,$cc,$lid) = dbFetchArray($q);
		$lockedAlliances = (int) $this->game->params['lockalliances'];
		$lockedAlliances = ($lockedAlliances > 1) ? 1 : 0;
		if ($pid != $lid && ! $lockedAlliances) {
			if ($cv == 0) {
				$this->db->query("UPDATE player SET a_vote=NULL WHERE id=$pid");
			}
			if ($cc == 0) {
				$this->db->query("DELETE FROM alliance_candidate WHERE candidate=$pid");
			}
		}
		// FIXME: message
	}
}

?>

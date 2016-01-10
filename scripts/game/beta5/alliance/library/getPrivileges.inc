<?php

class beta5_alliance_getPrivileges {

	function beta5_alliance_getPrivileges($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Returns the list of privileges a player has inside an alliance
	function run($p) {
		$nr = array(
			'list_access'		=> 0,
			'tech_trade'		=> 0,
			'attacks'		=> 0,
			'can_set_grades'	=> 0,
			'can_kick'		=> 0,
			'can_accept'		=> 0,
			'forum_admin'		=> 0,
			'dipl_contact'		=> 0,
			'can_vote'		=> 0,
			'can_be_cand'		=> 0,
			'is_leader'		=> 0
		);

		$pi = $this->players->call('get', $p);
		if (is_null($pi['aid'])) {
			return $nr;
		}

		$a = $this->lib->call('get', $pi['aid']);
		if (is_null($a)) {
			return $nr;
		}

		if ($a['leader'] == $p) {
			$pr = array(
				'list_access'		=> 3,
				'tech_trade'		=> ($a['enable_tt'] == 'N' ? 0 : 4),
				'attacks'		=> 1,
				'can_set_grades'	=> 1,
				'can_kick'		=> 1,
				'can_accept'		=> 1,
				'forum_admin'		=> 1,
				'dipl_contact'		=> 1,
				'can_vote'		=> 0,
				'can_be_cand'		=> 1,
				'is_leader'		=> 1
			);

			// Get all ranks (-> kick, change)
			$q = $this->db->query("SELECT id FROM alliance_grade WHERE alliance=".$a['id']);
			$ar = array();
			while ($r = dbFetchArray($q)) {
				array_push($ar, $r[0]);
			}
			$pr['kick_ranks'] = $pr['change_ranks'] = $ar;

			// Forums
			$pr['f_read'] = $pr['f_mod'] = array();
			$q = $this->db->query("SELECT id FROM af_forum WHERE alliance=".$a['id']);
			while ($r = dbFetchArray($q)) {
				array_push($pr['f_mod'], $r[0]);
			}
		} elseif (is_null($pi['a_grade'])) {
			$pr = $this->lib->call('getRankPrivileges', $a['default_grade']);
		} else {
			$pr = $this->lib->call('getRankPrivileges', $pi['a_grade']);
		}
		if ($a['democracy'] == "f") {
			$pr['can_vote'] = $pr['can_be_cand'] = 'f';
		}
		return $pr;
	}
}

?>

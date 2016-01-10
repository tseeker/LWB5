<?php

class beta5_alliance_rejectRequest {

	function beta5_alliance_rejectRequest($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->msgs	= $this->lib->game->getLib('beta5/msg');
	}


	// Rejects a request to join an alliance
	function run($aid, $pid, $kid) {
		$q = $this->db->query("SELECT id FROM player WHERE alliance=$aid AND a_status='REQ' AND id=$pid");
		if (!($q && dbCount($q) == 1)) {
			return;
		}

		$a = $this->lib->call('get', $aid);
		if (is_null($a)) {
			return;
		}

		$tag = addslashes($a['tag']);
		$tm = time();

		$this->msgs->call('send', $pid, 'alint', array(
			"msg_type"	=> 12,
			"alliance"	=> $aid,
			"tag"		=> $tag,
			"player"	=> $pid,
		));

		$l = $this->lib->call('getKeepers', $aid);
		foreach	($l as $id) {
			if ($id == $kid) {
				continue;
			}
			$this->msgs->call('send', $id, 'alint', array(
				"msg_type"	=> 13,
				"alliance"	=> $aid,
				"tag"		=> $tag,
				"player"	=> $pid,
			));
		}

		$this->db->query("UPDATE player SET a_status=NULL,alliance=NULL,a_vote=NULL,a_grade=NULL WHERE id=$pid");
	}
}

?>

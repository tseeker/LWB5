<?php

class beta5_alliance_addCandidate {

	function beta5_alliance_addCandidate($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Adds a new candidate for an alliance's presidency
	function run($aid,$pid) {
		$this->db->query("INSERT INTO alliance_candidate(alliance,candidate) VALUES($aid,$pid)");
		$q = $this->db->query("SELECT id FROM alliance_candidate WHERE candidate=$pid");
		list($id) = dbFetchArray($q);
		$this->db->query("UPDATE player SET a_vote=$id WHERE id=$pid");

		$a = $this->lib->call('get', $aid);
		$tag = addslashes($a['tag']);
		$tm = time();

		$vl = $this->lib->call('getVoters', $aid);
		foreach	($vl as $id) {
			if ($id == $pid) {
				continue;
			}
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',$pid,16)");
		}
	}
}

?>

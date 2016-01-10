<?php

class beta5_alliance_takePresidency {

	function beta5_alliance_takePresidency($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Makes a player the new president for an alliance
	function run($aid, $pid) {
		$this->db->query("UPDATE alliance SET leader=$pid,successor=NULL WHERE id=$aid");

		$a = $this->lib->call('get', $aid);
		$tag = addslashes($a['tag']);
		$tm = time();

		$l = array_keys($this->lib->call('getMembers', $aid));
		foreach	($l as $id) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			if ($id == $pid) {
				$mt = 18;
			} else {
				$mt = 19;
			}
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',$pid,$mt)");
		}
	}
}

?>

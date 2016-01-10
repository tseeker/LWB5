<?php

class beta5_alliance_setSuccessor {

	function beta5_alliance_setSuccessor($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Changes the successor for the current leader
	function run($aid, $sid) {
		$a = $this->lib->call('get', $aid);
		if (is_null($a) || $a['successor'] == $sid) {
			return;
		}
		$tag = addslashes($a['tag']);
		$tm = time();

		if (!is_null($a['successor'])) {
			$id = $a['successor'];
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT' AND mtype='alint' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',".$a['leader'].",2)");
		}

		$this->db->query("UPDATE alliance SET successor=".(is_null($sid)?"NULL":$sid) . " WHERE id=$aid");
		if (!is_null($sid)) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($sid,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$sid AND sent_on=$tm AND ftype='INT' AND mtype='alint' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',".$a['leader'].",3)");
		}
	}
}

?>

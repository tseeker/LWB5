<?php

class beta5_alliance_setDemocratic {

	function beta5_alliance_setDemocratic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Switches an alliance's government mode
	function run($aid, $demo) {
		$a = $this->lib->call('get', $aid);
		if (is_null($a)) {
			return null;
		}
		$ad = ($a['democracy'] != "f");
		if ($ad == $demo) {
			return;
		}

		$this->db->query("UPDATE alliance SET democracy=".dbBool($demo)." WHERE id=$aid");
		if ($demo) {
			$this->db->query("INSERT INTO alliance_candidate(alliance,candidate) VALUES ($aid,".$a['leader'].")");
			$q = $this->db->query("SELECT id FROM alliance_candidate WHERE candidate=".$a['leader']." AND alliance=$aid");
			list($cid) = dbFetchArray($q);
			$this->db->query("UPDATE player SET a_vote=$cid WHERE id=".$a['leader']);
			$msId = 4;
		} else {
			$this->db->query("DELETE FROM alliance_candidate WHERE alliance=$aid");
			$msId = 5;
		}

		$q = $this->db->query("SELECT id FROM player WHERE alliance=$aid AND a_status='IN' AND id<>".$a['leader']);
		$tm = time();
		$tag = addslashes($a['tag']);
		while ($r = dbFetchArray($q)) {
			$id = $r[0];
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q2 = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT' AND mtype='alint' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q2);
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',".$a['leader'].",$msId)");
		}
	}
}

?>

<?php

class beta5_alliance_stepDown {

	function beta5_alliance_stepDown($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Makes the alliance leader step down from power
	function run($aid, $isLeave = false) {
		$a = $this->lib->call('get', $aid);
		if (is_null($a) || is_null($a['successor'])) {
			return;
		}
		$this->db->query("UPDATE alliance SET leader=".$a['successor'].",successor=NULL WHERE id=$aid");
		if ($a['democracy'] == 't') {
			$q = $this->db->query("SELECT id FROM alliance_candidate WHERE candidate=".$a['successor']);
			if (dbCount($q) == 1) {
				$q2 = $this->db->query("SELECT id FROM alliance_candidate WHERE candidate=".$a['leader']);
				list($r1) = dbFetchArray($q);
				list($r2) = dbFetchArray($q2);
				$this->db->query("UPDATE player SET a_vote=$r2 WHERE a_vote=$r1");
				$this->db->query("DELETE FROM alliance_candidate WHERE candidate=".$a['successor']);
			} else {
				$q = $this->db->query("SELECT id FROM alliance_candidate WHERE candidate={$a['leader']}");
				list($id) = dbFetchArray($q);
				$this->db->query("UPDATE player SET a_vote=$id WHERE id={$a['successor']}");
			}
			$this->db->query("UPDATE alliance_candidate SET candidate=".$a['successor']." WHERE candidate=".$a['leader']);
		}

		if ($isLeave) {
			return;
		}

		$tm = time();
		$tag = addslashes($a['tag']);
		$qm = $this->db->query("SELECT id FROM player WHERE alliance=$aid AND a_status='IN'");
		while ($r = dbFetchArray($qm)) {
			$id = $r[0];
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT' AND mtype='alint' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			if ($id == $a['leader']) {
				$st = $a['successor'].",6";
			} elseif ($id == $a['successor']) {
				$st = $a['leader'].",7";
			} else {
				$st = $a['successor'].",8";
			}
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',$st)");
		}
	}
}

?>

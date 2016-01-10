<?php

class beta5_alliance_leave {

	function beta5_alliance_leave($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->rankings	= $this->lib->game->getLib('main/rankings');
	}


	// Leave an alliance
	function run($pid, $isKick = false) {
		$p = $this->players->call('get', $pid);
		if (is_null($p) || is_null($p['aid'])) {
			return;
		}

		$aid = $p['aid'];
		$a = $this->lib->call('get', $aid);
		if (is_null($a)) {
			return;
		}

		// Delete tech trading data
		$this->db->query("DELETE FROM tech_trade_list WHERE member = $pid");
		$this->db->query("DELETE FROM tech_trade_request WHERE player = $pid");
		$this->db->query("DELETE FROM tech_trade_order WHERE player = $pid OR send_to = $pid");

		$msgId = 9;
		$mpid = $pid;
		if ($pid == $a['leader']) {
			if (!is_null($a['successor'])) {
				$this->lib->call('stepDown', $aid, true);
				$msgId = 14;
				$mpid = $a['successor'];
			} else {
				$this->db->query("UPDATE alliance SET leader=NULL,democracy=TRUE WHERE id=$aid");
				$this->db->query("UPDATE alliance_grade SET can_vote=TRUE,can_be_cand=TRUE WHERE alliance=$aid");
				$msgId = 15;
			}
		} elseif ($pid == $a['successor']) {
			$this->db->query("UPDATE alliance SET successor=NULL WHERE id=$aid");
		}

		if ($a['democracy'] == 't') {
			$this->db->query("DELETE FROM alliance_candidate WHERE alliance=$aid AND candidate=".$pid);
		}

		$this->db->query("UPDATE player SET alliance=NULL,a_vote=NULL,a_status=NULL,a_grade=NULL WHERE id=$pid");

		$q = $this->db->query("SELECT COUNT(*) FROM player WHERE alliance=$aid AND a_status='IN'");
		list($pc) = dbFetchArray($q);
		if ($pc == 0) {
			$this->db->query("UPDATE player SET alliance=NULL,a_vote=NULL,a_status=NULL,a_grade=NULL WHERE alliance=$aid");
			$rt = $this->rankings->call('getType', 'a_general');
			$this->rankings->call('delete', $rt, $a['tag']);
			$this->db->query("DELETE FROM alliance WHERE id=$aid");
			return;
		}

		if ($isKick) {
			return;
		}

		if ($msgId == 9) {
			$l = $this->lib->call('getKeepers', $aid);
		} else {
			$l = array_keys($this->lib->call('getMembers', $aid));
		}
		$tag = addslashes($a['tag']);
		$tm = time();
		l::FIXME("Use message API"); // FIXME
		foreach	($l as $id) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT' ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$aid,'$tag',$mpid,$msgId)");
		}
	}
}

?>

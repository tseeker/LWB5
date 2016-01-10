<?php

class beta5_msg_sendInAlliance {

	function beta5_msg_sendInAlliance($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	// Sends a message from a player to his own alliance
	function run($src, $dst, $sub, $msg, $rep = null) {
		$moment = time();
		$pinf = $this->players->call('get', $src);
		if ($pinf['aid'] != $dst) {
			return false;
		}
		$tag = addslashes($pinf['alliance']);

		if (!is_null($rep)) {
			$q = $this->db->query("SELECT original FROM message WHERE player=$src AND id=$rep");
			if ($q && dbCount($q)) {
				list($oriMsgId) = dbFetchArray($q);
			} else {
				$rep = null;
			}
		}

		$qs  = "INSERT INTO message(player,sent_on,mtype,ftype,is_new";
		$qs .= (!is_null($rep)?",reply_to":"") . ") VALUES (";
		$qs .= "$src,$moment,'alliance','OUT',FALSE" . (!is_null($rep)?",$rep":"") . ")";
		$this->db->query($qs);
		$q = $this->db->query("SELECT id FROM message WHERE player=$src AND sent_on=$moment AND ftype='OUT'");
		list($id) = dbFetchArray($q);
		$this->db->query("INSERT INTO msg_alliance VALUES($id,$src,$dst,'$tag','".addslashes($sub)."','".addslashes($msg)."')");

		$pl = $this->db->query("SELECT id FROM player WHERE alliance=$dst AND a_status='IN' AND id <> $src");
		while ($r = dbFetchArray($pl)) {
			list($pid) = $r;

			if (!(is_null($rep) || is_null($oriMsgId))) {
				$q = $this->db->query("SELECT id FROM message WHERE id=$oriMsgId OR original=$oriMsgId");
				if ($q && dbCount($q)) {
					list($plMsgId) = dbFetchArray($q);
				} else {
					$plMsgId = $oriMsgId;
				}
			}

			$qs  = "INSERT INTO message(player,sent_on,mtype,ftype,original";
			$qs .= (!is_null($rep)?",reply_to":"") . ") VALUES (";
			$qs .= "$pid,$moment,'alliance','IN',$id".(!is_null($rep)?",$plMsgId":"").")";
			$this->db->query($qs);

			$q = $this->db->query("SELECT id FROM message WHERE player=$pid AND sent_on=$moment AND ftype='IN'");
			list($id2) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_alliance VALUES($id2,$src,$dst,'$tag','".addslashes($sub)."','".addslashes($msg)."')");
		}
	}
}

?>

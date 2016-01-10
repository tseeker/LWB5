<?php

class beta5_forums_reply {

	function beta5_forums_reply($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($a, $post, $sub, $txt, $ec, $es) {
		$tm = time();
		$fid = $post['fid']; $tid = $post['tid']; $pid = $post['id'];
		$qs = "INSERT INTO af_post(forum,topic,reply_to,author,moment,title,contents,enable_code,enable_smileys) VALUES ("
			. "$fid,$tid,$pid,$a,$tm,'".addslashes($sub)."','".addslashes($txt)."',"
			. dbBool($ec) . "," . dbBool($es) . ")";
		if (!$this->db->query($qs)) {
			return false;
		}

		$q = $this->db->query("SELECT id FROM af_post WHERE topic=$tid AND reply_to=$pid AND author=$a AND moment=$tm");
		if (!$q || dbCount($q) != 1) {
			return false;
		}
		list($pid) = dbFetchArray($q);

		$this->db->query("UPDATE af_topic SET last_post=$pid WHERE id=$tid");
		$this->db->query("UPDATE af_forum SET posts=posts+1,last_post=$pid WHERE id=$fid");
		$this->lib->call('markUnread', $tid,$a);
		return $pid;
	}
}

?>

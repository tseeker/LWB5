<?php

class main_forums_newTopic {

	function main_forums_newTopic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($a, $fid, $sub, $txt, $ec, $es, $st) {
		$tm = time();
		$qs = "INSERT INTO f_post(forum,author,moment,title,contents,enable_code,enable_smileys) VALUES ("
			. "$fid,$a,$tm,'".addslashes($sub)."','".addslashes($txt)."',"
			. dbBool($ec) . "," . dbBool($es) . ")";
		if (!$this->db->query($qs)) {
			return false;
		}

		$q = $this->db->query("SELECT id FROM f_post WHERE forum=$fid AND topic IS NULL AND author=$a AND moment=$tm ORDER BY id DESC LIMIT 1");
		if (!$q || dbCount($q) != 1) {
			return false;
		}
		list($pid) = dbFetchArray($q);

		$this->db->query("INSERT INTO f_topic(forum,first_post,last_post,sticky) VALUES($fid,$pid,$pid,"
			. dbBool($st) . ")");
		$q = $this->db->query("SELECT id FROM f_topic WHERE forum=$fid AND first_post=$pid");
		if (!$q || dbCount($q) != 1) {
			return false;
		}
		list($tid) = dbFetchArray($q);

		$this->db->query("UPDATE f_post SET topic=$tid WHERE id=$pid");
		$this->db->query("UPDATE f_forum SET topics=topics+1,posts=posts+1,last_post=$pid WHERE id=$fid");
		$this->lib->call('markRead', $tid, $a);
		return $pid;
	}
}

?>

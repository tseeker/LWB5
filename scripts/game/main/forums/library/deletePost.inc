<?php

class main_forums_deletePost {

	function main_forums_deletePost($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($postId) {
		$q = $this->db->query("SELECT forum,topic,reply_to FROM f_post WHERE id=$postId AND deleted IS NULL");
		if (!($q && dbCount($q))) {
			return;
		}
		list($fid,$tid,$rtid) = dbFetchArray($q);
		$this->db->query("UPDATE f_post SET reply_to=$rtid WHERE reply_to=$postId");
		$this->db->query("UPDATE f_forum SET posts=posts-1 WHERE id=$fid");
		$this->db->query("UPDATE f_post SET deleted=" . time() . " WHERE id=$postId");
		$q = $this->db->query("SELECT id FROM f_post WHERE topic=$tid AND deleted IS NULL ORDER BY moment DESC LIMIT 1");
		list($lastid) = dbFetchArray($q);
		$this->db->query("UPDATE f_topic SET last_post=$lastid WHERE id=$tid");
		$this->lib->call('updateLast', $fid);
	}
}

?>

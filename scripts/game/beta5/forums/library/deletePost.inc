<?php

class beta5_forums_deletePost {

	function beta5_forums_deletePost($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($postId) {
		$q = $this->db->query(
			"SELECT f.id AS forum, t.id AS topic, reply_to FROM af_post p, af_forum f, af_topic t "
				. "WHERE p.id = $postId AND t.id = p.topic AND f.id = p.forum "
				. "FOR UPDATE OF p, f, t"
		);
		if (!($q && dbCount($q))) {
			return;
		}
		list($fid,$tid,$rtid) = dbFetchArray($q);
		$this->db->query("UPDATE af_post SET reply_to=$rtid WHERE reply_to=$postId");
		$this->db->query("UPDATE af_forum SET posts=posts-1 WHERE id=$fid");
		$this->db->query("DELETE FROM af_post WHERE id=$postId");
		$q = $this->db->query("SELECT id FROM af_post WHERE topic=$tid ORDER BY moment DESC LIMIT 1");
		list($lastid) = dbFetchArray($q);
		$this->db->query("UPDATE af_topic SET last_post=$lastid WHERE id=$tid");
		$this->lib->call('updateLast', $fid);
	}
}

?>

<?php

class beta5_forums_deleteTopic {

	function beta5_forums_deleteTopic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($forum, $topic) {
		$q = $this->db->query(
			"SELECT t.id, f.id, p.id FROM af_post p, af_topic t, af_forum f "
				. "WHERE p.forum = $forum AND p.topic = $topic AND t.id = p.topic AND f.id = p.forum "
				. "FOR UPDATE OF p, t, f"
		);
		if (!($q && dbCount($q))) {
			return;
		}
		$np = dbCount($q);
		$this->db->query("DELETE FROM af_post WHERE topic=$topic AND forum=$forum");
		$this->db->query("UPDATE af_forum SET posts=posts-$np,topics=topics-1 WHERE id=$forum");
		$this->lib->call('updateLast', $forum);
	}
}

?>

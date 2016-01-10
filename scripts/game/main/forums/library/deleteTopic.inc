<?php

class main_forums_deleteTopic {

	function main_forums_deleteTopic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($forum, $topic) {
		$q = $this->db->query("SELECT COUNT(*) FROM f_post WHERE forum=$forum AND topic=$topic AND deleted IS NULL");
		if (!($q && dbCount($q))) {
			return;
		}
		list($np) = dbFetchArray($q);
		$tm = time();
		$this->db->query("UPDATE f_post SET deleted=$tm WHERE topic=$topic AND forum=$forum");
		$this->db->query("UPDATE f_topic SET deleted=$tm WHERE id=$topic");
		$this->db->query("UPDATE f_forum SET posts=posts-$np,topics=topics-1 WHERE id=$forum");
		$this->db->query("DELETE FROM f_read WHERE topic=$topic");
		$this->lib->call('updateLast', $forum);
	}
}

?>

<?php

class beta5_forums_moveTopic {

	function beta5_forums_moveTopic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($forum, $topic, $dest, $user) {
		$this->db->query("SELECT * FROM af_forum WHERE id IN ($forum,$dest) FOR UPDATE");

		$q = $this->db->query("SELECT COUNT(*) FROM af_post WHERE forum=$forum AND topic=$topic");
		if (!($q && dbCount($q))) {
			return;
		}
		list($np) = dbFetchArray($q);
		$this->db->query("UPDATE af_post SET forum=$dest WHERE topic=$topic AND forum=$forum");
		$this->db->query("UPDATE af_topic SET forum=$dest WHERE id=$topic AND forum=$forum");
		$this->db->query("UPDATE af_forum SET posts=posts-$np,topics=topics-1 WHERE id=$forum");
		$this->db->query("UPDATE af_forum SET posts=posts+$np,topics=topics+1 WHERE id=$dest");
		$this->lib->call('markUnread', $topic, $user);
		$this->lib->call('updateLast', $forum);
		$this->lib->call('updateLast', $dest);
	}
}

?>

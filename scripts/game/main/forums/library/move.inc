<?php

class main_forums_move {

	function main_forums_move($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($forum, $topic, $dest, $user) {
		$this->db->query("SELECT * FROM f_forum WHERE id IN ($forum,$dest) FOR UPDATE");

		$q = $this->db->query("SELECT COUNT(*) FROM f_post WHERE forum=$forum AND topic=$topic AND deleted IS NULL");
		if (!($q && dbCount($q))) {
			return;
		}
		list($np) = dbFetchArray($q);
		$this->db->query("UPDATE f_post SET forum=$dest WHERE topic=$topic AND forum=$forum");
		$this->db->query("UPDATE f_topic SET forum=$dest WHERE id=$topic AND forum=$forum");
		$this->db->query("UPDATE f_forum SET posts=posts-$np,topics=topics-1 WHERE id=$forum");
		$this->db->query("UPDATE f_forum SET posts=posts+$np,topics=topics+1 WHERE id=$dest");
		$this->lib->call('markUnread', $topic, $user);
		$this->lib->call('updateLast', $forum);
		$this->lib->call('updateLast', $dest);
	}
}

?>

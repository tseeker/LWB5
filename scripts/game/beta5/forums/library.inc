<?php

class beta5_forums_library {
	var $index	= array(
		'deletePost',
		'deleteTopic',
		'edit',
		'getLatest',
		'getPost',
		'getPosts',
		'getStructure',
		'getTopic',
		'getTopics',
		'moveTopic',
		'newTopic',
		'reply',
		'searchPosts',
		'searchTopics',
		'updateLast'
	);

	function beta5_forums_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get the amount of read topics in an alliance forum
	function getRead($fid, $pid) {
		$q = $this->db->query("SELECT COUNT(*) FROM af_read r,af_topic t WHERE t.id=r.topic AND t.forum=$fid AND r.reader=$pid");
		list($nr) = dbFetchArray($q);
		return $nr;
	}

	function isRead($topic, $player) {
		$q = $this->db->query("SELECT * FROM af_read WHERE topic=$topic AND reader=$player");
		return $q && dbCount($q);
	}

	function markRead($topic, $player) {
		if ($this->isRead($topic,$player)) {
			return false;
		}
		$this->db->query("DELETE FROM af_read WHERE topic=$topic AND reader=$player");
		$this->db->query("INSERT INTO af_read(topic,reader)VALUES($topic,$player)");
		return true;
	}

	function markUnread($topic, $player) {
		$this->db->query("DELETE FROM af_read WHERE topic=$topic AND reader<>$player");
	}

	function switchSticky($forum, $topic) {
		$this->db->query("UPDATE af_topic SET sticky=NOT sticky WHERE id=$topic AND forum=$forum");
	}

	function markForumRead($fid, $pid) {
		$q = $this->db->query("SELECT id FROM af_topic WHERE forum=$fid");
		while ($r = dbFetchArray($q)) {
			$this->markRead($r[0], $pid);
		}
	}
}

?>

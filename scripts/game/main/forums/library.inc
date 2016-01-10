<?php

class main_forums_library {
	var $index	= array(
		'deletePost',
		'deleteTopic',
		'edit',
		'get',
		'getAdministrator',
		'getCategories',
		'getCategory',
		'getForums',
		'getModerator',
		'getPost',
		'getPosts',
		'getTopic',
		'getTopics',
		'move',
		'newTopic',
		'reply',
		'signature',
		'substitute',
		'updateLast',
	);

	function main_forums_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function getVersionCategory($ver) {
		$q = $this->db->query("SELECT id,description FROM f_category WHERE title='!$ver!'");
		return dbFetchHash($q);
	}

	function isRead($topic, $player) {
		$q = $this->db->query("SELECT * FROM f_read WHERE topic=$topic AND reader=$player");
		return $q && dbCount($q);
	}

	function markRead($topic, $player) {
		if ($this->isRead($topic,$player)) {
			return false;
		}
		$this->db->query("DELETE FROM f_read WHERE topic=$topic AND reader=$player");
		$this->db->query("INSERT INTO f_read(topic,reader)VALUES($topic,$player)");
		return true;
	}

	function markUnread($topic, $player) {
		$this->db->query("DELETE FROM f_read WHERE topic=$topic AND reader<>$player");
	}

	// Get the amount of unread topics in a forum
	function getRead($fid, $uid) {
		$q = $this->db->query("SELECT COUNT(*) FROM f_read r,f_topic t WHERE t.id=r.topic AND t.forum=$fid AND r.reader=$uid AND t.deleted IS NULL");
		list($nr) = dbFetchArray($q);
		return $nr;
	}

	function switchSticky($forum, $topic) {
		$this->db->query("UPDATE f_topic SET sticky=NOT sticky WHERE id=$topic AND forum=$forum AND deleted IS NULL");
	}

	function markForumRead($fid, $uid) {
		$q = $this->db->query("SELECT id FROM f_topic WHERE forum=$fid AND deleted IS NULL");
		while ($r = dbFetchArray($q)) {
			$this->markRead($r[0], $uid);
		}
	}
}

?>

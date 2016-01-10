<?php

class beta5_forums_getTopics {

	function beta5_forums_getTopics($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	function run($f, $first, $count) {
		$q = $this->db->query(
			"SELECT t.id AS id,p.title AS title,p.moment AS moment,"
				. "p.author AS author_id,p2.moment AS last_moment,"
				. "p2.author AS last_author_id,t.sticky AS sticky "
			. "FROM af_topic t,af_post p,af_post p2 "
			. "WHERE t.forum=$f AND p.id=t.first_post AND p2.id=t.last_post "
			. "ORDER BY sticky DESC,last_moment DESC LIMIT $count OFFSET $first"
		);
		$a = array();
		if (!$q) {
			return $a;
		}
		while ($rs = dbFetchHash($q)) {
			$q2 = $this->db->query("SELECT COUNT(*)-1 FROM af_post WHERE topic=".$rs["id"]);
			list($rs['posts']) = dbFetchArray($q2);
			$rs['sticky'] = ($rs['sticky'] == 't');
			$rs['author'] = $this->players->call('getName', $rs['author_id']);
			$rs['last_author'] = $this->players->call('getName', $rs['last_author_id']);
			array_push($a, $rs);
		}
		return $a;
	}
}

?>

<?php

class main_forums_getTopics {

	function main_forums_getTopics($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($f, $first, $count) {
		$q = $this->db->query(
			"SELECT t.id AS id,p.title AS title,p.moment AS moment,"
				. "u.name AS author,p2.moment AS last_moment,"
				. "u2.name AS last_author,t.sticky AS sticky "
			. "FROM f_topic t,f_post p,account u,f_post p2,account u2 "
			. "WHERE t.forum=$f AND p.id=t.first_post AND u.id=p.author "
				. "AND p2.id=t.last_post AND u2.id=p2.author "
				. "AND t.deleted IS NULL "
			. "ORDER BY sticky DESC,last_moment DESC LIMIT $count OFFSET $first"
		);
		$a = array();
		while ($q && $rs = dbFetchHash($q)) {
			$q2 = $this->db->query("SELECT COUNT(*) - 1 FROM f_post WHERE topic={$rs["id"]} AND deleted IS NULL");
			list($rs['posts']) = dbFetchArray($q2);
			$rs['sticky'] = ($rs['sticky'] == 't');
			array_push($a, $rs);
		}
		return $a;
	}
}

?>

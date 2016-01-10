<?php

class main_forums_getTopic {

	function main_forums_getTopic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($tid) {
		// Get main topic data
		$q = $this->db->query(
			"SELECT t.id AS id,p.title AS title,"
				. "p.id AS fpid,t.last_post AS lpid,"
				. "f.id AS fid,f.title AS fname,"
				. "c.id AS cid,c.title AS cname "
			. "FROM f_topic t,f_post p,f_forum f,f_category c "
			. "WHERE t.id=$tid AND p.id=t.first_post "
				. "AND f.id=t.forum AND c.id=f.category "
				. "AND t.deleted IS NULL"
		);
		if (!$q || dbCount($q) != 1) {
			return null;
		}
		$rv = dbFetchHash($q);

		// Get post count
		$q = $this->db->query("SELECT COUNT(*) FROM f_post WHERE topic=$tid AND deleted IS NULL");
		list($rv["nitems"]) = dbFetchArray($q);

		return $rv;
	}
}

?>

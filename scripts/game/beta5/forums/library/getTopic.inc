<?php

class beta5_forums_getTopic {

	function beta5_forums_getTopic($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($tid) {
		// Get main topic data
		$q = $this->db->query(
			"SELECT t.id AS id,p.title AS title,"
				. "f.id AS fid,f.title AS fname,"
				. "p.id AS fpid,t.last_post AS lpid "
			. "FROM af_topic t,af_post p,af_forum f "
			. "WHERE t.id=$tid AND p.id=t.first_post AND f.id=t.forum"
		);
		if (!$q || dbCount($q) != 1) {
			return null;
		}
		$rv = dbFetchHash($q);

		// Get post count
		$q = $this->db->query("SELECT COUNT(*) FROM af_post WHERE topic=$tid");
		list($rv["nitems"]) = dbFetchArray($q);

		return $rv;
	}
}

?>

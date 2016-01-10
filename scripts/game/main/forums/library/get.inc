<?php

class main_forums_get {

	function main_forums_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($f) {
		$q = $this->db->query(
			"SELECT f.id AS id,f.title AS title,f.description AS description,"
				. "f.user_post AS user_post,f.topics AS ntopics,"
				. "c.id AS pid,c.title AS ptitle "
			. "FROM f_forum f,f_category c "
			. "WHERE f.id=$f AND c.id=f.category"
		);
		if (!$q||dbCount($q)!=1) {
			return null;
		}
		$a = dbFetchHash($q);
		$f['user_post'] = ($f['user_post'] == 't');
		return $a;
	}
}

?>

<?php

class beta5_forums_updateLast {

	function beta5_forums_updateLast($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($forum) {
		$q = $this->db->query("SELECT id FROM af_post WHERE forum=$forum ORDER BY moment DESC LIMIT 1");
		if (!($q && dbCount($q))) {
			return;
		}
		list($id) = dbFetchArray($q);
		$this->db->query("UPDATE af_forum SET last_post=$id WHERE id=$forum");
	}
}

?>

<?php

class main_forums_updateLast {

	function main_forums_updateLast($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($forum) {
		$q = $this->db->query("SELECT id FROM f_post WHERE forum=$forum AND deleted IS NULL ORDER BY moment DESC LIMIT 1");
		if (!($q && dbCount($q))) {
			return;
		}
		list($id) = dbFetchArray($q);
		$this->db->query("UPDATE f_forum SET last_post=$id WHERE id=$forum");
	}
}

?>

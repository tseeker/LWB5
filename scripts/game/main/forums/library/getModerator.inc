<?php

class main_forums_getModerator {

	function main_forums_getModerator($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid) {
		$q = $this->db->query("SELECT forum FROM f_moderator WHERE \"user\"=$uid");
		$a = array();
		while ($r = dbFetchArray($q)) {
			array_push($a, $r[0]);
		}
		return $a;
	}
}

?>

<?php

class beta5_alliance_createForum {

	function beta5_alliance_createForum($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Creates an alliance forum
	function run($aid, $name, $userPost, $after, $description) {
		if ($after == -1) {
			$ao = -1;
		} else {
			$q = $this->db->query("SELECT forder FROM af_forum WHERE id=$after");
			list($ao) = dbFetchArray($q);
		}
		$this->db->query("UPDATE af_forum SET forder=forder+1 WHERE forder>$ao AND alliance=$aid");
		$ao ++;
		$this->db->query(
			"INSERT INTO af_forum(alliance,forder,title,description,user_post) VALUES("
			. "$aid,$ao,'" . addslashes($name) . "','". addslashes($description) . "',"
			. ($userPost?'TRUE':'FALSE') . ")"
		);
	}
}

?>

<?php

class beta5_alliance_modifyForum {

	function beta5_alliance_modifyForum($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Modifies an alliance forum
	function run($id, $name, $userPost, $description) {
		$this->db->query(
			"UPDATE af_forum SET title='".addslashes($name)."',user_post=".($userPost?'TRUE':'FALSE').",description='".addslashes($description)
			."' WHERE id=$id"
		);
	}
}

?>

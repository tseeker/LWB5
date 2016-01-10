<?php

class beta5_alliance_setForumAccess {

	function beta5_alliance_setForumAccess($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Modifies the access list for an alliance forum
	function run($id, $readers, $mods) {
		$this->db->query("DELETE FROM algr_forums WHERE forum=$id");
		foreach	($readers as $rid) {
			$this->db->query("INSERT INTO algr_forums VALUES($rid,$id,false)");
		}
		foreach	($mods as $rid) {
			$this->db->query("INSERT INTO algr_forums VALUES($rid,$id,true)");
		}
	}
}

?>

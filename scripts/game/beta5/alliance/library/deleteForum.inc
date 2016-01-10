<?php

class beta5_alliance_deleteForum {

	function beta5_alliance_deleteForum($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Destroys an alliance forum
	function run($id) {
		$q = $this->db->query("SELECT alliance,forder FROM af_forum WHERE id=$id");
		list($aid,$o) = dbFetchArray($q);
		$this->db->query("DELETE FROM af_forum WHERE id=$id");
		$this->db->query("UPDATE af_forum SET forder=forder-1 WHERE alliance=$aid AND forder>$o");
	}
}

?>

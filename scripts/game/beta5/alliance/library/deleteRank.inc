<?php

class beta5_alliance_deleteRank {

	function beta5_alliance_deleteRank($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Deletes a rank from an alliance's ACL
	function run($aid, $rId, $nrId) {
		$this->db->query("UPDATE player SET a_grade=$nrId WHERE alliance=$aid AND a_status='IN' AND a_grade=$rId");
		$this->db->query("DELETE FROM alliance_grade WHERE alliance=$aid AND id=$rId");
	}
}

?>

<?php

class beta5_alliance_moveForum {

	function beta5_alliance_moveForum($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Moves an alliance forum up or down in the list
	function run($id, $up) {
		$q = $this->db->query("SELECT alliance,forder FROM af_forum WHERE id=$id");
		list($aid,$o) = dbFetchArray($q);
		if ($o == 0 && $up) {
			return;
		}

		$q = $this->db->query("SELECT MAX(forder) FROM af_forum WHERE alliance=$aid");
		list($mo) = dbFetchArray($q);
		if ($o == $mo && !$up) {
			return;
		}

		$this->db->query("UPDATE af_forum SET forder=" . ($mo+1) . " WHERE id=$id");
		if ($up) {
			$this->db->query("UPDATE af_forum SET forder=forder+1 WHERE alliance=$aid AND forder=".($o-1));
			$this->db->query("UPDATE af_forum SET forder=".($o-1)." WHERE id=$id");
		} else {
			$this->db->query("UPDATE af_forum SET forder=forder-1 WHERE alliance=$aid AND forder=".($o+1));
			$this->db->query("UPDATE af_forum SET forder=".($o+1)." WHERE id=$id");
		}
	}
}

?>

<?php

class beta5_player_reorderAllies {

	function beta5_player_reorderAllies($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Reorders the list of allies after it's been modified
	function run($pid) {
		$q = $this->db->query("SELECT level FROM trusted WHERE player=$pid ORDER BY level ASC");
		$i = 0;
		while ($r = dbFetchArray($q)) {
			if ($r[0] != $i) {
				$this->db->query("UPDATE trusted SET level=$i WHERE player=$pid AND level=".$r[0]);
			}
			$i ++;
		}
	}
}

?>

<?php

class main_rankings_delete {

	function main_rankings_delete($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($type,$id) {
		$q = $this->db->query("SELECT ranking FROM ranking WHERE r_type=$type AND id='".addslashes($id)."'");
		list($rk) = dbFetchArray($q);
		if (!is_null($rk)) {
			$this->db->query("DELETE FROM ranking WHERE r_type=$type AND id='".addslashes($id)."'");
			$this->db->query("UPDATE ranking SET ranking=ranking-1 WHERE r_type=$type AND ranking>$rk");
		}
	}
}

?>

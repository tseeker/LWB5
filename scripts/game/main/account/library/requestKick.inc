<?php

class main_account_requestKick {

	function main_account_requestKick($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($admin, $account, $reason) {
		$q = $this->db->query("SELECT * FROM adm_kick WHERE to_kick=$account AND status<>'N'");
		if (!$q || dbCount($q)) {
			return false;
		}

		$id = $this->db->query("INSERT INTO adm_kick (to_kick,requested_by,requested_at,reason) "
			. "VALUES ($account,$admin," . time() . ",'" . addslashes($reason) . "')");

		return !!$id;
	}
}

?>

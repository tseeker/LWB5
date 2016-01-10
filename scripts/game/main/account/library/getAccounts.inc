<?php

class main_account_getAccounts {

	function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run() {
		$q = $this->db->query("SELECT COUNT(*) FROM account WHERE status IN ('STD','VAC')");
		if (!($q && dbCount($q))) {
			return null;
		}
		list($total) = dbFetchArray($q);

		$q = $this->db->query("SELECT COUNT(*) FROM account WHERE status IN ('STD','VAC') AND last_login IS NOT NULL AND (last_logout IS NULL OR last_logout<last_login)");
		if (!($q && dbCount($q))) {
			return null;
		}
		list($online) = dbFetchArray($q);

		return array($total, $online);
	}
}

?>

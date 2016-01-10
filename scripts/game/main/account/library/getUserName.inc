<?php

class main_account_getUserName {
	var $userNames	= array();	// FIXME: can be cached

	function main_account_getUserName($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid) {
		if (!is_null($this->userNames[$uid])) {
			return $this->userNames[$uid];
		}
		$q = $this->db->query("SELECT name FROM account WHERE id=$uid");
		if (!($q && dbCount($q))) {
			return null;
		}
		list($this->userNames[$uid]) = dbFetchArray($q);
		return $this->userNames[$uid];
	}
}

?>

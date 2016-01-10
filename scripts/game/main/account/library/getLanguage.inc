<?php

class main_account_getLanguage {

	function main_account_getLanguage($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($id) {
		$ql = $this->db->query("SELECT value FROM user_preferences WHERE id='language' AND version='main' AND account=$id");
		if ($ql && dbCount($ql)) {
			list($l) = dbFetchArray($ql);
		} else {
			$l = 'en';
		}
		return $l;
	}
}

?>

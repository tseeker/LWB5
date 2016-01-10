<?php

class main_account_setQuitCountdown {

	function main_account_setQuitCountdown($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($uid, $reason) {
		$reason = trim(preg_replace('/\s+/', ' ', $reason));
		if ($reason == '') {
			$rqs = "";
		} else {
			$rqs = ",reason='" . addslashes($reason) . "'";
		}
		$this->db->query("UPDATE account SET quit_ts=UNIX_TIMESTAMP(NOW())$rqs WHERE id=$uid AND status='STD'");
	}


}

?>

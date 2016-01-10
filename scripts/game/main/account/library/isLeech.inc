<?php

class main_account_isLeech {

	function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($id) {
		$q = $this->db->query("SELECT UNIX_TIMESTAMP(NOW())-donated FROM pp_history ORDER BY donated ASC LIMIT 1");
		if (!($q && dbCount($q))) {
			return true;
		}
		list($interval) = dbFetchArray($q);
		if ($interval == 0) {
			return true;
		}
		$days = $interval / 86400;

		$q = $this->db->query("SELECT SUM(amount) FROM pp_history WHERE account=$id");
		if (!($q && dbCount($q))) {
			return false;
		}
		list($sum) = dbFetchArray($q);
		$sum = is_null($sum) ? 0 : $sum;

		return ($sum / $days <= 0.05);
	}
}

?>

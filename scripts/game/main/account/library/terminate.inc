<?php

class main_account_terminate {

	function main_account_terminate($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($uid, $status, $reason = null) {
		foreach (config::getGames() as $game) {
			if ($game->name == 'main' || $game->status() == 'FINISHED') {
				continue;
			}

			$lib = $game->getLib();
			$pid = $lib->call('doesUserPlay', $uid);
			if (is_null($pid)) {
				continue;
			}

			$lib->call('leaveGame', $pid, $status);
		}

		$qs =  is_null($reason) ? "" : (",reason='" . addslashes($reason) . "'");
		$this->db->query("UPDATE account SET status='$status',quit_ts=NULL,vac_start=NULL$qs WHERE id=$uid");
		$this->lib->call('log', $uid, 'q');
	}
}

?>

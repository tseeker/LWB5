<?php

class main_vacation_start {

	function main_vacation_start($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->accounts	= $this->lib->game->getLib('main/account');
	}

	function run($uid) {
		foreach (config::getGames() as $game) {
			if ($game->name == 'main') {
				continue;
			}

			$lib = $game->getLib();
			$pid = $lib->call('doesUserPlay', $uid);
			if (!is_null($pid)) {
				$lib->call('startVacation', $pid);
			}
		}

		$this->db->query("UPDATE account SET status='VAC',vac_start=NULL,vac_credits=vac_credits-1 WHERE id=$uid");
		$this->accounts->call('log', $uid, 'vs');

		logText("Account $uid has entered vacation mode");
	}
}

?>

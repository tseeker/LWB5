<?php

class main_vacation_leave {

	function main_vacation_leave($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->accounts	=& $this->lib->game->getLib('main/account');
	}

	function run($uid) {
		foreach (config::getGames() as $game) {
			if ($game->name == 'main') {
				continue;
			}

			$lib = $game->getLib();
			$pid = $lib->call('doesUserPlay', $uid);
			if (!is_null($pid)) {
				$lib->call('leaveVacation', $pid);
			}
		}

		$this->accounts->call('log', $uid, 've');
		$this->db->query("UPDATE account SET status='STD' WHERE id=$uid");

		logText("Account $uid has left vacation mode");
	}
}

?>

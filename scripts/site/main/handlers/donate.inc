<?php

class page_handler {
	var $needsAuth = true;
	var $ajax = array();

	function handle($input) {
		$this->lib = $this->game->getLib("main/paypal");

		$this->data = array();
		if ($input['doit'] == 1) {
			$this->data['pid'] = $this->lib->call('newTicket', $_SESSION['userid']);
			logText("PAYPAL: generated ticket {$this->data['pid']} for account #{$_SESSION['userid']}", LOG_INFO);
			$this->data['doit'] = true;
		} else {
			$this->data['selfContrib'] = $this->lib->call('getUserContributions', $_SESSION['userid']);
			$this->data['hist'] = $this->lib->call('getUserHistory', $_SESSION['userid']);
			$this->data['totalContrib'] = $this->lib->call('getTotalContributions');
			$this->data['totalMonth'] = $this->lib->call('getMonthContributions');
		}

		$this->output = "donate";
	}
}

?>

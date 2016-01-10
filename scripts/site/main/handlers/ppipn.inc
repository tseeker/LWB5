<?php

class page_handler {
	var $noTracking	= true;

	function handleIPN(&$input) {
		$lid = $this->lib->call('logIPN', $input);

		$ticket = $input['item_number'];
		if ($input['payment_status'] == 'Completed') {
			if ($input['mc_currency'] == 'EUR') {
				$cash = $input['mc_gross'];
			} else {
				$cash = $input['settle_amount'];
			}
			$cash = (float) $cash;

			if ($cash == 0) {
				logText("PAYPAL: could not retrieve the amount (log ID #$lid)", LOG_WARNING);
				return;
			}

			$this->lib->call('addDonation', $ticket, $cash);
			logText("PAYPAL: accepted donation ($cash euros, log ID #$lid)", LOG_INFO);
		} elseif ($input['payment_status'] == 'Failed' || $input['payment_status'] == 'Denied') {
			$this->lib->call('cancelDonation', $ticket);
			logText("PAYPAL: received cancelled donation :( (log ID #$lid)", LOG_INFO);
		} else {
			logText("PAYPAL: ignoring IPN with status '{$input['payment_status']}' (log ID #$lid)", LOG_INFO);
		}
	}

	function handle($input) {
		$this->lib = $this->game->getLib("main/paypal");

		logText("PAYPAL: handling incoming IPN from {$_SERVER['REMOTE_ADDR']}", LOG_INFO);
		if ($this->lib->call('checkIPN')) {
			$this->handleIPN($input);
		}

		$this->output = "ppipn";
	}
}

?>

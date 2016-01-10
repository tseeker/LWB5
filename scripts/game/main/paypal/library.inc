<?php

class main_paypal_library {
	var $index	= array();

	function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function newTicket($account) {
		$this->db->query("DELETE FROM pp_ticket WHERE UNIX_TIMESTAMP(NOW())-created>2592000");

		do {
			$v = md5(uniqid(rand()));
			$q  = "SELECT * FROM pp_ticket WHERE md5_id='$v'";
			$qr = $this->db->query($q);
		} while ($qr && dbCount($qr));

		if (is_null($qr)) {
			return null;
		}

		$this->db->query("INSERT INTO pp_ticket(account,md5_id) VALUES($account,'$v')");
		return $v;
	}


	function getUserContributions($account) {
		$q = $this->db->query("SELECT SUM(amount) FROM pp_history WHERE account=$account");
		if (!($q && dbCount($q))) {
			return 0;
		}
		list($amount) = dbFetchArray($q);
		return $amount;
	}


	function getTotalContributions() {
		$q = $this->db->query("SELECT SUM(amount) FROM pp_history");
		if (!($q && dbCount($q))) {
			return 0;
		}
		list($amount) = dbFetchArray($q);
		return $amount;
	}


	function getMonthContributions() {
		$q = $this->db->query("SELECT SUM(amount) FROM pp_history WHERE donated>UNIX_TIMESTAMP(NOW())-2592000");
		if (!($q && dbCount($q))) {
			return 0;
		}
		list($amount) = dbFetchArray($q);
		return $amount;
	}


	function getUserHistory($account) {
		$q = $this->db->query("SELECT donated,amount FROM pp_history WHERE account=$account ORDER BY donated DESC");
		if (!($q && dbCount($q))) {
			return array();
		}

		$res = array();
		while ($r = dbFetchArray($q)) {
			array_push($res, array(
				"time"		=> strftime("%Y-%m-%d %H:%M:%S", $r[0]),
				"amount"	=> number_format($r[1])
			));
		}

		return $res;
	}


	function checkIPN() {
		// Generate the verification request
		$ra = array();
		foreach ($_POST as $key => $value) {
			$value = urlencode(stripslashes($value));
			array_push($ra, "$key=$value");
		}
		$req = join('&', $ra);

		// Connect to the PayPal server
		$curl = curl_init("http://www.paypal.com/cgi-bin/webscr?cmd=_notify%2dvalidate&$req");
		curl_setopt($curl, CURLOPT_HEADER, 0);
		curl_setopt($curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
		$rv = curl_exec($curl);
		logText("PAYPAL: CURL request returned '$rv'", LOG_DEBUG);

		if (preg_match("/VERIFIED/", $rv)) {
			$handled = true;
			$ok = true;
		} elseif (preg_match("/INVALID/", $rv)) {
			$handled = true;
			$ok = false;
		} else {
			logText("PAYPAL: remote server didn't return VERIFIED or INVALID while checking IPN!", LOG_ERR);
			logText("PAYPAL: item number was '{$_POST['item_number']}'", LOG_INFO);
			return false;
		}

		if (!$ok) {
			logText("PAYPAL: remote server returned INVALID while checking IPN!", LOG_ERR);
			logText("PAYPAL: item number was '{$_POST['item_number']}'", LOG_INFO);
		}
		return $ok;
	}


	function logIPN($input) {
		$fields = array(
			"receiver_email", "item_number", "payment_status", "pending_reason", "payment_date", "mc_gross",
			"mc_fee", "tax", "mc_currency", "txn_id", "txn_type", "payer_email", "payer_status", "payment_type",
			"verify_sign", "referrer_id"
		);

		$q1 = "INSERT INTO pp_ipn(received";
		$q2 = ") VALUES (" . time();
		foreach ($fields as $f) {
			$q1 .= ",$f";
			$q2 .= ",'" . addslashes($input[$f]) . "'";
		}

		return $this->db->query("$q1$q2)");
	}


	function addDonation($ticket, $amount) {
		$q = $this->db->query("SELECT account FROM pp_ticket WHERE md5_id='" . addslashes($ticket) . "'");
		if (!($q || dbCount($q))) {
			logText("PAYPAL: couldn't find account number for ticket $ticket", LOG_ERR);
			return;
		}
		list($account) = dbFetchArray($q);

		$this->db->query("DELETE FROM pp_ticket WHERE md5_id='" . addslashes($ticket) . "'");
		$this->db->query("INSERT INTO pp_history VALUES($account," . time() . ",$amount)");
		$this->db->query("UPDATE credits SET credits_obtained = credits_obtained + " . round(10000 * $amount) . " WHERE account = $account");
	}
}

?>

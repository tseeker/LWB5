<?php

class beta5_player_transferFunds {

	function beta5_player_transferFunds($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Transfers cash from an account to another
	function run($s, $d, $a) {
		// Transfer cash
		$this->db->query("UPDATE player SET cash=cash-$a WHERE id=$s");
		$this->db->query("UPDATE player SET cash=cash+$a WHERE id=$d");
		$this->db->query("INSERT INTO donation_log VALUES(" . time() . ",$s,$d,$a)");

		// Send a message to the recipient
		$tm = time();
		$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($d,$tm,'cash','INT',TRUE)");
		$q = $this->db->query("SELECT id FROM message WHERE player=$d AND sent_on=$tm AND mtype='cash' ORDER BY id DESC LIMIT 1");
		list($mid) = dbFetchArray($q);
		$this->db->query("INSERT INTO msg_cash VALUES($mid,$s,$a)");
	}
}

?>

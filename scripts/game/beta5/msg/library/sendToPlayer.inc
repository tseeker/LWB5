<?php

class beta5_msg_sendToPlayer {

	function beta5_msg_sendToPlayer($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Sends a message from player to player
	function run($src, $dst, $sub, $msg, $rep = null) {
		$moment = time();
		if (is_null($rep)) {
			$oriMsgId = null;
		} else {
			$q = $this->db->query("SELECT original FROM message WHERE player=$src AND id=$rep");
			if ($q && dbCount($q)) {
				list($oriMsgId) = dbFetchArray($q);
			} else {
				$oriMsgId = $rep = null;
			}
		}

		$message = array(
			"sender"	=> $src,
			"recipient"	=> $dst,
			"subject"	=> $sub,
			"message"	=> $msg
		);
		$this->lib->call('send', $src, 'std', $message, 'OUT', $rep, false);
		$this->lib->call('send', $dst, 'std', $message, 'IN', $oriMsgId);
	}
}

?>

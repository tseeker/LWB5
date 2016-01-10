<?php

class beta5_msg_send {

	function beta5_msg_send($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Sends a message
	function run($player, $mType, $mData, $folder = 'INT', $replyTo = null, $isNew = true) {
		$moment = time();

		// Insert the message header
		$qs  = "INSERT INTO message(player,sent_on,mtype,ftype,is_new" . (is_null($replyTo) ? "" : ",reply_to")
			. ") VALUES ($player,$moment,'$mType','$folder'," . ($isNew ? 'TRUE' : 'FALSE') . (is_null($replyTo) ? "" : ",$replyTo") . ")";
		$id = $this->db->query($qs);
		if (!$id) {
			logText("beta5/msg/send: failed to send message to player $player", LOG_WARNING);
			return false;
		}

		if (is_array($mData[0])) {
			// It's an array of arrays, insert multiple entries
			foreach ($mData as $msg) {
				$this->insertData($mType, $id, $msg);
			}
		} else {
			$this->insertData($mType, $id, $mData);
		}

		return $id;
	}

	function insertData($mType, $id, $mData) {
		$fields = array_keys($mData);
		$qs = "INSERT INTO msg_$mType (id," . join(',', $fields) . ") VALUES ($id";
		foreach ($fields as $f) {
			if (is_null($mData[$f])) {
				$qs .= ",NULL";
			} else {
				$qs .= ",'" . addslashes($mData[$f]) . "'";
			}
		}
		$qs .= ")";
		$this->db->query($qs);
	}
}

?>

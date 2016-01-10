<?php

class beta5_msg_get {

	function beta5_msg_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns complete information about a message
	function run($mId, $pid) {
		$q = $this->db->query("SELECT * FROM message WHERE id=$mId AND NOT deleted");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}

		$a1 = dbFetchHash($q);
		$a = array(
			"id"		=> $a1['id'],
			"player"	=> $a1['player'],
			"ftype"		=> trim($a1['ftype']),
			"cfid"		=> $a1['fcustom'],
			"replyto"	=> $al['reply_to'],
			"received"	=> $a1['sent_on']
		);
		$new = ($a1['is_new'] == 't');
		if (!$new) {
			$q2 = $this->db->query("SELECT COUNT(*) <> 0 FROM message WHERE reply_to=$mId AND player=$pid");
			list($replied) = dbFetchArray($q2);
		}

		$q2 = $this->db->query("SELECT * FROM msg_".$a1['mtype']." WHERE id=$mId");
		if (!($q2 && dbCount($q2))) {
			return $a;
		}

		$lg = getLanguage();
		$f = $this->lib->mainClass->loadFormat($a1['mtype'], $lg, $pid);
		if (is_null($f)) {
			return $a;
		}

		if (dbCount($q2) == 1) {
			$f->data = dbFetchHash($q2);
		} else {
			$f->data = array();
			while ($r2 = dbFetchHash($q2))
				array_push($f->data, $r2);
		}
		$a["from"] =  $f->getSender();
		$a["to"] =  $f->getRecipient();
		$a["subject"] =  $f->getSubject();
		$a["slink"] =  $f->getSLink();
		$a["rlink"] =  $f->getRLink();
		$a["replink"] =  $f->getReplyLink();
		$a["text"] =  $f->getContents();
		$a["status"] = $new ? "N" : ($replied ? "R" : "r");
		return $a;
	}
}

?>

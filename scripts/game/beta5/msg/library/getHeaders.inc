<?php

class beta5_msg_getHeaders {

	function beta5_msg_getHeaders($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Get headers for all messages inside a folder
	function run($pid, $fld, $cfid) {
		$lg = getLanguage();
		$qs = "player=$pid AND NOT deleted AND ftype='$fld'";
		if ($fld == "CUS") {
			$qs .= " AND fcustom=$cfid";
		}
		$q = $this->db->query("SELECT id,sent_on,mtype,is_new,reply_to FROM message WHERE $qs");

		$a = array();
		while ($r = dbFetchArray($q)) {
			list($id,$mom,$type,$new,$rt) = $r;
			$new = ($new == 't');

			if (!$new) {
				$q2 = $this->db->query("SELECT COUNT(*) FROM message WHERE reply_to=$id AND player=$pid");
				list($replied) = dbFetchArray($q2);
			}

			$q2 = $this->db->query("SELECT * FROM msg_$type WHERE id=$id");
			if (!($q2 || dbCount($q2))) {
				continue;
			}

			$f = $this->lib->mainClass->loadFormat($type, $lg, $pid);
			if (is_null($f)) {
				continue;
			}

			if (dbCount($q2) == 1) {
				$f->data = dbFetchHash($q2);
			} else {
				$f->data = array();
				while ($r2 = dbFetchHash($q2))
					array_push($f->data, $r2);
			}
			$a[$id] = array(
				"received"	=> $mom,
				"from"		=> $f->getSender(),
				"to"		=> $f->getRecipient(),
				"subject"	=> $f->getSubject(),
				"slink"		=> $f->getSLink(),
				"rlink"		=> $f->getRLink(),
				"replink"	=> $f->getReplyLink(),
				"replyTo"	=> $rt,
				"status"	=> $new ? "N" : ($replied ? "R" : "r"),
			);
		}

		return $a;
	}
}

?>

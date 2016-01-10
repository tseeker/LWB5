<?php

class beta5_msg_sendToPlanet {

	function beta5_msg_sendToPlanet($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
	}


	// Sends a message from player to planet
	function run($src, $dst, $sub, $msg, $rep = null) {
		$moment = time();
		$pinf = $this->planets->call('byId', $dst);
		if (!is_null($rep)) {
			$q = $this->db->query("SELECT original FROM message WHERE player=$src AND id=$rep");
			if ($q && dbCount($q)) {
				list($oriMsgId) = dbFetchArray($q);
			} else {
				$rep = null;
			}
		}

		$qs  = "INSERT INTO message(player,sent_on,mtype,ftype,is_new";
		$qs .= (!is_null($rep)?",reply_to":"") . ") VALUES (";
		$qs .= "$src,$moment,'planet','OUT',FALSE" . (!is_null($rep)?",$rep":"") . ")";
		$this->db->query($qs);
		$q = $this->db->query("SELECT id FROM message WHERE player=$src AND sent_on=$moment AND ftype='OUT'");
		list($id) = dbFetchArray($q);
		$this->db->query(
			"INSERT INTO msg_planet VALUES($id,$dst,'".addslashes($pinf['name'])."',$src,"
			. "'".addslashes($sub)."','".addslashes($msg)."')"
		);

		if (!is_null($pinf['owner'])) {
			$qs  = "INSERT INTO message(player,sent_on,mtype,ftype,original";
			$qs .= (!is_null($rep)?",reply_to":"") . ") VALUES (";
			$qs .= $pinf['owner'].",$moment,'planet','IN',$id".(!is_null($rep)?",$oriMsgId":"").")";
			$this->db->query($qs);
			$q = $this->db->query("SELECT id FROM message WHERE player=".$pinf['owner']." AND sent_on=$moment AND ftype='IN'");
			list($id2) = dbFetchArray($q);
			$this->db->query(
				"INSERT INTO msg_planet VALUES($id2,$dst,'".addslashes($pinf['name'])."',$src,"
				. "'".addslashes($sub)."','".addslashes($msg)."')"
			);
		}
	}
}

?>

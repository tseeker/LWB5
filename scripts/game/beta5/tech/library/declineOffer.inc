<?php

class beta5_tech_declineOffer {

	function beta5_tech_declineOffer($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Declines a research offer
	function run($pid, $oid) {
		$q = $this->db->query("SELECT * FROM research_assistance WHERE offer_to=$pid AND id=$oid AND (UNIX_TIMESTAMP(NOW())-moment<=86400)");
		if (!($q && dbCount($q)==1)) {
			return 4;
		}

		$r = dbFetchHash($q);
		if (!is_null($r['accepted'])) {
			return $r['accepted'] == 't' ? 2 : 3;
		}

		$this->db->query("UPDATE research_assistance SET accepted=FALSE WHERE id=$oid");
		$tm = time();
		$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES(".$r['player'].",$tm,'resdipl','INT',TRUE)");
		$q = $this->db->query("SELECT id FROM message WHERE player=".$r['player']." AND sent_on=$tm AND ftype='INT'");
		list($mid) = dbFetchArray($q);
		$this->db->query("INSERT INTO msg_resdipl VALUES($mid,$oid,2)");

		return 0;
	}
}

?>

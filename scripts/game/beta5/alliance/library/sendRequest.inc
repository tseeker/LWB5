<?php

class beta5_alliance_sendRequest {

	function beta5_alliance_sendRequest($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Sends a request to join an alliance
	function run($p, $a) {
		$this->db->query("UPDATE player SET alliance=$a,a_status='REQ',a_vote=NULL WHERE id=$p");

		$ainf = $this->lib->call('get', $a);
		$tag = addslashes($ainf['tag']);
		$l = $this->lib->call('getKeepers', $a);
		$tm = time();

		foreach	($l as $id) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new) VALUES($id,$tm,'alint','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message WHERE player=$id AND sent_on=$tm AND ftype='INT'");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_alint VALUES($mid,$a,'$tag',$p,0)");
		}
	}
}

?>

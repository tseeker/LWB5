<?php

class beta5_tech_makeOffer {

	function beta5_tech_makeOffer($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->msg	= $this->lib->game->getLib('beta5/msg');
	}


	// Makes a research offer
	function run($cpid, $tpid, $tid, $pr) {
		$tm = time();
		$this->db->query(
			"INSERT INTO research_assistance(player,price,offer_to,moment" . (is_null($tid) ? "" : ",technology") . ")"
			. " VALUES($cpid,$pr,$tpid,$tm" . (is_null($tid) ? "" : ",$tid") . ")"
		);
		$q = $this->db->query("SELECT id FROM research_assistance WHERE player=$cpid AND moment=$tm");
		list($aid) = dbFetchArray($q);

		$this->msg->call('send', $tpid, 'resdipl', array(
			'offer'		=> $aid,
			'msg_id'	=> 0
		));
	}
}

?>

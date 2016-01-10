<?php

class beta5_sale_buy {
	var $index = array();


	function beta5_sale_buy($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
	}


	function run($pid, $offer) {
		$q = $this->db->query(
			"SELECT player,fleet,planet FROM sale "
			. "WHERE id=$offer AND player<>$pid AND finalized IS NULL AND "
			  . "(expires IS NULL OR expires>UNIX_TIMESTAMP(NOW()))"
		);
		if (!($q && dbCount($q))) {
			return "0";
		}
		list($seller,$flId,$plId) = dbFetchArray($q);

		$q = $this->db->query("SELECT price FROM public_offer WHERE offer=$offer AND NOT auction UNION SELECT price FROM private_offer WHERE offer=$offer");
		if (!($q && dbCount($q))) {
			return "0";
		}
		list($price) = dbFetchArray($q);

		$pi = $this->players->call('get', $pid);
		if ($pi['cash'] < $price) {
			return "1";
		}

		$tm = time();
		$this->db->query("UPDATE player SET cash=cash-$price WHERE id=$pid");
		$this->db->query("UPDATE player SET cash=cash+$price WHERE id=$seller");
		$this->db->query("UPDATE sale SET finalized=$tm,sold_to=$pid WHERE id=$offer");

		// Start owner transfer
		if (is_null($plId)) {
			$qs = "";
		} else {
			$this->db->query("UPDATE planet SET sale=3 WHERE id=$plId");
			$q = $this->db->query("SELECT pop,turrets,ifact+mfact FROM planet WHERE id=$plId");
			list($pop,$turrets,$fact) = dbFetchArray($q);
			$qs = ",p_pop=$pop,p_turrets=$turrets,p_factories=$fact";
		}
		if (!is_null($flId)) {
			$this->db->query("UPDATE fleet SET sale=3 WHERE id=$flId");
		}

		// Update history
		$this->db->query("UPDATE sale_history SET ended=$tm,end_mode=2,sell_price=$price,to_player=$pid$qs WHERE offer=$offer");

		// FIXME: send message
	}
}

?>

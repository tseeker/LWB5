<?php

class beta5_sale_cancelTransfer {
	var $index = array();


	function beta5_sale_cancelTransfer($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($pid,$id) {
		// Get sale details
		$q = $this->db->query("SELECT planet,fleet,sold_to FROM sale WHERE player=$pid AND id=$id AND finalized IS NOT NULL");
		if (!($q && dbCount($q))) {
			return false;
		}
		list($pl,$fl,$stid) = dbFetchArray($q);
		$q = $this->db->query("SELECT price FROM public_offer WHERE offer=$id UNION SELECT price FROM private_offer WHERE offer=$id");
		list($price) = dbFetchArray($q);

		// Get seller cash and find out if refund is possible
		$q = $this->db->query("SELECT cash FROM player WHERE id=$pid");
		list($cSeller) = dbFetchArray($q);
		if ($cSeller < $price) {
			return false;
		}

		// Refund
		$this->db->query("UPDATE player SET cash=cash+$price WHERE id=$stid");
		$this->db->query("UPDATE player SET cash=cash-$price WHERE id=$pid");

		// Cancel planet & fleet sale
		if (is_null($pl)) {
			$qs = "";
		} else {
			$this->db->query("UPDATE planet SET sale=NULL WHERE id=$pl");
			$q = $this->db->query("SELECT pop,turrets,ifact+mfact FROM planet WHERE id=$pl");
			list($pop,$turrets,$fact) = dbFetchArray($q);
			$qs = ",p_pop=$pop,p_turrets=$turrets,p_factories=$fact";
		}
		if (!is_null($fl)) {
			$this->db->query("UPDATE fleet SET sale=NULL WHERE id=$fl");
		}

		// Insert history entries
		$tm = time();
		$this->db->query("UPDATE sale_history SET end_mode=1,ended=$tm,sell_price=NULL$qs WHERE offer=$id");

		// Send message
		// FIXME

		// Delete offer
		$this->db->query("DELETE FROM sale WHERE id=$id");

		return true;
	}
}

?>

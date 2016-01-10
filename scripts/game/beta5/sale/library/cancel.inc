<?php

class beta5_sale_cancel {
	var $index = array();


	function beta5_sale_cancel($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($pid,$id) {
		$q = $this->db->query("SELECT planet FROM sale WHERE player=$pid AND id=$id AND finalized IS NULL");
		if (!($q && dbCount($q))) {
			return false;
		}
		list($pl) = dbFetchArray($q);

		// Refund auctions
		$q = $this->db->query("SELECT MAX(price),player FROM auction WHERE offer=$id GROUP BY player");
		while ($r = dbFetchArray($q)) {
			$this->db->query("UPDATE player SET cash=cash+".$r[0]." WHERE id=".$r[1]);
			// FIXME: send message
		}

		// Insert history entry
		if (is_null($pl)) {
			$qs = "";
		} else {
			$q = $this->db->query("SELECT pop,turrets,ifact+mfact FROM planet WHERE id=$pl");
			list($pop,$turrets,$fact) = dbFetchArray($q);
			$qs = ",p_pop=$pop,p_turrets=$turrets,p_factories=$fact";
		}
		$tm = time();
		$this->db->query("UPDATE sale_history SET end_mode=0,ended=$tm$qs WHERE offer=$id");

		// Delete offer
		$this->db->query("DELETE FROM sale WHERE id=$id");

		return true;
	}
}

?>

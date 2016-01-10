<?php

class beta5_sale_decline {
	var $index = array();


	function beta5_sale_decline($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($id) {
		$q = $this->db->query("SELECT planet FROM sale WHERE id=$id");
		if (!($q&&dbCount($q))) {
			return;
		}
		list($pl) = dbFetchArray($q);

		// Update history entry
		if (is_null($pl)) {
			$qs = "";
		} else {
			$q = $this->db->query("SELECT pop,turrets,ifact+mfact FROM planet WHERE id=$pl");
			list($pop,$turrets,$fact) = dbFetchArray($q);
			$qs = ",p_pop=$pop,p_turrets=$turrets,p_factories=$fact";
		}
		$tm = time();
		$this->db->query("UPDATE sale_history SET end_mode=4,ended=$tm$qs WHERE offer=$id");

		// Send message
		// FIXME

		// Delete offer
		$this->db->query("DELETE FROM sale WHERE id=$id");
	}
}

?>

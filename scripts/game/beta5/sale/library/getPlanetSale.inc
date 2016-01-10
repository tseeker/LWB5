<?php

class beta5_sale_getPlanetSale {
	var $index = array();


	function beta5_sale_getPlanetSale($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($id) {
		$q = $this->db->query("SELECT id,expires,finalized,sold_to FROM sale WHERE planet=$id");
		if (!($q && dbCount($q))) {
			return null;
		}
		$r = dbFetchHash($q);

		$q = $this->db->query("SELECT price FROM public_offer WHERE offer=".$r['id']);
		if ($q && dbCount($q)) {
			$r['public'] = true;
			list($r['price']) = dbFetchArray($q);
			return $r;
		}

		$q = $this->db->query("SELECT price,to_player FROM private_offer WHERE offer=".$r['id']);
		list($r['price'],$r['sold_to']) = dbFetchArray($q);
		return $r;
	}
}

?>

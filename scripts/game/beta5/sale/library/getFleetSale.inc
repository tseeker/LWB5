<?php

class beta5_sale_getFleetSale {
	var $index = array();


	function beta5_sale_getFleetSale($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($sId) {
		$q = $this->db->query("SELECT id,player,expires,finalized,sold_to,fleet FROM sale WHERE id=$sId AND planet IS NULL AND fleet IS NOT NULL");
		if (!($q && dbCount($q))) {
			return null;
		}
		$r = dbFetchHash($q);

		$q = $this->db->query("SELECT sale FROM fleet WHERE id={$r['fleet']}");
		if (!($q && dbCount($q)))
			return null;
		list($r['tx_time']) = dbFetchArray($q);

		$q = $this->db->query("SELECT price,auction FROM public_offer WHERE offer=".$r['id']);
		if ($q && dbCount($q)) {
			$r['public'] = true;
			list($r['price'],$r['is_auction']) = dbFetchArray($q);
			$r['is_auction'] = ($r['is_auction'] == 't');
			if ($r['is_auction']) {
				$q = $this->db->query("SELECT MAX(price) FROM auction WHERE offer={$r['id']}");
				if ($q && dbCount($q)) {
					list($r['max_bid']) = dbFetchArray($q);
					if (!is_null($r['max_bid'])) {
						$q = $this->db->query("SELECT player,moment FROM auction WHERE offer={$r['id']} AND price={$r['max_bid']}");
						list($r['bidder'],$r['last_bid']) = dbFetchArray($q);
					}
				}
			}
			return $r;
		}

		$q = $this->db->query("SELECT price,to_player FROM private_offer WHERE offer=".$r['id']);
		list($r['price'],$r['sold_to']) = dbFetchArray($q);
		return $r;
	}
}

?>

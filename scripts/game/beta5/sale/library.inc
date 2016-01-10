<?php

class beta5_sale_library {
	var $index = array(
		'bid',
		'buy',
		'cancel',
		'cancelTransfer',
		'decline',
		'getDirectSales',
		'getFleetSale',
		'getPlanetSale',
		'getPublicSales',
		'getSentOffers',
		'sell'
	);


	function beta5_sale_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function getHistoryFrom($player) {
		$q = $this->db->query("SELECT * FROM sale_history WHERE from_player=$player AND end_mode IS NOT NULL ORDER BY ended DESC,started DESC");
		$rs = array();
		while ($r = dbFetchHash($q)) {
			array_push($rs,$r);
		}
		return $rs;
	}


	function getHistoryTo($player) {
		$q = $this->db->query("SELECT * FROM sale_history WHERE to_player=$player AND mode<2 AND end_mode IS NOT NULL ORDER BY ended DESC,started DESC");
		$rs = array();
		while ($r = dbFetchHash($q)) {
			array_push($rs,$r);
		}
		return $rs;
	}


	function isDirectOffer($pid, $oid) {
		$q = $this->db->query(
			"SELECT id FROM sale,private_offer WHERE offer=id AND to_player=$pid AND id=$oid "
			. "AND (expires IS NULL OR UNIX_TIMESTAMP(NOW())-expires<0) AND finalized IS NULL"
		);
		return $q && (dbCount($q)==1);
	}
}

?>

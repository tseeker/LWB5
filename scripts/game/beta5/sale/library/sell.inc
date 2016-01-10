<?php

class beta5_sale_sell {
	var $index = array();


	function beta5_sale_sell($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
		$this->fleets	= $this->lib->game->getLib('beta5/fleet');
	}


	function run($player, $public, $auction, $expires, $price, $target, $planet, $fleet) {
		// Insert sale entry
		$tm = time();
		$exp = ($expires > 0) ? ($tm + 3600 * $expires) : 'NULL';
		$pl = is_null($planet)?'NULL':$planet;
		$plq = is_null($planet)?' IS NULL':"=$planet";
		$fl = is_null($fleet)?'NULL':$fleet;
		$flq = is_null($fleet)?' IS NULL':"=$fleet";
		$this->db->query("INSERT INTO sale(player,started,expires,planet,fleet) VALUES ($player,$tm,$exp,$pl,$fl)");

		// Insert offer
		$q = $this->db->query("SELECT id FROM sale WHERE player=$player AND started=$tm AND planet$plq AND fleet$flq");
		list($sId) = dbFetchArray($q);
		if ($public) {
			$this->db->query("INSERT INTO public_offer VALUES($sId,$price,".dbBool($auction).")");
		} else {
			$this->db->query("INSERT INTO private_offer VALUES($sId,$target,$price)");
		}

		// Insert history entry
		$mode = $public ? ($auction ? 3 : 2) : ($price > 0 ? 1 : 0);
		$toPl = $public ? "NULL" : $target;
		if (is_null($planet)) {
			$finf = $this->fleets->call('get', $fleet);
			$pinf = $this->planets->call('byId', $finf['location']);
			$pName = addslashes($pinf['name']);
			$pid = $finf['location'];
		} else {
			$pinf = $this->planets->call('byId', $planet);
			$pid = $planet;
			$pName = addslashes($pinf['name']);
		}
		if (is_null($fleet)) {
			$flFields = $flValues = "";
		} else {
			if (is_null($finf)) {
				$finf = $this->fleets->call('get', $fleet);
			}
			$flFields = ",f_gaships,f_fighters,f_cruisers,f_bcruisers";
			$flValues = ",".$finf['gaships'].",".$finf['fighters'].",".$finf['cruisers'].",".$finf['bcruisers'];
		}
		$this->db->query(
			"INSERT INTO sale_history(offer,from_player,to_player,started,mode,price,p_id,p_name,is_planet$flFields) VALUES"
			. "($sId,$player,$toPl,$tm,$mode,$price,$pid,'$pName',".dbBool(!is_null($planet))."$flValues)"
		);

		// FIXME: send messages

		if (!is_null($fleet)) {
			$this->fleets->call('invCache', $fleet);
		}
	}
}

?>

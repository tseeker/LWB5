<?php

class beta5_sale_getSentOffers {
	var $index = array();


	function beta5_sale_getSentOffers($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($pid) {
		$q = $this->db->query(
			"SELECT * FROM sale WHERE player=$pid AND finalized IS NULL "
				. "AND (expires IS NULL OR expires>UNIX_TIMESTAMP(NOW()))"
		);

		$oList = $pList = $fList = array();
		while ($r = dbFetchHash($q)) {
			$oList[$r['id']] = $r;
			if (!is_null($r['planet'])) {
				$pList[$r['planet']] = $r['id'];
			}
			if (!is_null($r['fleet'])) {
				$fList[$r['fleet']] = $r['id'];
			}
		}

		$l1 = join(',',array_keys($oList));
		$aList = array();
		if ($l1 != '') {
			$q = $this->db->query("SELECT * FROM public_offer WHERE offer IN ($l1)");
			while ($r = dbFetchArray($q)) {
				$oList[$r[0]]['mode'] = ($r[2] == 1 ? 3 : 2);
				$oList[$r[0]]['price'] = $r[1];
				if ($r[2])
					array_push($aList, $r[0]);
					// FIXME: to_player for auctions
			}

			$q = $this->db->query("SELECT * FROM private_offer WHERE offer IN ($l1)");
			while ($r = dbFetchArray($q)) {
				$oList[$r[0]]['mode'] = ($r[2] == 0 ? 0 : 1);
				$oList[$r[0]]['to_player'] = $r[1];
				$oList[$r[0]]['price'] = $r[2];
			}
		}

		$l1 = join(',', $aList);
		if ($l1 != '') {
			$q = $this->db->query("SELECT offer,MAX(price) FROM auction WHERE offer IN ($l1) GROUP BY offer");
			while ($r = dbFetchArray($q)) {
				$oList[$r[0]]['price'] = $r[1];
			}
		}

		$l1 = join(',',array_keys($pList));
		if ($l1 != '') {
			$q = $this->db->query(
				"SELECT p.id AS id,pop AS pop,p.turrets AS turrets,p.mfact+p.ifact AS fact,p.orbit AS orbit,s.x AS x,s.y AS y,p.name AS name "
				. "FROM planet p,system s WHERE p.system=s.id AND p.id IN ($l1)"
			);
			while ($r = dbFetchHash($q)) {
				$oList[$pList[$r['id']]]['planet'] = $r;
			}
		}

		$l1 = join(',',array_keys($fList));
		if ($l1 != '') {
			$q = $this->db->query(
				"SELECT f.id AS id,f.gaships AS sg,f.fighters AS sf,f.cruisers AS sc,f.bcruisers AS sb,"
				."p.id AS pid,p.name AS pname,s.x AS x,s.y AS y,p.orbit AS orbit "
				. "FROM fleet f,planet p,system s WHERE f.id IN ($l1) AND p.id=f.location AND p.system=s.id"
			);
			while ($r = dbFetchHash($q)) {
				$oList[$fList[$r['id']]]['fleet'] = $r;
			}
		}

		return array_values($oList);
	}
}

?>

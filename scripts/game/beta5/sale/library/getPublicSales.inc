<?php

class beta5_sale_getPublicSales {
	var $index = array();


	function beta5_sale_getPublicSales($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($pList) {
		$l1 = join(',',$pList);

		$q = $this->db->query("SELECT id FROM fleet WHERE location IN ($l1)");
		$fList = array();
		while ($r = dbFetchArray($q)) {
			array_push($fList, $r[0]);
		}
		$l2 = join(',',$fList);

		$q = $this->db->query(
			"SELECT * FROM sale,public_offer "
			. "WHERE finalized IS NULL AND offer=id AND (expires IS NULL OR expires>UNIX_TIMESTAMP(NOW())) "
			. "AND (planet IN ($l1)" . ($l2 != '' ? " OR fleet IN ($l2)" : "") . ")"
		);
		$aList = $oList = $pList = $fList = array();
		while ($r = dbFetchHash($q)) {
			$oList[$r['id']] = $r;
			if (!is_null($r['planet'])) {
				$pList[$r['planet']] = $r['id'];
			}
			if (!is_null($r['fleet'])) {
				$fList[$r['fleet']] = $r['id'];
			}
			if ($r['auction'] == 't') {
				$aList[$r['id']] = $r['price'];
			}
		}

		$l1 = join(',',array_keys($aList));
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

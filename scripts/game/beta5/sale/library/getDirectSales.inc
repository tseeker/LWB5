<?php

class beta5_sale_getDirectSales {
	var $index = array();


	function beta5_sale_getDirectSales($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($pid) {
		$q = $this->db->query(
			"SELECT * FROM sale,private_offer WHERE id=offer AND to_player=$pid AND finalized IS NULL AND offer=id "
				. "AND (expires IS NULL OR expires>UNIX_TIMESTAMP(NOW())) ORDER BY started DESC"
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
			while ($r = dbFetchHash($q))
				$oList[$fList[$r['id']]]['fleet'] = $r;
		}

		return array_values($oList);
	}
}

?>

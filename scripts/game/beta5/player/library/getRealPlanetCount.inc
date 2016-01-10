<?php

class beta5_player_getRealPlanetCount {

	function beta5_player_getRealPlanetCount($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($id) {
		$c = $this->lib->call('getPlanetCount', $id);
		$q = $this->db->query("SELECT COUNT(*) FROM sale WHERE player=$id AND planet IS NOT NULL");
		list($scount) = dbFetchArray($q);
		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE owner=$id AND (abandon<>0 OR bh_prep<>0)");
		list($acount) = dbFetchArray($q);
		return $c - ($scount + $acount);
	}
}

?>

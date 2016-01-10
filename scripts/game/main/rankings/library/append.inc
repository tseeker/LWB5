<?php

class main_rankings_append {

	function main_rankings_append($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($type, $id, $additional = null) {
		$q = $this->db->query("SELECT ranking FROM ranking WHERE r_type=$type AND points=0");
		if ($q && dbCount($q)) {
			list($rk) = dbFetchArray($q);
		} else {
			$q = $this->db->query("SELECT MAX(ranking) FROM ranking WHERE r_type=$type");
			list($mrk) = dbFetchArray($q);
			if (is_null($mrk)) {
				$rk = 1;
			} else {
				$q = $this->db->query("SELECT COUNT(*) FROM ranking WHERE r_type=$type AND ranking=$mrk");
				list($nrk) = dbFetchArray($q);
				$rk = $mrk + $nrk;
			}
		}
		$this->db->query("INSERT INTO ranking(r_type,id,additional,points,ranking) VALUES("
			. "$type,'" . addslashes($id) . "'," . (is_null($additional) ? "NULL" : ("'" . addslashes($additional) . "'")) . ",0,$rk)");
	}
}

?>

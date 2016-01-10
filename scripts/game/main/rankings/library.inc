<?php

class main_rankings_library {
	var $index	= array(
		'append',
		'delete',
		'getAll',
		'update'
	);

	function main_rankings_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function getType($identifier) {
		$version = $this->lib->game->version->id;
		$name = $this->lib->game->name;

		$q = $this->db->query("SELECT g.id FROM ranking_def d, ranking_game g "
			. "WHERE d.version='$version' AND d.name='$identifier' AND g.ranking=d.id AND g.game='$name'");
		list($id) = dbFetchArray($q);
		return $id;
	}

	function getTypes() {
		$game = $this->lib->game;
		$version = $game->version;

		$q = $this->db->query("SELECT g.id,d.name FROM ranking_def d, ranking_game g "
			. "WHERE d.version='{$version->id}' AND g.game='{$game->name}' AND g.ranking=d.id");
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[1]] = $r[0];
		}
		return $rs;
	}

	function getText($id, $lang) {
		$q = $this->db->query("SELECT t.name AS name,t.description AS description "
			. "FROM ranking_text t, ranking_game g "
			. "WHERE g.id=$id AND t.ranking=g.ranking AND t.lang='$lang'");
		return dbFetchHash($q);
	}

	function get($type, $id) {
		$q = $this->db->query("SELECT points,ranking FROM ranking WHERE r_type=$type AND LOWER(id)='".addslashes(strtolower($id))."' ORDER BY ranking,id");
		return dbFetchHash($q);
	}
}

?>

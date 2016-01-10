<?php

class beta5_tech_getTopicData {

	function beta5_tech_getTopicData($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Returns data about a research topic: identifier, title, cost, description
	function run($lang, $id) {
		$qs  = "SELECT r.cost,d.name,d.description,r.category FROM research r,research_txt d ";
		$qs .= "WHERE r.id=$id AND d.research=r.id AND d.lang='$lang'";
		$q = $this->db->query($qs);
		if (!($q && dbCount($q) == 1)) {
			return "";
		}

		$r = dbFetchArray($q);
		$str = "$id\n" . utf8entities($r[1]) . "\n" . $r[0] . "#" . $r[3] . "\n";
		$str .= preg_replace('/\n/', '<br/>', utf8entities($r[2]));
		return $str;
	}
}

?>

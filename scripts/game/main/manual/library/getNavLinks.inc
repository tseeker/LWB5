<?php

class main_manual_getNavLinks {

	function main_manual_getNavLinks($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($pageId) {
		// Get the version, language and parent section for the page
		$q = $this->db->query("SELECT version,lang,in_section,after_section FROM man_section WHERE id=$pageId");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($version,$lang,$parent,$after) = dbFetchArray($q);

		// Get the manual's root node identifier
		$q = $this->db->query("SELECT id FROM man_section WHERE version='$version' AND lang='$lang' AND in_section IS NULL");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($rootId) = dbFetchArray($q);

		// Get the root node's first page
		$q = $this->db->query("SELECT id FROM man_section WHERE in_section=$rootId AND after_section=$rootId AND is_page");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($fPage) = dbFetchArray($q);

		// Get identifiers
		$rv = array($pageId == $fPage ? null : $fPage);
		$rv[1] = ($parent == $rootId) ? (is_null($rv[0]) ? null : $rv[0]) : $parent;
		$rv[2] = ($after == $rootId) ? null : $after;
		$q = $this->db->query("SELECT id FROM man_section WHERE is_page AND after_section=$pageId");
		if ($q && dbCount($q) == 1) {
			list($rv[3]) = dbFetchArray($q);
		} else {
			$rv[3] = null;
		}

		// Get names
		$nnl = array();
		for ($i=0;$i<4;$i++) {
			if (!is_null($rv[$i])) {
				array_push($nnl, $rv[$i]);
			}
		}
		if (count($nnl)) {
			$q = $this->db->query("SELECT id,name FROM man_section WHERE id IN (" . join(',', array_unique($nnl)) . ")");
			$l = array();
			while ($r = dbFetchArray($q)) {
				$l[$r[0]] = $r[1];
			}
			for ($i=0;$i<4;$i++) {
				if (!is_null($rv[$i])) {
					$rv[$i] = $l[$rv[$i]];
				}
			}
		}

		return $rv;
	}
}

?>

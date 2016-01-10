<?php

class main_manual_getPageId {

	function main_manual_getPageId($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($secId) {
		// Find the section itself
		$q = $this->db->query("SELECT is_page,in_section FROM man_section WHERE id=$secId");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($isPage, $parentId) = dbFetchArray($q);

		// Find the page in which the section is
		while ($isPage != 't') {
			$q = $this->db->query("SELECT id,is_page,in_section FROM man_section WHERE id=$parentId");
			if (!($q && dbCount($q) == 1)) {
				return null;
			}
			list($secId, $isPage, $parentId) = dbFetchArray($q);
		}

		return $secId;
	}
}

?>

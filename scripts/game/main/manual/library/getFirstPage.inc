<?php

class main_manual_getFirstPage {

	function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->version	= $this->lib->mainClass->version;
	}

	function run($lang) {
		// Get the manual's root node identifier
		$q = $this->db->query("SELECT id FROM man_section WHERE version='{$this->version}' AND lang='$lang' AND in_section IS NULL");
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

		return $fPage;
	}
}

?>

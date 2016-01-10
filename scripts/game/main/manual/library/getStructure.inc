<?php

class main_manual_getStructure {

	function main_manual_getStructure($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->version	= $this->lib->mainClass->version;
	}

	function run($lang) {
		// Get the manual's root node identifier
		$q = $this->db->query("SELECT id FROM man_section WHERE version='{$this->version}' AND lang='$lang' AND in_section IS NULL");
		if (!($q && dbCount($q) == 1)) {
			return array();
		}
		list($rootId) = dbFetchArray($q);

		// List sections
		return $this->lib->call('getSectionsIn', $rootId, 2);
	}
}

?>

<?php

class main_manual_library {
	var $index	= array(
		'getFirstPage',
		'getNavLinks',
		'getPageId',
		'getSectionsIn',
		'getStructure',
		'readXMLFile',
		'search',
		'updateSections'
	);

	function main_manual_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->version	= $this->lib->game->version->id;
	}


	function getPage($pageId) {
		$section = $this->readSectionRecord($pageId);
		$tree = $this->lib->call('getSectionsIn', $pageId, 0);
		$this->readSubSections($pageId, $section, $tree);
		return $section;
	}


	function getSectionId($lang, $name) {
		$q = $this->db->query("SELECT id FROM man_section WHERE version='{$this->version}' AND lang='$lang' AND name='" . addslashes($name) . "'");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($secId) = dbFetchArray($q);
		return $secId;
	}


	function readSectionRecord($sectionId) {
		$q = dbQuery("SELECT name,title,contents,link_to FROM man_section WHERE id=$sectionId");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($name,$title,$contents,$linkto) = dbFetchArray($q);
		return array(
			"id"		=> $sectionId,
			"name"		=> $name,
			"title"		=> $title,
			"contents"	=> $contents,
			"linkto"	=> $linkto,
			"subsections"	=> array()
		);
	}


	function readSubSections($sectionId, &$section, &$subList) {
		foreach ($subList as $secId => $sData) {
			$nSec = $this->readSectionRecord($secId);
			$this->readSubSections($secId, $nSec, $subList[$secId]['subs']);
			array_push($section['subsections'], $nSec);
		}
	}
}

?>

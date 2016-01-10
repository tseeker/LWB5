<?php

class page_handler {
	var $noTracking	= true;
	var $needsAuth	= false;
	var $ajax	= array();

	var $lang	= null;
	var $version	= null;
	var $page	= null;
	var $searchText	= "";

	function pageNotFound() {
		$this->page = null;
		$this->output = "manual";
	}

	function getFirstPage() {
		$fPage = $this->lib->call('getFirstPage', $this->lang);
		if (is_null($fPage)) {
			$this->pageNotFound();
			return;
		}

		$this->page = $this->lib->call('getPage', $fPage);
		if (is_null($this->page)) {
			$this->pageNotFound();
			return;
		}

		$this->output = "manual";
	}

	function getPage($name) {
		$secId = $this->lib->call('getSectionId', $this->lang, $name);
		if (is_null($secId)) {
			$this->pageNotFound();
			return;
		}

		$pageId = $this->lib->call('getPageId', $secId);
		if (is_null($pageId)) {
			$this->pageNotFound();
			return;
		}

		$this->page = $this->lib->call('getPage', $pageId);
		if (is_null($this->page)) {
			$this->pageNotFound();
			return;
		}

		$this->output = "manual";
	}

	function getSearchPage($text) {
		$this->searchText = $text;

		if (is_array(tracking::$data['man_search']) && tracking::$data['man_search']['text'] != $text) {
			tracking::$data['man_search'] = null;
		}
		if (!is_array(tracking::$data['man_search'])) {
			tracking::$data['man_search'] = array(
				"text"		=> $text,
				"results"	=> $this->lib->call('search', $text, $this->lang)
			);
		}
		$this->data = tracking::$data['man_search'];
		$this->output = "manual_search";
	}

	function handle($input) {
		$game = config::getDefaultGame();
		$this->lib = $game->getLib('main/manual');
		$this->lang = getLanguage();

		if ($input['ss'] != '') {
			$this->getSearchPage($input['ss']);
		} else {
			if ($input['p'] != '') {
				$p = preg_replace('/[^A-Za-z0-9_\-]/', '', $input['p']);
				$this->getPage($p);
			} else {
				$this->getFirstPage();
			}
			if (is_array(tracking::$data['man_search'])) {
				$this->searchText = tracking::$data['man_search']['text'];
			} else {
				$this->searchText = "";
			}
		}
	}
}

?>

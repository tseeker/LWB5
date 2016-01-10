<?php

class page_handler {
	var $needsAuth = true;
	var $ajax = array();
	var $lang = null;
	var $lib = null;
	var $page = null;
	var $searchText = "";

	function pageNotFound() {
		$this->subPage = "notfound";
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

		$this->subPage = "page";
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

		$this->subPage = "page";
	}

	function getSearchPage($text) {
		$this->searchText = $text;

		if (is_array(tracking::$data['man_search']) && tracking::$data['man_search']['text'] != $text) {
			tracking::$data['man_search'] = null;
		}
		if (!is_array(tracking::$data['man_search'])) {
			tracking::$data['man_search'] = array(
				"text"		=> $text,
				"results"	=> $this->lib->call('search', $text, $this->lang, $this->version)
			);
		}
		$this->data = tracking::$data['man_search'];
		$this->subPage = "search";
	}

	function handle($input) {
		$this->lang = getLanguage();
		$this->lib = $this->game->getLib('main/manual');

		if ($input['ss'] != '') {
			$this->getSearchPage($input['ss']);
		} elseif ($input['p'] != '') {
			$p = preg_replace('/[^A-Za-z0-9_\-]/', '', $input['p']);
			$this->getPage($p);
		} else {
			$this->getFirstPage();
		}
		$this->output = "manual";
	}
}

?>

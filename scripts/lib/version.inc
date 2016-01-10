<?php

/****************************************************************************
 * VERSION MANAGEMENT
 ****************************************************************************/

class version {

	function __construct($id, $playable, $text) {
		$this->id	= $id;
		$this->playable	= $playable;
		$this->text	= $text;
		$this->tickDefs	= array();
	}

	function addTickDefinition($def) {
		if (is_null($this->tickDefs[$def->script])) {
			$this->tickDefs[$def->script] = $def;
		}
	}

	function getTickDefinition($id) {
		return $this->tickDefs[$id];
	}

	function getDirectory() {
		return config::$main['scriptdir'] . "/game/{$this->id}";
	}

	function getSiteDirectory() {
		return config::$main['scriptdir'] . "/site/{$this->id}";
	}

	function loadActionsClass() {
		$cn = "actions_{$this->id}";
		loader::load($this->getDirectory() . "/actions.inc", $cn);
		return $cn;
	}

	function loadAction($action) {
		$cn = "{$this->id}_{$action}";
		loader::load($this->getDirectory() . "/actions/$action.inc", $cn);

		return $cn;
	}
}

?>

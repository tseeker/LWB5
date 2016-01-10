<?php

class beta5_rules_get {
	function beta5_rules_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Loads rules for a player
	function run($pid) {
		$apid = is_null($pid) ? ' ' : $pid;
		if (is_array($this->lib->mainClass->rules[$apid])) {
			return $this->lib->mainClass->rules[$apid];
		}

		$q = $this->db->query("SELECT name,value FROM rule"
			. (is_null($pid) ? "_def" : " WHERE player=$pid"));
		if (!($q && dbCount($q))) {
			return null;
		}

		$this->lib->mainClass->rules[$apid] = array();
		while ($r = dbFetchArray($q)) {
			$this->lib->mainClass->rules[$apid][$r[0]] = $r[1];
		}

		return $this->lib->mainClass->rules[$apid];
	}
}

?>

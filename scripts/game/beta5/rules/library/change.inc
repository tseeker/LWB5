<?php

class beta5_rules_change {

	function beta5_rules_change($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($pid, $rules) {
		if (!count($rules)) {
			return;
		}

		foreach ($rules as $r => $v) {
			$this->db->query("UPDATE rule SET value=value+($v) WHERE player=$pid AND name='$r'");
		}

		$this->lib->mainClass->rules[$pl] = null;

		$q = $this->db->query("SELECT h.handler,r.value FROM rule_handler h,rule r "
			. "WHERE r.name IN ('" . join("','", array_keys($rules)) . "') AND r.player=$pid AND h.rule=r.name");
		while ($r = dbFetchArray($q)) {
			$this->lib->call("rh{$r[0]}", $pid, $r[1]);
		}
	}
}

?>

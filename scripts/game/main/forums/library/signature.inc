<?php

class main_forums_signature {
	var $signatures	= array();

	function main_forums_signature($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($u) {
		if (is_array($this->signatures)) {
			if ($this->signatures[$u] != "")
				return $this->signatures[$u];
		} else {
			$this->signatures = array();
		}
		$q = $this->db->query(
			"SELECT id,value FROM user_preferences "
			. "WHERE account IN (0,$u) "
				. "AND id IN ('forums_sig','forum_code','smileys') "
				. "AND version='main' "
			. "ORDER BY account"
		);
		$p = array();
		while ($r = dbFetchArray($q)) {
			$p[$r[0]] = $r[1];
		}
		if ($p["forums_sig"] == "") {
			$s = "";
		} else {
			$s = "<div class='fsig'><hr/>" . $this->lib->call('substitute', $p['forums_sig'], $p['forum_code'] ? 't' : 'f', $p['smileys'] ? 't' : 'f') . "</div>";
		}
		return ($this->signatures[$u] = $s);
	}
}

?>

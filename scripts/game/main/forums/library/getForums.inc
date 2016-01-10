<?php

class main_forums_getForums {

	function main_forums_getForums($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($c) {
		$q = $this->db->query("SELECT * FROM f_forum WHERE category=$c ORDER BY forder ASC");
		$a = array();
		if (!$q) {
			return $a;
		}
		while ($rs = dbFetchHash($q)) {
			if ($rs['last_post'] != "") {
				$q2 = $this->db->query(
					"SELECT u.name AS author,p.moment AS moment "
					. "FROM f_post p,account u "
					. "WHERE p.id=".$rs['last_post']." AND u.id=p.author"
				);
				$rs['last'] = dbFetchHash($q2);
			} else {
				$rs['last'] = null;
			}
			$rs['user_post'] = ($rs['user_post'] == 't');
			$rs['admin_only'] = ($rs['admin_only'] == 't');
			array_push($a, $rs);
		}
		return $a;
	}
}

?>

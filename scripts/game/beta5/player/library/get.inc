<?php

class beta5_player_get {

	function beta5_player_get($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($id, $quitOk = false) {
		if (is_null($id)) {
			l::warn("****** BUG: beta5::player::get(null)");
			if (config::$main['debug'] == 2) {
				l::backtrace();
			}
			return null;
		}

		if (is_array($this->lib->mainClass->players[$id])) {
			if (!$quitOk && $this->lib->mainClass->players[$id]["quit"] == "1") {
				return null;
			}
			return $this->lib->mainClass->players[$id];
		}

		$s = $quitOk ? "" : "AND (p.quit IS NULL OR UNIX_TIMESTAMP(NOW()) - p.quit < 86400)";
		$q = $this->db->query(
			"SELECT u.id,u.name,p.name,p.cash,p.alliance,TRIM(p.a_status),p.a_vote,p.a_grade,"
			. "(p.quit IS NOT NULL AND UNIX_TIMESTAMP(NOW()) - p.quit >= 86400),p.quit,"
			. "(u.status='VAC') "
			. "FROM account u, player p "
			. "WHERE p.id=$id $s AND u.id=p.userid"
		);
		if (!($q && dbCount($q))) {
			return null;
		}
		$a = dbFetchArray($q);
		$pinf = array(
			"uid"	=> $a[0],
			"pid"	=> $id,
			"name"	=> ($a[2] != "")?$a[2]:$a[1],
			"cash"	=> $a[3],
			"quit"	=> $a[8] == 't',
			"qts"	=> $a[9],
			"vac"	=> ($a[10] == 't'),
		);

		if (is_null($this->lib->mainClass->pNames[$id])) {
			$this->lib->mainClass->pNames[$id] = $pinf['name'];
		}

		if ($a[5] == "IN") {
			$q = $this->db->query("SELECT tag, name FROM alliance WHERE id=".$a[4]);
			$pinf['aid'] = $a[4];
			$pinf['vote'] = $a[6];
			$pinf['a_grade'] = $a[7];
			list($pinf['alliance'], $pinf['aname']) = dbFetchArray($q);
		} elseif ($a[5] == "REQ") {
			$q = $this->db->query("SELECT tag, name FROM alliance WHERE id=".$a[4]);
			$pinf['arid'] = $a[4];
			list($pinf['alliance_req'], $pinf['aname']) = dbFetchArray($q);
		}

		$q = $this->db->query("SELECT COUNT(*) FROM planet WHERE owner=$id");
		list($pinf["planets"]) = dbFetchArray($q);

		$this->lib->mainClass->players[$id] = $pinf;
		return $pinf;
	}
}

?>

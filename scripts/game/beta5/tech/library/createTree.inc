<?php

class beta5_tech_createTree {

	function beta5_tech_createTree($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
	}


	function run($player) {
		list($res, $rLeaf) = $this->lib->call('getTree');

		$pRes = array();
		foreach	($res as $r) {
			if ($r['optional'] == 0) {
				array_push($pRes, $r['id']);
			}
		}

		while (count($pRes) < 2 * count($res) / 5) {
			$tried = array();
			do {
				do {
					$rId = $rLeaf[rand(0, count($rLeaf) - 1)];
				} while (in_array($rId, $pRes) || in_array($rId, $tried));

				$npRes = $pRes;
				array_push($tried, $rId);
				array_push($npRes, $rId);
				$mDeps = $this->lib->mainClass->getAllDependencies($res,$rId);
				foreach	($mDeps as $d) {
					if (!in_array($d, $npRes)) {
						array_push($npRes, $d);
					}
				}
			} while	(count($npRes) > 3 * count($res) / 5);
			$pRes = $npRes;
		}

		foreach	($pRes as $rid) {
			$this->db->query("INSERT INTO research_player(player,research,possible) VALUES($player,$rid,TRUE)");
		}
	}
}

?>

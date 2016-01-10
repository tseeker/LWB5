<?php

class beta5_forums_getLatest {

	function beta5_forums_getLatest($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->mForums	= $this->lib->game->getLib('main/forums');
	}


	function run($aForums, $gForums, $nb, $first) {
		if (count($aForums) && count($gForums)) {
			$qs = "SELECT 'A',id,moment FROM af_post WHERE forum IN (".join(',',$aForums).")";
			$qs .= "UNION SELECT 'G',id,moment FROM f_post WHERE forum IN (".join(',',$gForums).") AND deleted IS NULL ";
		} elseif (count($aForums)) {
			$qs = "SELECT 'A',id,moment FROM af_post WHERE forum IN (".join(',',$aForums).") ";
		} elseif (count($gForums)) {
			$qs = "SELECT 'G',id,moment FROM f_post WHERE forum IN (".join(',',$gForums).") AND deleted IS NULL ";
		} else {
			return array();
		}
		$qs .= "ORDER BY moment DESC LIMIT $nb OFFSET $first";

		$q = $this->db->query($qs);
		$posts = array();
		while ($r = dbFetchArray($q)) {
			if ($r[0] == 'A') {
				$p = $this->lib->call('getPost', $r[1]);
				$p['contents'] = $p['html'];
			} else {
				$p = $this->mForums->call('getPost',$r[1]);
				$p['contents'] = $p['html'];
			}
			$p['ctype'] = $r[0];
			array_push($posts, $p);
		}

		return $posts;
	}
}

?>

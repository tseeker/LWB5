<?php

class beta5_forums_searchPosts {

	function beta5_forums_searchPosts($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->mForums	= $this->lib->game->getLib('main/forums');
	}

	function run($aForums, $gForums, $nb, $first, $text, $complete, $sField, $sOrder) {
		$tQs = " AND (title ILIKE '$text'" . ($complete ? " OR contents ILIKE '$text'" : "") . ")";
		if (count($aForums) && count($gForums)) {
			$qs = "SELECT 'A',id,$sField FROM af_post WHERE forum IN (".join(',',$aForums).") $tQs ";
			$qs .= "UNION SELECT 'G',id,$sField FROM f_post WHERE forum IN (".join(',',$gForums).") AND deleted IS NULL $tQs ";
		} else if (count($aForums)) {
			$qs = "SELECT 'A',id,$sField FROM af_post WHERE forum IN (".join(',',$aForums).") $tQs ";
		} else {
			$qs = "SELECT 'G',id,$sField FROM f_post WHERE forum IN (".join(',',$gForums).") AND deleted IS NULL $tQs ";
		}
		$qs .= "ORDER BY $sField $sOrder LIMIT $nb OFFSET $first";

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

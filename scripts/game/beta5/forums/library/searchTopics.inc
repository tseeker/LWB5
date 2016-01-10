<?php

class beta5_forums_searchTopics {

	function beta5_forums_searchTopics($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->mForums	= $this->lib->game->getLib('main/forums');
	}

	function run($aForums, $gForums, $nb, $first, $text, $complete, $sField, $sOrder) {
		$tQs = "AND (title ILIKE '$text'" . ($complete ? " OR contents ILIKE '$text'" : "") . ")";
		if (count($aForums) && count($gForums)) {
			$qs = "SELECT 'A',topic,MAX($sField) AS $sField FROM af_post WHERE forum IN (".join(',',$aForums).") $tQs GROUP BY topic ";
			$qs .= "UNION SELECT 'G',topic,MAX($sField) AS $sField FROM f_post WHERE forum IN (".join(',',$gForums).") $tQs GROUP BY topic ";
		} else if (count($aForums)) {
			$qs = "SELECT 'A',topic,MAX($sField) AS $sField FROM af_post WHERE forum IN (".join(',',$aForums).") $tQs GROUP BY topic ";
		} else {
			$qs = "SELECT 'G',topic,MAX($sField) AS $sField FROM f_post WHERE forum IN (".join(',',$gForums).") $tQs GROUP BY topic ";
		}
		$qs .= " ORDER BY $sField $sOrder LIMIT $nb OFFSET $first";

		$q = $this->db->query($qs);
		$topics = array();
		while ($r = dbFetchArray($q)) {
			if ($r[0] == 'A') {
				$p = $this->lib->call('getTopic', $r[1]);
				$p['contents'] = $p['html'];
			} else {
				$p = $this->mForums->call('getTopic',$r[1]);
				$p['contents'] = $p['html'];
			}
			$p['ctype'] = $r[0];
			array_push($topics, $p);
		}

		return $topics;
	}
}

?>

<?php

class main_forums_getPosts {

	function main_forums_getPosts($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->accounts	= $this->lib->game->getLib("main/account");
	}


	function run($tid, $thr, $o, $cnt, $fst) {
		$os = $o?"DESC":"ASC";
		$posts = array();

		if ($thr) {
			// Read list of IDs
			$q = $this->db->query(
				"SELECT id,reply_to FROM f_post WHERE topic=$tid AND deleted IS NULL ORDER BY moment $os"
			);
			$ids = array();
			while ($qr = dbFetchArray($q)) {
				array_push($ids, $qr);
			}

			// Get first post
			if ($o) {
				$mp = array_pop($ids);
			} else {
				$mp = array_shift($ids);
			}

			// Initialize IDs and depths
			$sids = array($mp[0]);
			$dpth = array(0);

			// Create lists
			$ist = array($mp[0]);
			$cd = 0;
			while (count($ids)) {
				$od = $cd;
				for ($i=0;$i<count($ids)&&$od==$cd;$i++) {
					if ($ids[$i][1] != $ist[$cd]) {
						continue;
					}
					array_push($ist, $ids[$i][0]);
					array_push($sids, $ids[$i][0]);
					array_splice($ids, $i, 1);
					array_push($dpth, $cd);
					$cd++;
				}
				if ($cd == $od) {
					$cd--;
					array_pop($ist);
				}
			}

			$rsids = array_splice($sids, $fst, $cnt);
			$q = $this->db->query(
				"SELECT p.id AS id,p.author AS uid,u.name AS author,"
					. "p.moment AS moment,p.title AS title,"
					. "p.contents AS contents,p.enable_code AS ec,"
					. "p.enable_smileys AS es,p.edited AS edited,"
					. "p.edited_by "
				. "FROM f_post p,account u "
				. "WHERE p.id IN (".join(',',$rsids).") AND u.id=p.author "
					. "AND p.deleted IS NULL"
			);
			while ($qr = dbFetchHash($q)) {
				$qr['mine'] = ($qr['uid'] == $_SESSION['userid']);
				$qr['contents'] = $this->lib->call('substitute', 
					$qr['contents'], $qr['ec'], $qr['es']
				);
				$qr['contents'] .= $this->lib->call('signature', $qr['uid']);
				$i = array_search($qr['id'], $rsids);
				$qr['depth'] = $dpth[$i+$fst];
				if ($qr['depth'] > 19) {
					$qr['depth'] = 19;
				}
				if (!is_null($qr['edited_by'])) {
					$qr['edited_by'] = $this->accounts->call('getUserName', $qr['edited_by']);
				}
				$posts[$i] = $qr;
				$i++;
			}
		} else {
			$q = $this->db->query(
				"SELECT p.id AS id,p.author AS uid,u.name AS author,"
					. "p.moment AS moment,p.title AS title,"
					. "p.contents AS contents,p.enable_code AS ec,"
					. "p.enable_smileys AS es,p.edited AS edited,"
					. "p.edited_by AS edited_by "
				. "FROM f_post p,account u "
				. "WHERE p.topic=$tid AND u.id=p.author "
					. "AND p.deleted IS NULL "
				. "ORDER BY p.moment $os "
				. "LIMIT $cnt OFFSET $fst"
			);
			while ($qr = dbFetchHash($q)) {
				$qr['mine'] = ($qr['uid'] == $_SESSION['userid']);
				$qr['contents'] = $this->lib->call('substitute',
					$qr['contents'], $qr['ec'], $qr['es']
				);
				$qr['contents'] .= $this->lib->call('signature', $qr['uid']);
				$qr['depth'] = 0;
				if (!is_null($qr['edited_by'])) {
					$qr['edited_by'] = $this->accounts->call('getUserName', $qr['edited_by']);
				}
				array_push($posts, $qr);
			}
		}

		return $posts;
	}
}

?>

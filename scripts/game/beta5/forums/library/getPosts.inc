<?php

class beta5_forums_getPosts {

	function beta5_forums_getPosts($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->mForums	= $this->lib->game->getLib('main/forums');
	}


	function run($tid, $thr, $o, $cnt, $fst) {
		$os = $o?"DESC":"ASC";
		$posts = array();

		// FIXME: this is the wrong way to access player ID
		$myId = $_SESSION[game::sessName()]['player'];
		if ($thr) {
			// Read list of IDs
			$q = $this->db->query(
				"SELECT id,reply_to FROM af_post WHERE topic=$tid ORDER BY moment $os"
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
				"SELECT id,author AS pid,moment,title,contents,enable_code AS ec,"
					. "enable_smileys AS es,edited,edited_by AS edit_id "
				. "FROM af_post WHERE id IN (".join(',',$rsids).")"
			);
			while ($qr = dbFetchHash($q)) {
				$pinf = $this->players->call('get', $qr['pid'], true);
				$qr['author'] = $pinf['name'];
				$qr['mine'] = ($qr['pid'] == $myId);
				$qr['contents'] = $this->mForums->call('substitute',
					$qr['contents'], $qr['ec'], $qr['es']
				);
				$qr['contents'] .= $this->mForums->call('signature', $pinf['uid']);
				$i = array_search($qr['id'], $rsids);
				$qr['depth'] = $dpth[$i+$fst];
				if ($qr['depth'] > 9) {
					$qr['depth'] = 9;
				}
				if (!is_null($qr['edit_id'])) {
					$qr['edited_by'] = $this->players->call('getName', $qr['edit_id']);
				}
				$posts[$i] = $qr;
				$i++;
			}
		} else {
			$q = $this->db->query(
				"SELECT id,author AS pid,moment,title,contents,enable_code AS ec,"
					. "enable_smileys AS es,edited,edited_by AS edit_id "
				. "FROM af_post WHERE topic=$tid "
				. "ORDER BY moment $os LIMIT $cnt OFFSET $fst"
			);
			while ($qr = dbFetchHash($q)) {
				$pinf = $this->players->call('get', $qr['pid'], true);
				$qr['mine'] = ($qr['pid'] == $myId);
				$qr['author'] = $pinf['name'];
				$qr['contents'] = $this->mForums->call('substitute',
					$qr['contents'], $qr['ec'], $qr['es']
				);
				$qr['contents'] .= $this->mForums->call('signature', $pinf['uid']);
				$qr['depth'] = 0;
				if (!is_null($qr['edit_id'])) {
					$qr['edited_by'] = $this->players->call('getName', $qr['edit_id']);
				}
				array_push($posts, $qr);
			}
		}

		return $posts;
	}
}

?>

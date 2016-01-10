<?php

class beta5_forums_getPost {

	function beta5_forums_getPost($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->players	= $this->lib->game->getLib('beta5/player');
		$this->mForums	= $this->lib->game->getLib('main/forums');
	}


	function run($pid) {
		// Get post data
		$q = $this->db->query(
			"SELECT p.id AS id,p.title AS title,"
				. "t.id AS tid,p2.title AS tname,"
				. "f.id AS fid,f.title AS fname,"
				. "c.id AS cid,c.name AS cname,"
				. "p.author AS pid,p.reply_to as reply_to,"
				. "p.moment AS moment,p.title AS title,"
				. "p.contents AS contents,p.enable_code AS ec,"
				. "p.enable_smileys AS es,p.edited AS edited,"
				. "p.edited_by AS edit_id "
			. "FROM af_topic t,af_post p,af_post p2,af_forum f,alliance c "
			. "WHERE p.id=$pid AND t.id=p.topic AND p2.id=t.first_post "
				. "AND f.id=p.forum AND c.id=f.alliance"
		);
		if (!$q || dbCount($q) != 1) {
			return null;
		}
		$rv = dbFetchHash($q);
		$rv['html'] = $this->mForums->call('substitute',
			$rv['contents'], $rv['ec'], $rv['es']
		);
		$pinf = $this->players->call('get', $rv['pid'], true);
		$rv['html'] .= $this->mForums->call('signature', $pinf['uid']);
		$rv['author'] = $pinf['name'];
		if (!is_null($rv['edit_id'])) {
			$rv['edited_by'] = $this->players->call('getName', $rv['edit_id'], true);
		}
		return $rv;
	}
}

?>

<?php

class main_forums_getPost {

	function main_forums_getPost($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->accounts	= $this->lib->game->getLib("main/account");
	}


	function run($pid) {
		// Get post data
		$q = $this->db->query(
			"SELECT p.id AS id,p.title AS title,"
				. "t.id AS tid,p2.title AS tname,"
				. "f.id AS fid,f.title AS fname,"
				. "c.id AS cid,c.title AS cname,"
				. "p.author AS uid,u.name AS author,p.reply_to AS reply_to,"
				. "p.moment AS moment,p.title AS title,"
				. "p.contents AS contents,p.enable_code AS ec,"
				. "p.enable_smileys AS es,p.edited AS edited,"
				. "p.edited_by AS edited_by "
			. "FROM f_topic t,f_post p,f_post p2,f_forum f,f_category c,account u "
			. "WHERE p.id=$pid AND t.id=p.topic AND p2.id=t.first_post "
				. "AND f.id=p.forum AND c.id=f.category AND u.id=p.author "
				. "AND p.deleted IS NULL"
		);
		if (!$q || dbCount($q) != 1) {
			return null;
		}
		$rv = dbFetchHash($q);
		$rv['html'] = $this->lib->call('substitute',
			$rv['contents'], $rv['ec'], $rv['es']
		);
		$rv['html'] .= $this->lib->call('signature', $rv['uid']);

		if (!is_null($rv['edited_by'])) {
			$rv['edited_by'] = $this->accounts->call('getUserName', $rv['edited_by']);
		}
		if (preg_match('/^!.*!$/', $rv['cname'])) {
			$game = config::getGame(preg_replace('/!/', '', $rv['cname']));
			$rv['cname'] = $all[$game->game['site_path']][1];
		}
		return $rv;
	}
}

?>

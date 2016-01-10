<?php

class	page_handler
{
	var	$needsAuth = true;

	function	getForum($ctype, $fid, &$cats)
	{
		$forum = null;
		foreach	($cats as $cid => $cat)
		{
			if	($cat['type'] != $ctype)
				continue;
			for	($i=0;is_null($forum)&&$i<count($cat['forums']);$i++)
				if	($cat['forums'][$i]['id'] == $fid)
					$forum = $cat['forums'][$i];
			if	(!is_null($forum))
				break;
		}
		return	$forum;
	}

	function	getTopic($ctype, $tid)
	{
		if	(!preg_match('/^[1-9][0-9]*$/', $tid))
			return	null;
		if	($ctype == 'A')
			$ga = 'getForumTopic';
		else
			$ga = array('main','getTopic');
		return	gameAction($ga, $tid);
	}

	function	getPost($ctype, $pid)
	{
		if	(!preg_match('/^[1-9][0-9]*$/', $pid) || !preg_match('/^[AVG]$/', $ctype))
			return	null;
		if	($ctype == 'A')
			$ga = 'getForumPost';
		else
			$ga = array('main','getPost');
		$p = gameAction($ga, $pid);
		if	(!is_null($p))
			$p['rctype'] = $ctype;
		return	$p;
	}

	function	getPostFormData($in)
	{
		$action = is_null($in['e']) ? (is_null($in['p']) ? (is_null($in['c']) ? 'none' : 'cancel') : 'preview') : 'post';
		$smileys = ($in['sm'] == '1');
		$tags = ($in['fc'] == '1');
		$subject = $in['sub'];
		$txt = $in['txt'];
		$uid = $in['uid'];
		$sticky = $in['st'];
		return	array($action,$smileys,$tags,$subject,$txt,$uid,$sticky);
	}

	function checkPostData($s, &$t) {
		$maxNL = 500; $maxNC = 100;

		if	(strlen($s) < 2) {
			$e = 1;
		} elseif (strlen($s) > 100) {
			$e = 2;
		} elseif (strlen($t) < 3) {
			$e = 3;
		} else {
			$ot = preg_replace('/\r/', '', $t);
			$nt = ""; $nl = 0;
			while ($ot != '' && $nl < $maxNL) {
				$p = strpos($ot, "\n");
				if ($p !== false && $p < $maxNC) {
					$nt .= substr($ot, 0, $p+1);
					$ot = substr($ot, $p+1);
				} else if (strlen($ot) < $maxNC) {
					$nt .= $ot;
					$ot = "";
				} else {
					$s = substr($ot, 0, $maxNC);
					$p = strrpos($s, ' ');
					$ot = substr($ot, $maxNC);
					$nt .= $s;
					if	($p === false)
						$nt .= "\n";
				}
				$nl ++;
			}

			if ($nl >= $maxNL) {
				$e = 4;
			} else {
				$t = $nt;
				$e = 0;
			}
		}
		return	$e;
	}

	function	makeUniqueId()
	{
		if	(!is_array($_SESSION[game::sessName()]['postids']))
			$_SESSION[game::sessName()]['postids'] = array();
		do {
			$id = md5(uniqid(rand()));
		} while (in_array($id, $_SESSION[game::sessName()]['postids']));
		return	$id;
	}

	function	cleanIds()
	{
		if	(!is_array($_SESSION[game::sessName()]['postids']))
			return;
		$a = array();
		$now = time();
		foreach	($_SESSION[game::sessName()]['postids'] as $t => $id)
			if	($now - $t < 5)
				$a[$t] = $id;
		$_SESSION[game::sessName()]['postids'] = $a;
	}

	function	notDoublePost($id)
	{
		return	!(is_array($_SESSION[game::sessName()]['postids']) && in_array($id, $_SESSION[game::sessName()]['postids']));
	}

	function	addPostId($id)
	{
		if	(!is_array($_SESSION[game::sessName()]['postids']))
			return;
		$_SESSION[game::sessName()]['postids'][time()] = $id;
	}

	function	doForumMod(&$forum, $in, &$cats)
	{
		$sel = $in['msel'];
		if	(!count($sel))
			return;
		if	($in['dt'] != '')
		{
			if	($forum['ctype'] == 'A')
				$ga = 'deleteForumTopic';
			else
				$ga = array('main','deleteTopic');
			foreach	($sel as $tid) {
				gameAction($ga, $forum['id'], $tid);
				$forum['topics'] --;
			}
		}
		else if	($in['st'] != '')
		{
			if	($forum['ctype'] == 'A')
				$ga = 'switchTopicSticky';
			else
				$ga = array('main','switchTopicSticky');
			foreach	($sel as $tid)
				gameAction($ga, $forum['id'], $tid);
		}
		else if	($in['mt'] != '')
		{
			$dest = $in['mdest'];
			$f = $this->getForum($forum['ctype'],$dest,$cats);
			if (is_null($f) && $forum['ctype'] != 'A') {
				$rev = array('V' => 'G', 'G' => 'V');
				$ctype = $rev[$forum['ctype']];
				$f = $this->getForum($ctype,$dest,$cats);
				if (is_null($f) || !$f['mod']) {
					return;
				}
			} else if (!$f['mod']) {
				return;
			}

			if	($forum['ctype'] == 'A')
			{
				$ga = 'moveForumTopic';
				$pid = $_SESSION[game::sessName()]['player'];
			}
			else
			{
				$ga = array('main','moveTopic');
				$pid = $_SESSION['userid'];
			}
			foreach	($sel as $tid) {
				gameAction($ga, $forum['id'], $tid, $f['id'], $pid);
				$forum['topics'] --;
			}
		}
	}

	function	doDelete($cpid)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		list($ctype,$postId) = explode('#',$cpid);
		$post = $this->getPost($ctype,$postId);
		if	(is_null($post))
		{
			$this->data = array("sp" => 'postnotfound', "d" => $cats);
			return;
		}
		$forum = $this->getForum($ctype,$post['fid'],$cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}
		$topic = $this->getTopic($ctype,$post['tid']);
		if	(is_null($topic))
		{
			$this->data = array("sp" => 'topicnotfound', "d" => $cats);
			return;
		}

		if	(	!$forum['mod']
			&&	(	($ctype == 'A' && $post['pid'] != $_SESSION[game::sessName()]['player'])
				||	($ctype != 'A' && $post['uid'] != $_SESSION['userid'])
				||	($post['id'] == $topic['fpid'])
				)
			)
		{
			$this->data = array("sp" => 'overview', "d" => $cats);
			return;
		}

		if	($forum['mod'] && $post['id'] == $topic['fpid'])
		{
			if	($ctype == 'A')
				$ga = 'deleteForumTopic';
			else
				$ga = array('main','deleteTopic');
			gameAction($ga, $forum['id'], $topic['id']);
			$this->pgForum($ctype."#".$forum['id'], '', '');
		}
		else
		{
			if	($ctype == 'A')
				$ga = 'deleteSinglePost';
			else
				$ga = array('main','deleteSinglePost');
			gameAction($ga, $post['id']);
			$this->pgTopic($ctype."#".$topic['id'], '', '', '', '');
		}
	}

	function	markForum($id, $pg)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);

		list($ctype,$fid) = explode('#',$id);
		$forum = $this->getForum($ctype,$fid,$cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}
		$forum['ctype'] = $ctype;

		if	($ctype == 'A')
		{
			$ga = 'markForumAsRead';
			$uid = $_SESSION[game::sessName()]['player'];
		}
		else
		{
			$ga = array('main','markForumAsRead');
			$uid = $_SESSION['userid'];
		}
		gameAction($ga, $fid, $uid);

		$this->pgForum($id, '', $pg);
	}

	function	markCategory($cid)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		if	(preg_match('/^V#/', $cid))
			$cid = preg_replace('/^V#/', 'G#', $cid);
		if	(is_null($cats[$cid]))
		{
			$this->data = array("sp" => 'catnotfound', "d" => $cats);
			return;
		}

		$c = $cats[$cid];
		if	($c['type'] == 'A')
		{
			$ga = 'markForumAsRead';
			$uid = $_SESSION[game::sessName()]['player'];
		}
		else
		{
			$ga = array('main','markForumAsRead');
			$uid = $_SESSION['userid'];
		}
		foreach	($c['forums'] as $f)
			gameAction($ga, $f['id'], $uid);

		$this->pgCategory($cid);
	}

	function	pgOverview()
	{
		$this->data = array(
			"d"	=> gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']),
			"sp"	=> 'overview'
		);
	}

	function	pgCategory($cid)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		if	(is_null($cats[$cid]))
			$this->data = array("sp" => 'catnotfound', "d" => $cats);
		else
			$this->data = array(
				"sp"	=> "category",
				"d"	=> array(
					"all"	=> $cats,
					"id"	=> $cid
				)
			);
	}

	function	pgForum($cfid, $tpp, $pg, $in = array())
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);

		list($ctype,$fid) = explode('#',$cfid);
		$forum = $this->getForum($ctype,$fid,$cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}
		$forum['ctype'] = $ctype;

		if	($forum['mod'] && count($in))
			$this->doForumMod($forum, $in, $cats);

		if	(preg_match('/^[1-5]0$/', $tpp))
			prefs::set('main/forums_ntopics', $tpp);
		elseif	($prefs['main']['forums_ntopics'] != "")
			$tpp = prefs::get('main/forums_ntopics', 20);
		else
			$tpp = 20;

		$mod = $forum['topics'] % $tpp;
		$npg = ($mod ? 1 : 0) + ($forum['topics']-$mod) / $tpp;

		if	($pg != "0" && !preg_match('/^[0-9]+$/', $pg))
			$pg = 0;
		else if	($pg * $tpp > $forum['topics'])
			$pg = $npg - 1;

		if	($ctype == "A")
		{
			$get = 'getForumTopics';
			$isRead = 'isTopicRead';
			$uid = $_SESSION[game::sessName()]['player'];
		}
		else
		{
			$get = array('main','getTopics');
			$isRead = array('main','isTopicRead');
			$uid = $_SESSION['userid'];
		}
		$topics = gameAction($get,$forum['id'],$pg*$tpp,$tpp);
		for	($i=0;$i<count($topics);$i++)
			$topics[$i]['read'] = gameAction($isRead,$topics[$i]['id'],$uid);

		$this->data = array(
			"sp"		=> 'forum',
			"d"		=> array(
				"cats"		=> $cats,
				"forum"		=> $forum,
				"topics"	=> $topics,
				"details"	=> array($tpp,$pg,$npg)
			)
		);
	}

	function	pgTopic($ctid, $mpp, $pg, $fdm, $fmo)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);

		list($ctype,$tid) = explode('#',$ctid);
		$topic = $this->getTopic($ctype, $tid);
		if	(is_null($topic))
		{
			$this->data = array("sp" => 'topicnotfound', "d" => $cats);
			return;
		}

		$forum = $this->getForum($ctype, $topic['fid'], $cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}
		$forum['ctype'] = $ctype;

		if	(preg_match('/^[1-5]0$/', $mpp))
			prefs::set('main/forums_nitems', $mpp);
		elseif	($prefs['main']['forums_nitems'] != "")
			$mpp = prefs::get('main/forums_nitems', 20);
		else
			$mpp = 20;

		$mod = $topic['nitems'] % $mpp;
		$npg = ($mod ? 1 : 0) + ($topic['nitems']-$mod) / $mpp;

		if	($pg != "0" && !preg_match('/^[0-9]+$/', $pg))
			$pg = 0;
		else if	(($pg - 1) * $mpp > $topic['nitems'])
			$pg = $npg - 1;
		else
			$pg --;

		if	($fdm != "")
		{
			$thr = ($fdm == "1");
			if	(prefs::get('main/forums_threaded') != $fdm)
				prefs::set('main/forums_threaded', $thr?"1":"0");
		}
		else
			$thr = (prefs::get('main/forums_threaded') == 1);
		if	($fmo != "")
		{
			$rev = ($fmo == "1");
			if	(prefs::get('main/forums_reversed') != $fmo)
				prefs::set('main/forums_reversed', $rev?"1":"0");
		}
		else
			$rev = (prefs::get('main/forums_reversed') == 1);

		if	($ctype == "A")
		{
			$ga = 'getForumPosts';
			$mr = 'markAsRead';
			$uid = $_SESSION[game::sessName()]['player'];
		}
		else
		{
			$ga = array('main', 'getPosts');
			$mr = array('main', 'markAsRead');
			$uid = $_SESSION['userid'];
		}
		$posts = gameAction($ga, $topic['id'], $thr, $rev, $mpp, $mpp * $pg);
		if	(gameAction($mr, $topic['id'], $uid))
		{
			$forum['unread'] --;
			$cid = $ctype."#".$forum[$ctype=='A'?'alliance':'category'];
			for	($i=0;$i<count($cats[$cid]['forums']);$i++)
				if	($cats[$cid]['forums'][$i]['id'] == $forum['id'])
				{
					$cats[$cid]['forums'][$i]['unread'] --;
					break;
				}
		}

		$this->data = array(
			"sp"		=> 'topic',
			"d"		=> array(
				"cats"		=> $cats,
				"forum"		=> $forum,
				"topic"		=> $topic,
				"posts"		=> $posts,
				"details"	=> array($mpp,$pg,$npg,$thr,$rev)
			)
		);
	}

	function	pgLatest($cat, $mpp = '', $pg = '')
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		$aForums = $gForums = array();

		if	($cat == '')
		{
			foreach	($cats as $cid => $cd)
			{
				foreach	($cd['forums'] as $f)
					if	($cd['type'] == 'A')
						array_push($aForums, $f['id']);
					else
						$gForums[$f['id']] = $cd['type'];
			}
		}
		else if	(is_null($cats[$cat]))
		{
			$this->data = array("sp" => 'catnotfound', "d" => $cats);
			return;
		}
		else if	($cats[$cat]['type'] == 'A')
			foreach	($cats[$cat]['forums'] as $f)
				array_push($aForums, $f['id']);
		else
			foreach	($cats[$cat]['forums'] as $f)
				$gForums[$f['id']] = $cats[$cat]['type'];

		if	(preg_match('/^[1-5]0$/', $mpp))
			prefs::set('main/forums_nitems', $mpp);
		else
			$mpp = prefs::get('main/forums_nitems', 20);

		$i = 0;
		do {
			$list = gameAction('getForumLatest', $aForums, array_keys($gForums), $mpp + 1, (int)$pg * $mpp);
			if	(!count($list))
			{
				$pg --;
				$i ++;
			}
		} while	(!count($list) && $pg > -1 && $i < 20);
		for	($i=0;$i<count($list);$i++)
		{
			if	($list[$i]['ctype'] == 'G')
				$list[$i]['rctype'] = $gForums[$list[$i]['fid']];
			else
				$list[$i]['rctype'] = 'A';
		}
		if	($pg == -1)
			$pg = 0;
		$this->data = array(
			"sp"		=> 'latest',
			"d"		=> array(
				"cats"		=> $cats,
				"cat"		=> $cat,
				"posts"		=> $list,
				"details"	=> array($mpp,$pg)
			)
		);
	}

	function	pgNewTopic($cfid,$in)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);

		list($ctype,$fid) = explode('#',$cfid);
		$forum = $this->getForum($ctype,$fid,$cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}

		list($action,$smileys,$tags,$subject,$text,$uid,$sticky) = $this->getPostFormData($in);
		if	($action == 'cancel')
		{
			$this->pgForum($cfid, '', '');
			return;
		}
		else if	(!$forum['user_post'] && !$forum['mod'])
		{
			$this->data = array("sp" => 'forumnopost', "d" => $cats);
			return;
		}

		$sticky = $sticky && $forum['mod'];
		$this->data = array(
			'sp'	=> 'post',
			'd'	=> array(
				'cats'	=> $cats,
				'cmd'	=> 'n',
				'forum'	=> $forum,
				'cfid'	=> $cfid,
				'ref'	=> $cfid,
				'sub'	=> '',
				'txt'	=> '',
				'prev'	=> '',
				'err'	=> 0,
				'sst'	=> $forum['mod'],
				'st'	=> $sticky
			)
		);

		if	($action == 'none')
		{
			$this->data['d']['uid'] = $this->makeUniqueId();
			$this->data['d']['sm'] = prefs::get('main/smileys');
			$this->data['d']['fc'] = prefs::get('main/forum_code');
			return;
		}

		$this->data['d']['sm'] = $smileys;
		$this->data['d']['fc'] = $tags;
		$this->data['d']['st'] = $sticky;
		$this->data['d']['sub'] = $subject;
		$this->data['d']['txt'] = $text;
		$this->data['d']['uid'] = $uid;
		$this->data['d']['err'] = $err = $this->checkPostData($subject,$text);

		if	($err)
			return;

		if	($action == 'preview')
		{
			$subst = array('main','forumSubstitute');
			$prev = gameAction($subst, $text, $tags, $smileys);
			if	(prefs::get('main/forums_sig') != "")
				$prev .= '<hr/>' . gameAction(
					$subst, prefs::get('main/forums_sig'), prefs::get('main/forum_code'), prefs::get('main/smileys')
				);
			$this->data['d']['prev'] = $prev;
			$this->data['d']['st'] = $sticky;
			return;
		}

		if	($this->notDoublePost($uid))
		{
			if	($ctype == 'A')
			{
				$a = $_SESSION[game::sessName()]['player'];
				$ga = 'newForumTopic';
			}
			else
			{
				$a = $_SESSION['userid'];
				$ga = array('main','newTopic');
			}
			gameAction($ga, $a, $fid, $subject, $text, $tags, $smileys, $sticky);
			$this->addPostId($uid);
		}

		$this->pgForum($cfid, '', '');
	}

	function	pgReply($repTo,$in)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		list($ctype,$postId) = explode('#',$repTo);
		$post = $this->getPost($ctype,$postId);
		if	(is_null($post))
		{
			$this->data = array("sp" => 'postnotfound', "d" => $cats);
			return;
		}
		$forum = $this->getForum($ctype,$post['fid'],$cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}

		$list = array($post);
		$i = 0;
		while	($i<4 && !is_null($post['reply_to']))
		{
			$post = $this->getPost($ctype,$post['reply_to']);
			if	(is_null($post))
				break;
			array_push($list, $post);
			$i ++;
		}

		list($action,$smileys,$tags,$subject,$text,$uid) = $this->getPostFormData($in);
		if	($action == 'cancel')
		{
			$this->pgTopic($ctype."#".$list[0]['tid'], '', 10000, '', '');
			return;
		}

		$this->data = array(
			'sp'	=> 'post',
			'd'	=> array(
				'cats'	=> $cats,
				'cmd'	=> 'R#'.$repTo,
				'forum'	=> $forum,
				'cfid'	=> $ctype."#".$forum['id'],
				'ref'	=> $repTo,
				'posts'	=> $list,
				'sub'	=> '',
				'txt'	=> '',
				'prev'	=> '',
				'err'	=> 0
			)
		);

		if	($action == 'none')
		{
			$this->data['d']['uid'] = $this->makeUniqueId();
			$this->data['d']['sm'] = prefs::get('main/smileys');
			$this->data['d']['fc'] = prefs::get('main/forum_code');
			$sub = trim($list[0]['title']);
			if	(!preg_match('/^Re:/', $sub))
				$sub = "Re: $sub";
			$this->data['d']['sub'] = $sub;
			if	($in['q'] == 1)
				$this->data['d']['txt'] = "[quote=".$list[0]['author']."]".$list[0]['contents']."[/quote]";
			return;
		}

		$this->data['d']['sm'] = $smileys;
		$this->data['d']['fc'] = $tags;
		$this->data['d']['sub'] = $subject;
		$this->data['d']['txt'] = $text;
		$this->data['d']['uid'] = $uid;
		$this->data['d']['err'] = $err = $this->checkPostData($subject,$text);

		if	($err)
			return;

		if	($action == 'preview')
		{
			$subst = array('main','forumSubstitute');
			$prev = gameAction($subst, $text, $tags, $smileys);
			if	(prefs::get('main/forums_sig') != "")
				$prev .= '<hr/>' . gameAction(
					$subst, prefs::get('main/forums_sig'), prefs::get('main/forum_code'), prefs::get('main/smileys')
				);
			$this->data['d']['prev'] = $prev;
			return;
		}

		if	($this->notDoublePost($uid))
		{
			if	($ctype == 'A')
			{
				$a = $_SESSION[game::sessName()]['player'];
				$ga = 'postForumReply';
			}
			else
			{
				$a = $_SESSION['userid'];
				$ga = array('main','postReply');
			}
			gameAction($ga, $a, $list[0], $subject, $text, $tags, $smileys);
			$this->addPostId($uid);
		}

		$this->pgTopic($ctype."#".$list[0]['tid'], '', 10000, '', '');
	}

	function	pgEdit($id,$in)
	{
		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		list($ctype,$postId) = explode('#',$id);
		$post = $this->getPost($ctype,$postId);
		if	(is_null($post))
		{
			$this->data = array("sp" => 'postnotfound', "d" => $cats);
			return;
		}
		$forum = $this->getForum($ctype,$post['fid'],$cats);
		if	(is_null($forum))
		{
			$this->data = array("sp" => 'forumnotfound', "d" => $cats);
			return;
		}

		if	(	!$forum['mod']
			&&	(	($ctype == 'A' && $post['pid'] != $_SESSION[game::sessName()]['player'])
				||	($ctype != 'A' && $post['uid'] != $_SESSION['userid'])
				)
			)
		{
			$this->data = array("sp" => 'overview', "d" => $cats);
			return;
		}

		list($action,$smileys,$tags,$subject,$text,$uid) = $this->getPostFormData($in);
		if	($action == 'cancel')
		{
			$this->pgTopic($ctype."#".$post['tid'], '', 10000, '', '');
			return;
		}

		$this->data = array(
			'sp'	=> 'post',
			'd'	=> array(
				'cats'	=> $cats,
				'cmd'	=> 'E#'.$id,
				'forum'	=> $forum,
				'cfid'	=> $ctype."#".$forum['id'],
				'ref'	=> $id,
				'post'	=> $post,
				'sub'	=> '',
				'txt'	=> '',
				'prev'	=> '',
				'err'	=> 0
			)
		);

		if	($action == 'none')
		{
			$this->data['d']['uid'] = '';
			$this->data['d']['sm'] = ($post['es'] == 't');
			$this->data['d']['fc'] = ($post['ec'] == 't');
			$this->data['d']['sub'] = trim($post['title']);
			$this->data['d']['txt'] = $post['contents'];
			return;
		}

		$this->data['d']['sm'] = $smileys ? 1 : 0;
		$this->data['d']['fc'] = $tags ? 1 : 0;
		$this->data['d']['sub'] = $subject;
		$this->data['d']['txt'] = $text;
		$this->data['d']['uid'] = $uid;
		$this->data['d']['err'] = $err = $this->checkPostData($subject,$text);

		if	($err)
			return;

		if	($action == 'preview')
		{
			$subst = array('main','forumSubstitute');
			$prev = gameAction($subst, $text, $tags, $smileys);
			if	(prefs::get('main/forums_sig') != "")
				$prev .= '<hr/>' . gameAction(
					$subst, prefs::get('main/forums_sig'),
					prefs::get('main/forum_code'), prefs::get('main/smileys')
				);
			$this->data['d']['prev'] = $prev;
			return;
		}

		if	($ctype == 'A')
		{
			$a = $_SESSION[game::sessName()]['player'];
			$ga = 'editForumPost';
		}
		else
		{
			$a = $_SESSION['userid'];
			$ga = array('main','postEdit');
		}
		gameAction($ga, $a, $post['id'], $subject, $text, $tags, $smileys);

		$this->pgTopic($ctype."#".$post['tid'], '', 10000, '', '');
	}

	function	doSearchPosts(&$cats)
	{
		$gfl = $afl = array();
		if	($_SESSION[game::sessName()]['forumsearch']['fid'] == '')
		{
			foreach ($cats as $cat)
				if	($cat['type'] == 'A')
					foreach	($cat['forums'] as $f) array_push($afl, $f['id']);
				else
					foreach	($cat['forums'] as $f) $gfl[$f['id']] = $cat['type'];
		}
		else
		{
			list($ctype,$fid) = explode('#', $_SESSION[game::sessName()]['forumsearch']['fid']);
			if	($ctype == 'A')
				array_push($afl, $fid);
			else
				$gfl[$fid] = $ctype;
		}

		$txt = '%' . preg_replace('/\*/', '%', addslashes($_SESSION[game::sessName()]['forumsearch']['text'])) . '%';
		$pg = $_SESSION[game::sessName()]['forumsearch']['page'];
		$mpp = $_SESSION[game::sessName()]['forumsearch']['perpage'];
		$c = $_SESSION[game::sessName()]['forumsearch']['whole'];
		$sfs = array('moment','title');
		$stt = $sfs[$_SESSION[game::sessName()]['forumsearch']['stype']];
		$ord = $_SESSION[game::sessName()]['forumsearch']['sord'] ? "DESC" : "ASC";
		$i = 0;
		do {
			$list = gameAction('forumSearchPosts',$afl,array_keys($gfl),$mpp+1,$pg*$mpp,$txt,$c,$stt,$ord);
			if	(!count($list))
			{
				$pg --;
				$i ++;
			}
		} while	(!count($list) && $pg > -1 && $i < 20);
		for	($i=0;$i<count($list);$i++)
		{
			if	($list[$i]['ctype'] == 'G')
				$list[$i]['rctype'] = $gfl[$list[$i]['fid']];
			else
				$list[$i]['rctype'] = 'A';
		}

		if	($pg < 0)
			$pg = 0;
		$_SESSION[game::sessName()]['forumsearch']['page'] = $pg;

		$this->data = array(
			"sp"	=> 'sresposts',
			"d"	=> array(
					'cats'	=> $cats,
					'sparm'	=> $_SESSION[game::sessName()]['forumsearch'],
					'posts'	=> $list
				)
		);
	}

	function	doSearchTopics(&$cats)
	{
		$fl = $gfl = $afl = array();
		if	($_SESSION[game::sessName()]['forumsearch']['fid'] == '')
		{
			foreach ($cats as $cat)
			{
				if	($cat['type'] == 'A')
					foreach	($cat['forums'] as $f) array_push($afl, $f['id']);
				else
					foreach	($cat['forums'] as $f) $gfl[$f['id']] = $cat['type'];
				foreach ($cat['forums'] as $f)
					$fl[$cat['type'] . "#" . $f['id']] = array($f['title'],$cat['id'],$cat['title']);
			}
		}
		else
		{
			list($ctype,$fid) = explode('#', $_SESSION[game::sessName()]['forumsearch']['fid']);
			if	($ctype == 'A')
				array_push($afl, $fid);
			else
				$gfl[$fid] = $ctype;

			foreach ($cats as $cat) foreach ($cat['forums'] as $f)
				$fl[$cat['type'] . "#" . $f['id']] = array($f['title'],$cat['id'],$cat['title']);
		}

		$txt = '%' . preg_replace('/\*/', '%', addslashes($_SESSION[game::sessName()]['forumsearch']['text'])) . '%';
		$pg = $_SESSION[game::sessName()]['forumsearch']['page'];
		$mpp = $_SESSION[game::sessName()]['forumsearch']['perpage'];
		$c = $_SESSION[game::sessName()]['forumsearch']['whole'];
		$sfs = array('moment','title');
		$stt = $sfs[$_SESSION[game::sessName()]['forumsearch']['stype']];
		$ord = $_SESSION[game::sessName()]['forumsearch']['sord'] ? "DESC" : "ASC";
		$i = 0;
		do {
			$list = gameAction('forumSearchTopics',$afl,array_keys($gfl),$mpp+1,$pg*$mpp,$txt,$c,$stt,$ord);
			if	(!count($list))
			{
				$pg --;
				$i ++;
			}
		} while	(!count($list) && $pg > -1 && $i < 20);
		$_SESSION[game::sessName()]['forumsearch']['page'] = $pg;

		$pid = $_SESSION[game::sessName()]['player'];
		$uid = $_SESSION['userid'];
		for	($i=0;$i<count($list);$i++)
		{
			list($list[$i]['fname'],$list[$i]['catid'],$list[$i]['catname']) = $fl[$list[$i]['ctype']."#".$list[$i]['fid']];
			if	($list[$i]['catid'] == "")
				list($list[$i]['fname'],$list[$i]['catid'],$list[$i]['catname']) = $fl["V#".$list[$i]['fid']];
			if	($list[$i]['ctype'] == 'A')
			{
				$list[$i]['read'] = gameAction('isTopicRead', $list[$i]['id'], $pid);
				$fp = gameAction('getForumPost', $list[$i]['fpid']);
				$lp = gameAction('getForumPost', $list[$i]['lpid']);
			}
			else
			{
				$list[$i]['read'] = gameAction(array('main','isTopicRead'), $list[$i]['id'], $uid);
				$fp = gameAction(array('main','getPost'), $list[$i]['fpid']);
				$lp = gameAction(array('main','getPost'), $list[$i]['lpid']);
			}
			$list[$i]['moment'] = $fp['moment'];
			$list[$i]['author'] = $fp['author'];
			$list[$i]['author_id'] = $fp['pid'];
			$list[$i]['last_moment'] = $lp['moment'];
			$list[$i]['last_author'] = $lp['author'];
			$list[$i]['last_author_id'] = $lp['pid'];
			if	($list[$i]['ctype'] == 'G')
				$list[$i]['rctype'] = $gfl[$list[$i]['fid']];
			else
				$list[$i]['rctype'] = 'A';
		}

		$this->data = array(
			"sp"	=> 'srestopics',
			"d"	=> array(
					'cats'		=> $cats,
					'sparm'		=> $_SESSION[game::sessName()]['forumsearch'],
					'topics'	=> $list
				)
		);
	}

	function	pgSearch($in)
	{
		if	(!is_array($_SESSION[game::sessName()]['forumsearch']))
			$_SESSION[game::sessName()]['forumsearch'] = array();

		$cats = gameAction('getCatsAndForums', $_SESSION[game::sessName()]['player']);
		if	((is_null($in['s']) || trim($in['stxt']) == '') && is_null($in['pg']))
		{
			$this->data = array(
				"sp"	=> 'search',
				"d"	=> array('cats' => $cats, 'sparm' => $_SESSION[game::sessName()]['forumsearch'])
			);
			return;
		}

		if	(!is_null($in['s']))
		{
			$_SESSION[game::sessName()]['forumsearch']['text'] = trim($in['stxt']);
			$_SESSION[game::sessName()]['forumsearch']['whole'] = ($in['sin'] == 1);
			$sid = $in['sif'];
			if	($sid != '')
			{
				list($ctype,$fid) = explode('#', $sid);
				$f = $this->getForum($ctype,$fid,$cats);
				if	(is_null($f))
					$sid = '';
			}
			$_SESSION[game::sessName()]['forumsearch']['fid'] = $sid;
			$st = (int)$in['sst'];
			if	($st < 0 || $st > 1)
				$st = 0;
			$_SESSION[game::sessName()]['forumsearch']['stype'] = $st;
			$_SESSION[game::sessName()]['forumsearch']['sord'] = ($in['sor'] == 1);
			$_SESSION[game::sessName()]['forumsearch']['resd'] = ($in['srp'] == 1);
			$_SESSION[game::sessName()]['forumsearch']['page'] = 0;
			$_SESSION[game::sessName()]['forumsearch']['perpage'] = prefs::get('main/' . ($in['srp'] == 1 ? 'forums_ntopics' : 'forums_nitems'));
		} else {
			$_SESSION[game::sessName()]['forumsearch']['page'] = (int)$in['pg'];
			if	(preg_match('/^[1-5]0$/', $in['mpp']))
			{
				$_SESSION[game::sessName()]['forumsearch']['perpage'] = (int)$in['mpp'];
				prefs::set("main/" . ($_SESSION[game::sessName()]['forumsearch']['resd'] ? 'forums_ntopics' : 'forums_nitems'), $in['mpp']);
			}
		}

		if	($_SESSION[game::sessName()]['forumsearch']['resd'])
			$this->doSearchTopics($cats);
		else
			$this->doSearchPosts($cats);
	}

	function	handle($input)
	{
		$this->cleanIds();
		$c = $input['cmd'];
		switch	($c)
		{
			case	'o':
				$this->pgOverview();
				break;
			case	'n':
				$this->pgNewTopic(substr($input['f'],2),$input);
				break;
			case	'l':
				$this->pgLatest('', $input['mpp'], $input['pg']);
				break;
			case	's':
				$this->pgSearch($input);
				break;
			default:
				if	(substr($c,0,2) == "C#")
					$this->pgCategory(substr($c,2));
				else if	(substr($c,0,2) == "F#")
					$this->pgForum(substr($c,2), $input['tpp'], $input['pg'], $input);
				else if	(substr($c,0,2) == "T#")
					$this->pgTopic(substr($c,2),$input['mpp'],$input['pg'],$input['thr'],$input['ord']);
				else if	(substr($c,0,2) == "L#")
					$this->pgLatest(substr($c,2), $input['mpp'], $input['pg']);
				else if	(substr($c,0,2) == "R#")
					$this->pgReply(substr($c,2), $input);
				else if	(substr($c,0,2) == "E#")
					$this->pgEdit(substr($c,2), $input);
				else if	(substr($c,0,2) == "D#")
					$this->doDelete(substr($c,2), $input);
				else if	(substr($c,0,3) == "MF#")
					$this->markForum(substr($c,3), $input['pg']);
				else if	(substr($c,0,3) == "MC#")
					$this->markCategory(substr($c,3));
				else
					$this->pgOverview();
		}
		$this->output = "forums";
	}
}

?>

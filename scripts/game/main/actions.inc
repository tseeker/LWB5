<?php

class actions_main {
	var	$versions = null;
	var	$userNames = array();


	var $index	= array(
		'joinGame',
		'lostPassword'
	);


	function actions_main($game) {
		$this->game = $game;
		$this->main = $this->game->getLib('main');
		$this->accounts = $this->game->getLib('main/account');
		$this->vacation = $this->game->getLib('main/vacation');
		$this->forums = $this->game->getLib('main/forums');
	}

	function getUserName($uid) {
		return $this->accounts->call('getUserName', $uid);
	}

	function createAccount($u, $p, $e, $l) {
		return $this->accounts->call('createAccount', $u, $p, $e, $l);
	}

	function isGameRunning($version) {
		return $this->main->call('isGameRunning', $version);
	}

	function getTick($version, $name, $lang = null) {
		$g = config::getGame($version);
		$lib = $g->getLib('main');
		return $lib->call('getTick', $name, $lang);
	}

	function getTicks($version, $lang) {
		$g = config::getGame($version);
		$lib = $g->getLib('main');
		return $lib->call('getTicks', $lang);
	}

	function getRankingType($version, $identifier) {
		$g = config::getGame($version);
		$rnk = $g->getLib('main/rankings');
		return $rnk->call('getType', $identifier);
	}

	function getRankingTypes($version) {
		$g = config::getGame($version);
		$rnk = $g->getLib('main/rankings');
		return $rnk->call('getTypes');
	}

	function getRankingText($id, $lang) {
		$rnk = $this->game->getLib('main/rankings');
		return $rnk->call('getText', $id, $lang);
	}

	function updateRankings($id, $data) {
		$rnk = $this->game->getLib('main/rankings');
		$rnk->call('update', $id, $data);
	}

	function getRankings($type, $top = null) {
		$rnk = $this->game->getLib('main/rankings');
		return $rnk->call('getAll', $type, $top);
	}

	function getRanking($type, $id) {
		$rnk = $this->game->getLib('main/rankings');
		return $rnk->call('get', $type, $id);
	}

	function appendRanking($type,$id) {
		$rnk = $this->game->getLib('main/rankings');
		return $rnk->call('append', $type, $id);
	}

	function deleteRanking($type,$id) {
		$rnk = $this->game->getLib('main/rankings');
		return $rnk->call('delete', $type, $id);
	}

	function getForumCategories() {
		return $this->forums->call('getCategories');
	}

	function getVersionCategory($ver) {
		return $this->forums->call('getVersionCategory', $ver);
	}

	function getForumCategory($c) {
		return $this->forums->call('getCategory', $c);
	}

	function getForums($c) {
		return $this->forums->call('getForums', $c);
	}

	function getForum($f) {
		return $this->forums->call('get', $f);
	}

	function getTopics($f, $first, $count) {
		return $this->forums->call('getTopics', $f, $first, $count);
	}

	function newTopic($a, $fid, $sub, $txt, $ec, $es, $st) {
		return $this->forums->call('newTopic', $a, $fid, $sub, $txt, $ec, $es, $st);
	}

	function postReply($a, $post, $sub, $txt, $ec, $es) {
		return $this->forums->call('reply', $a, $post, $sub, $txt, $ec, $es);
	}

	function postEdit($a, $pid, $sub, $txt, $ec, $es) {
		return $this->forums->call('edit', $a, $pid, $sub, $txt, $ec, $es);
	}

	function forumSubstitute($text, $ec, $es) {
		return $this->forums->call('substitute', $text, $ec, $es);
	}

	function forumSignature($u) {
		return $this->forums->call('signature', $u);
	}

	function getTopic($tid) {
		return $this->forums->call('getTopic', $tid);
	}

	function getPosts($tid, $thr, $o, $cnt, $fst) {
		return $this->forums->call('getPosts', $tid, $thr, $o, $cnt, $fst);
	}

	function getPost($pid) {
		return $this->forums->call('getPost', $pid);
	}

	function isTopicRead($topic, $player) {
		return $this->forums->call('isRead', $topic, $player);
	}

	function markAsRead($topic, $player) {
		return $this->forums->call('markRead', $topic, $player);
	}

	function markAsUnread($topic, $player) {
		return $this->forums->call('markUnread', $topic, $player);
	}

	function getReadTopics($fid, $uid) {
		return $this->forums->call('getRead', $fid, $uid);
	}

	function updateForumLast($forum) {
		return $this->forums->call('updateLast', $forum);
	}

	function deleteTopic($forum, $topic) {
		return $this->forums->call('deleteTopic', $forum, $topic);
	}

	function switchTopicSticky($forum, $topic) {
		return $this->forums->call('switchSticky', $forum, $topic);
	}

	function moveTopic($forum, $topic, $dest, $user) {
		return $this->forums->call('move', $forum, $topic, $dest, $user);
	}

	function deleteSinglePost($postId) {
		return $this->forums->call('deletePost', $postId);
	}

	function markForumAsRead($fid, $uid) {
		return $this->forums->call('markForumRead', $fid, $uid);
	}

	function getAdministrator($uid) {
		return $this->forums->call('getAdministrator', $uid);
	}

	function getModerator($uid) {
		return $this->forums->call('getModerator', $uid);
	}

	function getAccounts() {
		return $this->accounts->call('getAccounts');
	}

	function isOnline($uid) {
		return $this->accounts->call('isOnline', $uid);
	}

	function setQuitCountdown($uid, $reason) {
		$this->accounts->call('setQuitCountdown', $uid, $reason);
	}

	function cancelQuitCountdown($uid) {
		$this->accounts->call('cancelQuitCountdown', $uid);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// VACATION MODE MANAGEMENT
	//--------------------------------------------------------------------------------------------------------------------------------

	function isOnVacation($uid) {
		return $this->vacation->call('isOnVacation', $uid);
	}

	function canSetVacation($uid) {
		return $this->vacation->call('canSet', $uid);
	}

	function setVacationStart($uid) {
		return $this->vacation->call('setStart', $uid);
	}

	function resetVacationStart($uid) {
		return $this->vacation->call('resetStart', $uid);
	}

	function startVacation($uid) {
		return $this->vacation->call('start', $uid);
	}

	function leaveVacation($uid) {
		return $this->vacation->call('leave', $uid);
	}
}

?>

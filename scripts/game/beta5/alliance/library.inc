<?php

class beta5_alliance_library {
	var $index	= array(
		'acceptRequest',
		'addCandidate',
		'cancelRequest',
		'changeRank',
		'create',
		'createForum',
		'createRank',
		'deleteForum',
		'deleteRank',
		'get',
		'getCandidates',
		'getForums',
		'getForumsComplete',
		'getId',
		'getKeepers',
		'getMembers',
		'getMilitary',
		'getPlanets',
		'getPrivileges',
		'getRankPrivileges',
		'getRankSize',
		'getRanks',
		'getRequests',
		'getTechList',
		'getTechOrder',
		'getTechOrders',
		'getTechRequests',
		'getTechSubmission',
		'getVoters',
		'kick',
		'leave',
		'modifyForum',
		'modifyRank',
		'moveForum',
		'rejectRequest',
		'removeCandidate',
		'sendRequest',
		'setDemocratic',
		'setForumAccess',
		'setSuccessor',
		'setTechRequests',
		'setTechTradeMode',
		'stepDown',
		'submitTechList',
		'submitTechOrders',
		'takePresidency',
		'updateRequests',
		'updateVictory'
	);


	function beta5_alliance_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Changes a player's vote
	function setVote($pid,$v) {
		$this->db->query("UPDATE player SET a_vote=$v WHERE id=$pid");
		$this->players[$pid] = null;
	}

	// Marks a tech trading order as obeyed
	public function obeyOrder($pid) {
		$this->db->query("UPDATE tech_trade_order SET obeyed = UNIX_TIMESTAMP( NOW() ) WHERE player = $pid");
	}

	// Get the timestamp of the latest tech trading orders and the next time it'll be possible to
	// change the orders
	public function getLatestTechOrders($alliance) {
		// Get latest submission, if any
		$q = $this->db->query("SELECT MAX(submitted) FROM tech_trade_order WHERE alliance = $alliance");
		list($sub) = dbFetchArray($q);
		$sub = (int) $sub;

		// Get delays
		$interval = 2 * $this->lib->game->ticks['battle']->interval;
		$now = time();
		if ($now - $sub >= $interval) {
			$next = 0;
		} else {
			$next = $sub + $interval;
		}

		return array($sub, $next);
	}
}

?>

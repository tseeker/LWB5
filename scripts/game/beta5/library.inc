<?php

class beta5_library {
	var $index	= array(
		'checkPlanetName',
		'getPlayerStatus',
		'getPlayerCount',
		'investigate',
		'isFinished',
		'leaveGame',
		'leaveVacation',
		'listing',
		'preJoin',
		'preRegister',
		'register',
		'startVacation',
		'updateRankings'
	);

	function beta5_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	/** Checks whether a user plays this version; returns null if he
	 * doesn't or his player ID otherwise.
	 */
	function doesUserPlay($u) {
		$r = $this->db->query("SELECT id FROM player WHERE userid='" . addslashes($u)
			. "' AND (quit IS NULL OR UNIX_TIMESTAMP(NOW())-quit<86400)");
		if (!($r && dbCount($r))) {
			return null;
		}
		list($pid) = dbFetchArray($r);
		return $pid;
	}


	/** Checks whether a user has played this version. */
	function hasPlayed($u) {
		$r = $this->db->query("SELECT id FROM player WHERE userid='$u'");
		return ($r && dbCount($r));
	}


	/** Checks whether it's possible to join this game. */
	function canJoin() {
		$mPlayers = $this->lib->game->params['maxplayers'];
		if ($mPlayers == -1) {
			$q = $this->db->query("SELECT COUNT(*) FROM system WHERE nebula = 0");
			list($mPlayers) = dbFetchArray($q);
		}
		return ( !$mPlayers ) || ( $this->lib->call('getPlayerCount') < $mPlayers);
	}
}

?>

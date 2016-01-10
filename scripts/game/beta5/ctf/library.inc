<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/ctf/library.inc
//
// This library handles everything specific to Kings of the Hill (CTF)
// matches.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_ctf_library {
	var $index	= array (
		'assign',
		'checkTargets',
		'joinMessage',
		'resetGame'
	);

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}

	// This function sends a CTF-specific message to a player
	public function message($player, $mType, $team, $timestamp = null) {
		if (! $this->msgs) {
			$this->msgs = $this->game->getLib('beta5/msg');
		}

		$this->msgs->call('send', $player, 'ctf', array(
			'msg_type'	=> $mType,
			'team'		=> $team,
			'time_stamp'	=> $timestamp
		), $mType ? 'INT' : 'IN');
	}

	// This function checks whether a specific system is a target
	public function isTarget($system) {
		// Cache the targets as this function tends to be called several
		// times in a row
		if (! $this->targets) {
			$q = $this->db->query("SELECT system FROM ctf_target");
			if (!($q && dbCount($q))) {
				return;
			}
			$this->targets = array();
			while ($r = dbFetchArray($q)) {
				array_push($this->targets, $r[0]);
			}
		}

		return in_array($system, $this->targets);
	}

	// This function returns the amount of points a team has
	public function getTeamPoints($team) {
		$q = $this->db->query("SELECT points FROM ctf_points WHERE team = $team");
		if (!($q && dbCount($q))) {
			return 0;
		}
		list($points) = dbFetchArray($q);
		return $points;
	}

	// This function checks for victory
	public function checkVictory() {
		// Get the target count
		$q = $this->db->query("SELECT COUNT(*) FROM ctf_target");
		list($tCount) = dbFetchArray($q);

		// For each team, get the count of targets held without grace
		// for more than <v2time> hours
		$time = $this->game->params['v2time'] * 3600;
		$q = $this->db->query(
			"SELECT held_by, COUNT(*) FROM ctf_target "
				   . "WHERE held_by IS NOT NULL AND grace_expires IS NULL "
				     . "AND held_since <= UNIX_TIMESTAMP(NOW()) - $time "
				. "GROUP BY held_by"
		);

		// If there's only one result and if the count is the same as
		// the target count, we got a winner
		if (dbCount($q) == 1) {
			list($team, $hCount) = dbFetchArray($q);
			if ($hCount == $tCount) {
				return $team;
			}
		}
		return null;
	}
}

?>

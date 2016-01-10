<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/removeTrustingAllies.inc
//
// This action removes a player from other players' trusted allies
// lists.
//
// Parameters:
//	$player		The player to remove from others' lists.
//	$removeList	An array of the player IDs from whose lists
//				the player must be removed
//
// Possible return values:
//	an array	The trusted allies data for the player; see
//				documentation for getTrustedAllies
//	an integer	Error; check the error codes.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_removeTrustingAllies
		extends game_action {

	/* ERROR CODES */
	const playerNotFound		= 0;	// Player not found
	const playerOnVacation		= 1;	// Player is on vacation
	const trustingPlayerNotFound	= 2;	// One of the allies to remove wasn't found
	/***************/

	public function __construct($game) {
		parent::__construct($game, array(
			"players"	=> "beta5/player"
		));
	}

	public function run($player, $removeList) {
		// Check if the player ID is not null
		if (is_null($player)) {
			return self::playerNotFound;
		}
		$player = (int) $player;

		// Check if the player is valid
		$playerRecord = $this->players->call('get', $player);
		if (is_null($playerRecord)) {
			return self::playerNotFound;
		}

		if ($this->players->call('isOnVacation', $player)) {
			return self::playerOnVacation;
		}

		// Check if the player is listed as an ally for these players
		$trustedBy = $this->players->call('isAllyOf', $player);
		$removeList = array_unique($removeList);
		foreach	($removeList as $removePlayer) {
			if (!array_key_exists((int) $removePlayer, $trustedBy)) {
				return self::trustingPlayerNotFound;
			}
		}

		// Remove the player from their lists and reorder
		foreach	($removeList as $removePlayer) {
			$this->players->call('removeAlly', (int) $removePlayer,
				$trustedBy[(int) $removePlayer]['level']);
			$this->players->call('reorderAllies', $removePlayer);
		}

		return $this->game->action("getTrustedAllies", $player);
	}

}

?>

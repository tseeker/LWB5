<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/banTrustingAlly.inc
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


class beta5_banTrustingAlly
		extends game_action {

	/* ERROR CODES */
	const playerNotFound		= 0;	// Player not found
	const playerOnVacation		= 1;	// Player is on vacation
	const emptyName			= 2;	// The name of the player to ban is empty
	const invalidName		= 3;	// The name of the player to ban is invalid
	const targetNotFound		= 4;	// The player to ban could not be found
	const targetIsPlayer		= 5;	// The player is trying to ban himself
	const alreadyBanned		= 6;	// This player has already been banned
	/***************/

	public function __construct($game) {
		parent::__construct($game, array(
			"players"	=> "beta5/player"
		));
	}

	public function run($player, $name) {
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

		// Check the name of the player to ban
		$name = preg_replace('/\s+/', ' ', trim($name));
		if ($name == "") {
			return self::emptyName;
		} elseif (strlen($name) > 15) {
			return self::invalidName;
		}

		// Examine the player to ban
		$toBan = $this->players->call("getPlayerId", $name);
		if (is_null($toBan)) {
			return self::targetNotFound;
		} elseif ($toBan == $player) {
			return self::targetIsPlayer;
		}

		// Check if the target player is already banned
		if ($this->players->call('checkTAListBan', $toBan, $player)) {
			return self::alreadyBanned;
		}

		// Remove the current player from the banned player's TA list
		$reverseList = $this->players->call('isAllyOf', $player);
		foreach ($reverseList as $id => $data) {
			if ($id == $toBan) {
				$this->players->call('removeAlly', $toBan, $reverseList[$toBan]['level']);
				$this->players->call('reorderAllies', $toBan);
				break;
			}
		}

		// Add the ban
		$this->players->call('addTAListBan', $player, $toBan);

		return $this->game->action("getTrustedAllies", $player);
	}

}

?>

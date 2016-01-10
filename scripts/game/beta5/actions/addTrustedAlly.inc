<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/addTrustedAlly.inc
//
// This action adds a player to another player's trusted allies list.
//
// Parameters:
//	$player		Identifier of the player whose list must be
//				changed
//	$allyName	Name of the new trusted ally
//
// Possible return values:
//	an array	The trusted allies data for the player; see
//				documentation for getTrustedAllies
//	an integer	Error; check the error codes.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_addTrustedAlly
		extends game_action {


	/* ERROR CODES */
	const playerNotFound	= 0;		// Player not found
	const playerOnVacation	= 1;		// Player is on vacation
	const noAllyName	= 2;		// No ally name
	const invalidAllyName	= 3;		// Invalid ally name
	const allyNotFound	= 4;		// Ally not found
	const allyIsPlayer	= 5;		// Ally and player are the same
	const allyIsEnemy	= 6;		// New ally is on enemy list
	const playerBlacklisted	= 7;		// Player is banned from the new ally's reverse list
	const allyAlreadyListed	= 8;		// Ally already in the player's list
	const maxPlayerTrust	= 9;		// Player already has 5 trusted allies
	const maxAllyTrust	= 10;		// Ally already is trusted by 5 players
	/***************/


	public function __construct($game) {
		parent::__construct($game, array(
			"players"	=> "beta5/player"
		));
	}

	public function run($player, $allyName) {
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

		// Check if the player is on vacation
		if ($this->players->call('isOnVacation', $player)) {
			return self::playerOnVacation;
		}

		// Check the ally's name
		$allyName = preg_replace('/\s+/', ' ', trim($allyName));
		if ($allyName == "") {
			return self::noAllyName;
		} elseif (strlen($allyName) > 15) {
			return self::invalidAllyName;
		}

		// Check the ally's record
		$ally = $this->players->call('getPlayerId', $allyName);
		if (is_null($ally)) {
			return self::allyNotFound;
		} elseif ($ally == $player) {
			return self::allyIsPlayer;
		}

		// Check the enemy list
		if ($this->players->call('isEnemy', $player, $ally)) {
			return self::allyIsEnemy;
		}

		// Check the blacklist
		if ($this->players->call('checkTAListBan', $ally, $player)) {
			return self::playerBlacklisted;
		}

		// Check the player's current TA list
		$taList = $this->players->call('getAllies', $player);
		if (count($taList) == 5) {
			return self::maxPlayerTrust;
		}
		foreach	($taList as $id => $data) {
			if ($data['id'] == $ally) {
				return self::allyAlreadyListed;
			}
		}

		// Check the reverse TA list
		$taList = $this->players->call('isAllyOf', $ally);
		if (count($taList) == 5) {
			return self::maxAllyTrust;
		}

		// Add to the player's list
		$this->players->call('addAlly', $player, $ally);

		// Return all trusted allies data
		return $this->game->action('getTrustedAllies', $player);
	}

}

?>

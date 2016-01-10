<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/getTrustedAllies.inc
//
// This action returns all data associated with the trusted allies list.
//
// Parameters:
//	$player		Identifier of the player
//
// Possible return values:
//	an array	The trusted allies data for the player
//	NULL		Error, player not found
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_getTrustedAllies
		extends game_action {

	public function __construct($game) {
		parent::__construct($game, array(
			"players"	=> "beta5/player"
		));
	}

	public function run($player) {
		// Check if the player ID is not null
		if (is_null($player)) {
			return null;
		}
		$player = (int) $player;

		// Check if the player is valid
		$playerRecord = $this->players->call('get', $player);
		if (is_null($playerRecord)) {
			return null;
		}

		// Return data
		return array(
			"allies"	=> $this->players->call('getAllies', $player),
			"reverse"	=> $this->players->call('isAllyOf', $player),
			"blacklist"	=> $this->players->call('getTAListBans', $player)
		);
	}

}

?>

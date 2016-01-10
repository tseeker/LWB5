<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/getOverview.inc
//
// This action fetches data associated with the complete overview.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_getOverview
		extends game_action {

	public function __construct($game) {
		parent::__construct($game, array(
			'players'	=> 'beta5/player'
		));
	}

	public function run($player, $language) {
		if (is_null($player)) {
			return null;
		}

		return array(
			"protection"	=> $this->players->call('getProtectionLevel', $player),
			"comms"		=> $this->game->action('getCommsOverview', $player),
			"universe"	=> $this->game->action('getUniverseOverview', $player, $language),
			"empire"	=> $this->game->action('getEmpireOverview', $player)
		);
	}
}

?>

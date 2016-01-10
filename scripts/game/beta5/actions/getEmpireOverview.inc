<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/getEmpireOverview.inc
//
// This action fetches data associated with an empire's overview.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_getEmpireOverview
		extends game_action {

	public function __construct($game) {
		parent::__construct($game, array(
			"planets"	=> "beta5/planet",
			"players"	=> "beta5/player",
			"fleets"	=> "beta5/fleet",
			"techs"		=> "beta5/tech"
		));
	}

	public function run($player) {
		if (is_null($player)) {
			return null;
		}

		// Get general statistics
		$genStats	= $this->planets->call('getStats', $player);
		$fleetStats	= $this->fleets->call('getStats', $player);
		$planets	= $this->players->call('getPlanets', $player);

		// Additional planet data
		$income = 0;
		foreach ($planets as $id => $name) {
			$pInfo = $this->planets->call('byId', $id);
			$pIncome = $this->planets->call('getIncome', $pInfo);
			$income += $pIncome[0];
		}

		// Get research data
		$rPoints = $this->techs->call('getPoints', $player);
		$rBudget = $this->techs->call('getBudget', $player);
		$nNew = count($this->techs->call('getTopics', $player, 0));
		$nForeseen = count($this->techs->call('getTopics', $player, -1)) / 2;

		// Return data
		return array(
			"planetStats"	=> $genStats,
			"planets"	=> $planets,
			"fleetStats"	=> $fleetStats,
			"techStats"	=> array(
				"points"	=> $rPoints,
				"budget"	=> $rBudget,
				"new"		=> $nNew,
				"foreseen"	=> $nForeseen
			),
			"income"	=> $income,
			"profit"	=> $income - $fleetStats['upkeep']
		);
	}

}

?>

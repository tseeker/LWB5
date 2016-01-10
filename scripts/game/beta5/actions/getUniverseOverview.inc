<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/getUniverseOverview.inc
//
// This action fetches data associated with the universe's overview.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_getUniverseOverview
		extends game_action {

	public function __construct($game) {
		parent::__construct($game, array(
			"main"		=> "main",
			"beta5"		=> "beta5",
			"map"		=> "beta5/map",
			"players"	=> "beta5/player",
			"rankings"	=> "main/rankings"
		));
	}

	public function run($player, $language) {
		if (is_null($player)) {
			return null;
		}

		return array(
			"summary"	=> $this->map->call('getUniverse'),
			"ticks"		=> $this->getTicks($language),
			"rankings"	=> $this->getPlayerRankings($player)
		);
	}

	private function getTicks($language) {
		$ticks = array();
		$info = $this->main->call('getTicks', $language);
		foreach ($info as $tid => $td) {
			if (! $td['game']) {
				continue;
			}
			array_push($ticks, array($tid, $td['first'], $td['interval'], $td['last'], $td['name']));
		}
		return $ticks;
	}

	private function getRanking($type, $name) {
		$rt = $this->rankings->call('getType', $type);
		$r = $this->rankings->call('get', $rt, $name);
		if (!$r) {
			$r = array('','');
		}
		return $r;
	}

	private function getPlayerRankings($pid) {
		$pc = $this->beta5->call('getPlayerCount');

		$pName = $this->players->call('getName', $pid);
		$gr = $this->getRanking('p_general', $pName);
		$cr = $this->getRanking('p_civ', $pName);
		$mr = $this->getRanking('p_financial', $pName);
		$fr = $this->getRanking('p_military', $pName);
		$or = $this->getRanking('p_round', $pName);
		$ir = $this->getRanking('p_idr', $pName);

		return array(
			$pc, $gr['points'], $gr['ranking'], $cr['points'], $cr['ranking'],
			$mr['points'], $mr['ranking'], $fr['points'], $fr['ranking'],
			$or['points'], $or['ranking'], $ir['points'], $ir['ranking']
		);
	}
}

?>

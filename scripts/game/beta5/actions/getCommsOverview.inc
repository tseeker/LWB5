<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game actions
//
// beta5/actions/getCommsOverview.inc
//
// This action fetches data associated with the overview of a player's
// communication channels.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_getCommsOverview
		extends game_action {

	public function __construct($game) {
		parent::__construct($game, array(
			"forums"	=> "beta5/forums",
			"msgs"		=> "beta5/msg"
		));
	}

	public function run($player) {
		if (is_null($player)) {
			return null;
		}

		// Messages in default folders
		$folders = array();
		$dfld = array('IN', 'INT', 'OUT');
		foreach ($dfld as $f) {
			$pm = $this->msgs->call('getAll', $player, $f);
			$pmn = $this->msgs->call('getNew', $player, $f);
			$folders[$f] = array($pm, $pmn);
		}

		// Custom folders
		$folders["CUS"] = array();
		$cFold = $this->msgs->call('getCustomFolders', $player);
		foreach	($cFold as $cfid => $cfn) {
			$pm = $this->msgs->call('getAll', $player, 'CUS', $cfid);
			$pmn = $this->msgs->call('getNew', $player, 'CUS', $cfid);
			$folders["CUS"][$cfid] = array($pm, $pmn, $cfn);
		}

		// Forums
		$cats = $this->forums->call('getStructure', $player);
		$forums = array(
			"general"	=> array(),
			"alliance"	=> array()
		);
		foreach	($cats as $c) {
			if (!count($c['forums'])) {
				continue;
			}

			if ($c['type'] == 'A') {
				$forums['allianceID'] = $c['id'];
				foreach	($c['forums'] as $f) {
					array_push($forums['alliance'], array(
						$f['id'], $f['topics'], $f['unread'], $f['title']
					));
				}
			} else {
				$gCat = array(
					"id"		=> $c['id'],
					"type"		=> $c['type'],
					"title"		=> $c['title'],
					"forums"	=> array()
				);
				foreach	($c['forums'] as $f) {
					array_push($gCat['forums'], array(
						$f['id'], $f['topics'], $f['unread'], $f['title']
					));
				}
				array_push($forums['general'], $gCat);
			}
		}

		return array(
			"folders"	=> $folders,
			"forums"	=> $forums
		);
	}

}

?>

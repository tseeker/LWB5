<?php

class page_handler {
	var $engines = array('redirect', 'xml');
	var $defaultEngine = 'redirect';


	private function getRanking($type, $name) {
		$rt = $this->rankings->call('getType', $type);
		$r = $this->rankings->call('get', $rt, $name);
		return $r;
	}


	private function player($id) {
		$pinf = $this->players->call('get', $id);

		// Basic player data
		$data = new data_node('Player');
		$data->setAttribute('id', $id);
		$data->setAttribute('name', $pinf['name']);
		$data->setAttribute('cash', $pinf['cash']);

		// Alliance
		if ($pinf['aid'] || $pinf['arid']) {
			$aData = new data_leaf('Alliance');
			if ($pinf['aid']) {
				$aData->setAttribute('inalliance', 1);
				$tag = $pinf['alliance'];
			} else {
				$aData->setAttribute('inalliance', 0);
				$tag = $pinf['alliance_req'];
			}
			$aData->setAttribute('tag', $tag);
			$data->addContents($aData);
		}

		// Rankings
		$data->addContents($rankings = new data_node('Rankings'));
		$rankings->setAttribute('players', $this->beta5->call('getPlayerCount'));
		$rkTypes = array(
			'p_general'	=> 'General',
			'p_civ'		=> 'Civilian',
			'p_financial'	=> 'Financial',
			'p_military'	=> 'Military',
			'p_round'	=> 'Overall Round',
			'p_idr'		=> 'Inflicted Damage'
		);
		foreach ($rkTypes as $type => $name) {
			$r = $this->getRanking($type, $pinf['name']);
			if (!$r) {
				continue;
			}
			$rt = new data_leaf('Ranking', "$name Ranking");
			$rt->setAttribute('points', $r['points']);
			$rt->setAttribute('rank', $r['ranking']);
			$rankings->addContents($rt);
		}

		return $data;
	}


	private function empire($pid) {
		$empire = new data_node('Empire');

		// Planet statistics
		$empire->addContents($planets = new data_node('Planets'));
		$pld = $this->planets->call('getStats', $pid);
		$planets->setAttribute('count', $pld[0]);
		$planets->setAttribute('avgHap', $pld[1]);
		$planets->setAttribute('totPop', $pld[2]);
		$planets->setAttribute('avgPop', $pld[3]);
		$planets->setAttribute('totFac', $pld[4]);
		$planets->setAttribute('avgFac', $pld[5]);
		$planets->setAttribute('totTur', $pld[6]);
		$planets->setAttribute('avgTur', $pld[7]);
		$planets->setAttribute('siege', $pld[8]);
		$planets->setAttribute('avgCor', $pld[9]);

		// Planet list and income
		$income = 0;
		if ($pld[0] > 0) {
			$planets->addContents($plist = new data_node('List'));
			$l = $this->players->call('getPlanets', $pid);
			foreach ($l as $id => $name) {
				$p = new data_leaf('Planet', $name);
				$p->setAttribute('id', $id);
				$plist->addContents($p);

				$info = $this->planets->call('byId', $id);
				$m = $this->planets->call('getIncome', $pid, $info['pop'], $info['happ'], $info['ifact'], $info['mfact'], $info['turrets'], $info['corruption']);
				$income += $m[0];
			}
		}

		// Fleets
		$empire->addContents($fleets = new data_node('Fleets'));
		$fs = $this->fleets->call('getStats', $pid);
		$fleets->setAttribute('count', $fs['fleets']);
		$fleets->setAttribute('inBattle', $fs['battle']);
		$copy = array('power', 'gaships', 'fighters', 'cruisers', 'bcruisers');
		foreach ($copy as $name) {
			$fleets->setAttribute($name, $fs[$name]);
		}

		// Income and upkeep
		$empire->addContents($budget = new data_node('Budget'));
		$budget->setAttribute('income', $income);
		$budget->setAttribute('upkeep', $fs['upkeep']);
		$budget->setAttribute('profit', $income - $fs['upkeep']);

		// Research
		$empire->addContents($research = new data_node('Research'));
		$research->setAttribute('points', $this->techs->call('getPoints', $pid));
		$research->setAttribute('new', count($this->techs->call('getTopics', $pid, 0)));
		$research->setAttribute('foreseen', count($this->techs->call('getTopics', $pid, -1)) / 2);

		// Research budget
		$research->addContents($rBudget = new data_node('RBudget'));
		$rb = $this->techs->call('getBudget', $pid);
		$attrs = array('fundamental', 'military', 'civilian');
		for ($i=0;$i<count($attrs);$i++) {
			$rBudget->setAttribute($attrs[$i], $rb[$i]);
		}

		return $empire;
	}


	private function ticks() {
		$lib = $this->game->getLib('main');
		$tinf = $lib->call('getTicks', getLanguage());

		$ticks = new data_node('Ticks');
		foreach ($tinf as $tid => $td) {
			if (!$td['game']) {
				continue;
			}

			$ticks->addContents($tick = new data_leaf('Tick', $td['name']));
			$tick->setAttribute('first', $td['first']);
			$tick->setAttribute('interval', $td['interval']);
			if ($td['last']) {
				$tick->setAttribute('last', $td['last']);
			}
		}

		return $ticks;
	}


	private function communications($pid) {
		$comms = new data_node('Communications');
		$comms->addContents($messages = new data_node('Messages'));
		$comms->addContents($forums = new data_node('Forums'));

		// Get custom folders and forum structure
		$cfold = $this->msgs->call('getCustomFolders', $pid);
		$cats = $this->forums->call('getStructure', $pid);

		// Messages in default folders
		$dfld = array('IN', 'INT', 'OUT');
		foreach	($dfld as $f) {
			$messages->addContents($node = new data_leaf('DefaultFolder'));
			$node->setAttribute('id', $f);
			$node->setAttribute('all', $this->msgs->call('getAll', $pid, $f));
			$node->setAttribute('new', $this->msgs->call('getNew', $pid, $f));
		}

		// Custom folders
		foreach	($cfold as $cfid => $cfn) {
			$messages->addContents($node = new data_leaf('CustomFolder', utf8_encode($cfn)));
			$node->setAttribute('id', $cfid);
			$node->setAttribute('all', $this->msgs->call('getAll', $pid, 'CUS', $cfid));
			$node->setAttribute('new', $this->msgs->call('getNew', $pid, 'CUS', $cfid));
		}

		// Forums
		foreach	($cats as $c) {
			if (!count($c['forums'])) {
				continue;
			}

			if ($c['type'] == 'A') {
				$forums->addContents($category = new data_node('AllianceForums'));
			} else  {
				$forums->addContents($category = new data_node('GeneralForums'));
				$category->addContents($cdesc = new data_leaf('Description', utf8_encode($c['title'])));
				$cdesc->setAttribute('id', $c['id']);
				$cdesc->setAttribute('type', $c['type']);
			}
			foreach	($c['forums'] as $f) {
				$category->addContents($forum = new data_leaf('Forum', utf8_encode($f['title'])));
				$forum->setAttribute('id', $f['id']);
				$forum->setAttribute('topics', $f['topics']);
				$forum->setAttribute('unread', $f['unread']);
			}
		}

		return $comms;
	}


	function xml($input) {
		if (!$_SESSION['authok']) {
			return null;
		}

		$pid = $_SESSION[game::sessName()]['player'];

		$this->beta5 = $this->game->getLib('beta5');
		$this->forums = $this->game->getLib('beta5/forums');
		$this->fleets = $this->game->getLib('beta5/fleet');
		$this->msgs = $this->game->getLib('beta5/msg');
		$this->planets = $this->game->getLib('beta5/planet');
		$this->players = $this->game->getLib('beta5/player');
		$this->rankings = $this->game->getLib('main/rankings');
		$this->techs = $this->game->getLib('beta5/tech');

		$data = new data_node('Overview');
		$data->setAttribute('serverTime', time());

		$data->addContents($this->player($pid));
		$data->addContents($this->empire($pid));
		$data->addContents($this->ticks());
		$data->addContents($this->communications($pid));

		return $data;
	}


	function redirect($input) {
		$this->accounts	= $this->game->getLib('main/account');

		if (!is_null($_SESSION['userid'])) {
			$isAdmin = $this->accounts->call('isAdmin', $_SESSION['userid']);
		} else {
			$isAdmin = false;
		}
		return $isAdmin ? "admin" : "overview";
	}
}

?>

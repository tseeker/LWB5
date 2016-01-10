<?php

class beta5_tech_library {
	var $index = array(
		'acceptOffer',
		'checkDependencies',
		'createTree',
		'declineOffer',
		'getAvailableTechs',
		'getBudget',
		'getLaws',
		'getOffers',
		'getPoints',
		'getTopicData',
		'getTopics',
		'getTree',
		'has',
		'implement',
		'makeOffer',
		'switchLaw'
	);


	function beta5_tech_library($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
	}


	// Modifies the research budget
	function setBudget($pid, $ba) {
		$s = join('!', $ba);
		$this->db->query("UPDATE player SET research='$s' WHERE id = $pid");
	}


	// Checks whether a player can make an offer or if he has already done so.
	function checkOffer($pid) {
		$delay = $this->game->ticks['day']->interval - $this->game->ticks['hour']->interval * 2;
		$q = $this->db->query(
			"SELECT id FROM research_assistance "
				. "WHERE player = $pid AND (UNIX_TIMESTAMP(NOW()) - moment) < $delay"
		);
		return (dbCount($q) > 0);
	}


	function getAllDependencies(&$list, $id) {
		if (is_array($list[$id]['all_deps'])) {
			return $list[$id]['all_deps'];
		}

		$list[$id]['all_deps'] = array();
		foreach	($list[$id]['depends_on'] as $dep) {
			if (in_array($dep, $list[$id]['all_deps'])) {
				continue;
			}

			array_push($list[$id]['all_deps'], $dep);
			$ddeps = $this->getAllDependencies($list, $dep);
			foreach	($ddeps as $ddep) {
				if	(in_array($ddep, $list[$id]['all_deps'])) {
					continue;
				}
				array_push($list[$id]['all_deps'], $ddep);
			}
		}
		return $list[$id]['all_deps'];
	}

	// Returns the list of all technologies
	public function getAll($lang) {
		$q = $this->db->query(
			"SELECT t.research, t.name, d.depends_on FROM research_txt t "
				. "LEFT JOIN research_dep d ON d.research = t.research "
				. "WHERE t.lang = '$lang' "
				. "ORDER BY t.research, d.depends_on"
		);

		$list = array();
		while ($r = dbFetchArray($q)) {
			if (!is_array($list[$r[0]])) {
				$list[$r[0]] = array(
					"name"	=> $r[1],
					"deps"	=> array()
				);
			}
			if (!is_null($r[2])) {
				array_push($list[$r[0]]["deps"], $r[2]);
			}
		}

		return $list;
	}
}

?>

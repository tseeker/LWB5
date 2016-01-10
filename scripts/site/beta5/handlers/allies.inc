<?php

class page_handler {
	public $needsAuth = true;
	public $ajax = array(
		'init' => "makeAlliesTooltips();\ninitPage();",
		'func' => array(
			'getTrusted', 'moveAllies', 'removeAllies', 'addAlly',
			'removeRAllies', 'removeBanRAllies',
			'removeBans', 'addBan'
		)
	);


	/********************
	 * GETTING THE LIST *
	 ********************/

	/** This method generates the AJAX data for the lists returned
	 * by the getTrustedAllies() action.
	 */
	private function formatData($taData) {
		$result = array();
		array_push($result, count($taData['allies']) . "#" . count($taData['reverse'])
			. "#" . count($taData['blacklist']));

		foreach ($taData['allies'] as $data) {
			array_push($result, "{$data['id']}#" . utf8entities($data['name']));
		}

		foreach ($taData['reverse'] as $id => $data) {
			array_push($result, "$id#{$data['level']}#" . utf8entities($data['name']));
		}

		foreach ($taData['blacklist'] as $id => $name) {
			array_push($result, "$id#" . utf8entities($name));
		}

		return join("\n", $result);
	}


	/** AJAX callback that returns the list of trusted allies, the
	 * reverse list as well as the blacklist.
	 */
	public function getTrusted() {
		$taData = $this->game->action('getTrustedAllies', $_SESSION[game::sessName()]['player']);
		return $this->formatData($taData);
	}


	/*******************
	 * MANAGING ALLIES *
	 *******************/

	/** AJAX callback to add an ally to the player's list.
	 */
	public function addAlly($name) {
		$result = $this->game->action('addTrustedAlly', $_SESSION[game::sessName()]['player'], $name);
		if (is_array($result)) {
			return $this->formatData($result);
		}

		switch ($result) :
		case beta5_addTrustedAlly::playerNotFound: $error = -1; break;
		case beta5_addTrustedAlly::playerOnVacation: $error = 200; break;
		case beta5_addTrustedAlly::noAllyName: $error = 1; break;
		case beta5_addTrustedAlly::invalidAllyName: $error = 0; break;
		case beta5_addTrustedAlly::allyNotFound: $error = 2; break;
		case beta5_addTrustedAlly::allyIsPlayer: $error = 3; break;
		case beta5_addTrustedAlly::allyIsEnemy: $error = 6; break;
		case beta5_addTrustedAlly::playerBlacklisted: $error = 9; break;
		case beta5_addTrustedAlly::allyAlreadyListed: $error = 4; break;
		case beta5_addTrustedAlly::maxPlayerTrust: $error = 14; break;
		case beta5_addTrustedAlly::maxAllyTrust: $error = 5; break;
		endswitch;

		return "ERR#$error";
	}


	/*************************
	 * MANAGING REVERSE LIST *
	 *************************/

	/** AJAX callback to remove a player from his allies' lists.
	 */
	public function removeRAllies($list) {
		$result = $this->game->action('removeTrustingAllies', $_SESSION[game::sessName()]['player'],
			explode('#', $list));
		if (is_array($result)) {
			return $this->formatData($result);
		}

		switch ($result) :
		case beta5_removeTrustingAllies::playerNotFound: $error = -1; break;
		case beta5_removeTrustingAllies::playerOnVacation: $error = 200; break;
		case beta5_removeTrustingAllies::trustingPlayerNotFound: $error = 7; break;
		endswitch;
		return "ERR#$error";
	}


	/**********************
	 * MANAGING BLACKLIST *
	 **********************/

	public function addBan($name) {
		$result = $this->game->action('banTrustingAlly', $_SESSION[game::sessName()]['player'], $name);
		if (is_array($result)) {
			return $this->formatData($result);
		}

		switch ($result) :
		case beta5_banTrustingAlly::playerNotFound: $error = -1; break;
		case beta5_banTrustingAlly::playerOnVacation: $error = 200; break;
		case beta5_banTrustingAlly::emptyName: $error = 11; break;
		case beta5_banTrustingAlly::invalidName: $error = 15; break;
		case beta5_banTrustingAlly::targetNotFound: $error = 2; break;
		case beta5_banTrustingAlly::targetIsPlayer: $error = 13; break;
		case beta5_banTrustingAlly::alreadyBanned: $error = 12; break;
		endswitch;
		return "ERR#$error";
	}


	// ----------------------------------- OLD CODE BELOW!!!!

	public function findLevels($list) {
		$al = explode('#', $list);
		$l = gameAction('getPlayerAllies', $_SESSION[game::sessName()]['player']);
		$ll = array();
		$i = 0;
		$c = count($l);
		foreach	($al as $id)
		{
			for	($j=0;$j<$c&&$l[$j]['id']!=$id;$j++) ;
			if	($j<$c)
				array_push($ll,$j);
			$i ++;
			if	($i == 5)
				break;
		}
		return	$ll;
	}

	public function moveAllies($list, $dir) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$levels = $this->findLevels($list);
		sort($levels);
		if	(!count($levels))
			return	$this->getTrusted();

		$pid = $_SESSION[game::sessName()]['player'];
		if	($dir == "0")
		{
			$l = gameAction('getPlayerAllies', $pid);
			if	($levels[0] == count($l) - 1)
				return	$this->getTrusted();
			$levels = array_reverse($levels);
		}
		elseif	($levels[0] == 0)
			return	$this->getTrusted();

		$act = 'moveAlly' . (($dir == "0") ? "Down" : "Up");
		foreach	($levels as $l)
			gameAction($act, $pid, $l);

		return	$this->getTrusted();
	}

	public function removeAllies($list) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$levels = $this->findLevels($list);
		if	(count($levels)) {
			$pid = $_SESSION[game::sessName()]['player'];
			foreach	($levels as $l)
				gameAction('removeAlly', $pid, $l);
			gameAction('reorderPlayerAllies', $pid);
		}
		return	$this->getTrusted();
	}

	public function removeBanRAllies($list) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$tb = gameAction('getPlayerIsAlly', $_SESSION[game::sessName()]['player']);
		$pid = $_SESSION[game::sessName()]['player'];
		$al = explode('#', $list);
		foreach	($al as $opid) {
			if	(is_null($tb[$opid]))
				return	"ERR#7";
			elseif	(gameAction('checkTAListBan', $pid, $opid))
				return	"ERR#8";
		}
		foreach	($al as $opid) {
			gameAction('removeAlly', $opid, $tb[$opid]['level']);
			gameAction('reorderPlayerAllies', $opid);
			gameAction('addTAListBan', $pid, $opid);
		}
		return	$this->getTrusted();
	}

	public function removeBans($list) {
		if (gameAction('isOnVacation', $_SESSION[game::sessName()]['player'])) {
			return "ERR#200";
		}

		$pid = $_SESSION[game::sessName()]['player'];
		$tb = gameAction('getTAListBans', $pid);
		$al = explode('#', $list);
		foreach	($al as $opid)
			if	(is_null($tb[$opid]))
				return	"ERR#10";
		foreach	($al as $opid)
			gameAction('delTAListBan', $pid, $opid);
		return	$this->getTrusted();
	}


	/** Main webpage handler.
	 */
	public function handle($input) {
		$this->data = $this->getTrusted();
		$this->output = "allies";
	}
}

?>

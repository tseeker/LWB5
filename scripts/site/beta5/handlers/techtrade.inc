<?php


class page_handler {

	/* Engine data */
	public $needsAuth = true;
	public $ajax = array(
		"func"	=> array(
				// General information
				"getGeneralInfo", "getTechList",
				// Main page
				"getSubmitPage", "submitList", "sendOffer",
				// Requests page
				"getRequestsPage", "submitRequests",
				// Alliance-wide listing
				"getAllianceList", "setListOptions",
				// Viewing orders
				"getCurrentOrders",
				// Changing orders
				"getChangeOrdersData", "submitOrders"
			),
		"init"	=> "new TechTrade(document.getElementById('init-data').value)"
	);

	/* Valid pages */
	static private $subPages = array(
		"Submit", "Req", "ViewList", "SOrders", "VOrders"
	);

	/* Libraries */
	private $pLib;
	private $aLib;
	private $rLib;

	/* Player information */
	private $player;
	private $pInfo;
	private $vacation;
	private $alliance;
	private $hasRequests;
	private $accessLevel;

	/** This function initialises the libraries
	 * and checks the current player's privileges
	 * with regards to technology trading.
	 */
	private function init() {
		// Get libraries
		$this->pLib = $this->game->getLib('beta5/player');
		$this->aLib = $this->game->getLib('beta5/alliance');
		$this->rLib = $this->game->getLib('beta5/tech');

		// Get player information
		$this->player = $_SESSION[game::sessName()]['player'];
		$this->pInfo = $this->pLib->call('get', $this->player);
		$this->vacation = $this->pLib->call('isOnVacation', $this->player);
		$this->alliance = $this->pInfo['aid'];
		$alInfo = $this->aLib->call('get', $this->alliance);
		$this->hasRequests = ($alInfo['enable_tt'] == 'R');
		if (is_null($this->alliance)) {
			$this->accessLevel = 0;
		} else {
			$privileges = $this->aLib->call('getPrivileges', $this->player);
			$this->accessLevel = $privileges['tech_trade'];
		}
	}


	/** This method tries to set the current sub-page being viewed, accounting for
	 * the user's privileges.
	 */
	private function currentPage($page = null) {
		// If we're not trying to change page or if we're trying to set an invalid page,
		// fetch the current page from the session.
		if (is_null($page) || !in_array($page, self::$subPages)) {
			$page = $_SESSION[game::sessName()]['tt_page'];
		}

		// Check privileges
		switch ($page) {
			case 'Submit': $ok = ($this->accessLevel > 0); break;
			case 'Req': $ok = ($this->accessLevel > 1 && $this->hasRequests); break;
			case 'ViewList': $ok = ($this->accessLevel > 2); break;
			case 'SOrders': $ok = ($this->accessLevel > 3); break;
			case 'VOrders': $ok = ($this->accessLevel > 3); break;
			default: $ok = false; break;
		}

		// If the privileges are invalid, set the page
		if (!$ok) {
			if ($this->accessLevel == 0) {
				$page = 'NoAccess';
			} else {
				$page = 'Submit';
			}
		}

		return ($_SESSION[game::sessName()]['tt_page'] = $page);
	}


	/***************************************************************************************
	 * General information retrieval                                                       *
	 ***************************************************************************************/

	/** This AJAX function reads the information about the player and returns
	 * it. It allows the page to "know" what to display.
	 */
	public function getGeneralInfo() {
		// Initialise the handler
		$this->init();

		// Get the current page
		$page = $this->currentPage();

		$requests = ($this->hasRequests ? 1 : 0);
		return "$page#{$this->accessLevel}#$requests#{$this->alliance}#" . ($this->vacation ? 1 : 0);
	}


	/** This AJAX function returns the complete list of technologies as pairs
	 * of ID / Name lines.
	 */
	public function getTechList() {
		// Prevent people from loading the list just for the sake of it
		if ($_SESSION[game::sessName()]['tt_list']) {
			return "";
		}
		$_SESSION[game::sessName()]['tt_list'] = true;

		// Initialise the handler and get the language
		$this->init();
		$language = getLanguage();

		// Get the list
		$techList = $this->rLib->call('getAll', $language);

		// Create the result
		$result = array();
		foreach ($techList as $id => $data) {
			$line = array($id, count($data['deps']));
			$line = array_merge($line, $data['deps']);
			array_push($line, utf8entities($data['name']));
			array_push($result, join('#', $line));
		}

		return join("\n", $result);
	}


	/***************************************************************************************
	 * Submission / view orders page                                                       *
	 ***************************************************************************************/

	/** This is the actual method that generates the output for the submission page.
	 */
	private function __submitPage() {
		$result = array();

		// Get last submission date
		list($canSubmit, $lastSubmission, $nextSubmission) = $this->aLib->call(
				'getTechSubmission', $this->player);
		array_push($result, ($canSubmit ? 1 : 0) . "#$lastSubmission#$nextSubmission");

		// Get orders
		list($sendWhat, $toWhom, $sDate, $oDate) = $this->aLib->call('getTechOrder', $this->player, true);
		if ($toWhom) {
			$toName = $this->pLib->call('getName', $toWhom);
			if (!$oDate) {
				$canSend = !($this->vacation || $this->rLib->call('checkOffer', $this->player)) ? 1 : 0;
			} else {
				$canSend = 0;
			}
			array_push($result, "$sDate#$oDate#$canSend#$sendWhat#$toWhom#" . utf8entities($toName));
		} else {
			array_push($result, "-");
		}

		list($getWhat, $fromWhom, $sDate, $oDate) = $this->aLib->call('getTechOrder', $this->player, false);
		if ($fromWhom) {
			$fromName = $this->pLib->call('getName', $fromWhom);
			array_push($result, "$sDate#$oDate#$getWhat#$fromWhom#" . utf8entities($fromName));
		} else {
			array_push($result, "-");
		}

		return join("\n", $result);
	}


	/** This AJAX function returns the contents for the submission page.
	 */
	public function getSubmitPage() {
		// Initialise page
		$this->init();
		$page = $this->currentPage('Submit');
		if ($page != 'Submit') {
			return "FU";
		}

		return $this->__submitPage();
	}


	/** This AJAX function submits a player's technology list to his alliance
	 */
	public function submitList() {
		// Initialise page
		$this->init();
		$page = $this->currentPage('Submit');
		if ($page != 'Submit') {
			return "FU";
		}
		$result = array();

		// Check if the player can submit
		list($canSubmit, $lastSubmission, $nextSubmission) = $this->aLib->call(
				'getTechSubmission', $this->player);
		if ($this->vacation || ! $canSubmit) {
			return $this->__submitPage();
		}

		// Submit the list
		$this->aLib->call('submitTechList', $this->player);
		return $this->__submitPage();
	}


	/** This AJAX function causes a player to obey his tech trading order.
	 */
	public function sendOffer() {
		// Initialise page
		$this->init();
		$page = $this->currentPage('Submit');
		if ($page != 'Submit') {
			return "FU";
		}
		$result = array();

		// Check if the player can submit
		list($sendWhat, $toWhom, $sDate, $oDate) = $this->aLib->call('getTechOrder', $this->player, true);
		if ($toWhom) {
			$canSend = !($oDate || $this->vacation || $this->rLib->call('checkOffer', $this->player));
			if ($canSend) {
				$this->rLib->call('makeOffer', $this->player, $toWhom, $sendWhat, 0);
				$this->aLib->call('obeyOrder', $this->player);
			}
		}

		return $this->__submitPage();
	}


	/***************************************************************************************
	 * Player requests page                                                                *
	 ***************************************************************************************/

	/** This method generates the actual requests page data.
	 */
	private function __requestsPage() {
		// Get requests
		$requests = $this->aLib->call('getTechRequests', $this->player);

		// Get techs the player could request
		$requestable = $this->rLib->call('getAvailableTechs', $this->player);

		return join('#', $requests) . "\n" . join('#', $requestable);
	}


	/** This AJAX function returns the contents for the requests page.
	 */
	public function getRequestsPage() {
		// Initialise page
		$this->init();
		$page = $this->currentPage('Req');
		if ($page != 'Req') {
			return "FU";
		}

		// Make sure the requests have been updated
		$this->aLib->call('updateRequests', $this->player);
		return $this->__requestsPage();
	}

	/** This AJAX function updates a player's technology requests.
	 */
	public function submitRequests($requests = "") {
		// Initialise page
		$this->init();
		$page = $this->currentPage('Req');
		if ($page != 'Req') {
			return "FU";
		}

		// Parse the list
		$requestable = $this->rLib->call('getAvailableTechs', $this->player);
		$rawList = explode('#', $requests);
		$list = array();
		foreach ($rawList as $techID) {
			$techID = (int) $techID;
			if (in_array($techID, $list) || !in_array($techID, $requestable)) {
				continue;
			}
			array_push($list, $techID);
		}

		// Make sure the requests have been updated
		$this->aLib->call('setTechRequests', $this->player, $list);
		return $this->__requestsPage();
	}


	/***************************************************************************************
	 * Alliance tech list page                                                             *
	 ***************************************************************************************/

	public function __allianceList() {
		// Get the list itself
		$aList = $this->aLib->call('getTechList', $this->alliance, $this->hasRequests);

		// Generate the output string
		$result = array(count($aList));
		foreach ($aList as $player => $techData) {
			array_push($result, "$player#{$techData['vacation']}#{$techData['submitted']}#"
				. "{$techData['restrict']}#" . count($techData['list']) . "#"
				. utf8entities($this->pLib->call('getName', $player)));

			foreach ($techData['list'] as $tech => $status) {
				array_push($result, "$tech#$status");
			}

			if (is_null($techData['requests'])) {
				array_push($result, "");
			} else {
				array_push($result, join('#', $techData['requests']));
			}
		}

		return join("\n", $result);
	}

	public function getAllianceList($oldMD5 = "") {
		// Initialise page
		$this->init();
		$page = $this->currentPage('ViewList');
		if ($page != 'ViewList') {
			return "FU";
		}

		$rVal = $this->__allianceList();
		$newMD5 = md5($rVal);
		$nTechs = prefs::get("beta5/ttTechs", 10);
		$nPlayers = prefs::get("beta5/ttPlayers", 3);
		return ($oldMD5 == $newMD5) ? "NC" : "$newMD5\n$nTechs#$nPlayers\n$rVal";
	}

	public function setListOptions($playersPerPage, $techsPerPage) {
		// Initialise page
		$this->init();
		$page = $this->currentPage('ViewList');
		if ($page != 'ViewList') {
			return;
		}

		$ppp = min(10, max(2, (int) $playersPerPage));
		prefs::set("beta5/ttPlayers", $ppp);

		$tpp = min(8, max(1, floor((int) $techsPerPage / 10))) * 10;
		prefs::set("beta5/ttTechs", $tpp);
	}


	/***************************************************************************************
	 * Order viewing and management                                                        *
	 ***************************************************************************************/

	private function __getOrders() {
		// Get list of orders
		$orders = $this->aLib->call('getTechOrders', $this->alliance, $this->hasRequests);

		// Generate output
		$result = array();
		foreach ($orders as $player => $pRecord) {
			$pName = $this->pLib->call('getName', $player);
			array_push($result, "$player#{$pRecord['sendTo']}#{$pRecord['sendTech']}"
				."#{$pRecord['sendSub']}#{$pRecord['sendDone']}"
				. "#{$pRecord['recvFrom']}#{$pRecord['recvTech']}"
				. "#{$pRecord['recvSub']}#{$pRecord['recvDone']}#" . utf8entities($pName));
		}

		return join("\n", $result);
	}

	public function getCurrentOrders($oldMD5 = "") {
		// Initialise page
		$this->init();
		$page = $this->currentPage('VOrders');
		if ($page != 'VOrders') {
			return;
		}

		$rVal = $this->__getOrders();
		$newMD5 = md5($rVal);
		return ($oldMD5 == $newMD5) ? "NC" : "$newMD5\n$rVal";
	}


	public function getChangeOrdersData($oldMD5 = "") {
		// Initialise page
		$this->init();
		$page = $this->currentPage('SOrders');
		if ($page != 'SOrders') {
			return "FU";
		}

		list($lastSet, $nextSet) = $this->aLib->call('getLatestTechOrders', $this->alliance);
		$rVal = "$lastSet#$nextSet";
		if ($nextSet == 0) {
			$rVal .= "\n" . $this->__allianceList();
		}

		$newMD5 = md5($rVal);
		return ($oldMD5 == $newMD5) ? "NC" : "$newMD5\n$rVal";
	}


	public function submitOrders($orderString) {
		// Initialise page
		$this->init();
		$page = $this->currentPage('SOrders');
		if ($page != 'SOrders') {
			return "FU";
		}

		// If the order string is not empty, extract orders
		$pfx = '';
		if ($orderString != '') {
			$oList = explode('!', $orderString);
			$orders = array();
			foreach ($oList as $oText) {
				$order = explode('#', $oText);
				if (count($order) != 3) {
					$pfx = "ERR";
					break;
				}

				$order[0] = (int)$order[0];
				$order[1] = (int)$order[1];
				$order[2] = (int)$order[2];

				$id = array_shift($order);
				if (array_key_exists($id, $orders)) {
					$pfx = "ERR";
					break;
				}
				$orders[$id] = $order;
			}
			if ($pfx == '') {
				$pfx = $this->aLib->call('submitTechOrders', $this->alliance, $orders) ? '' : 'ERR';
			}
		} else {
			$pfx = 'ERR';
		}

		list($lastSet, $nextSet) = $this->aLib->call('getLatestTechOrders', $this->alliance);
		$rVal = "$lastSet#$nextSet";
		if ($nextSet == 0) {
			$rVal .= "\n" . $this->__allianceList();
		}
		$pfx .= md5($rVal);
		return "$pfx\n$rVal";
	}


	/***************************************************************************************
	 * Main page handler                                                                   *
	 ***************************************************************************************/

	public function handle($input) {
		$_SESSION[game::sessName()]['tt_list'] = false;
		$this->output = "techtrade";
		$this->data = $this->getGeneralInfo();
	}

}


?>

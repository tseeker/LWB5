<?php

class actions_beta5 {
	public $index = array(
		// Overview actions
		"getCommsOverview",
		"getEmpireOverview",
		"getOverview",
		"getUniverseOverview",
		// Trusted allies list
		"addTrustedAlly",
		"banTrustingAlly",
		"getTrustedAllies",
		"removeTrustingAllies",
	);

	var	$rules = array();
	var	$qLen = array();
	var	$formats = array();
	var	$players = array();
	var	$pNames = array();
	var	$pOwners = array();
	var	$pFleets = array();
	var	$pPlanets = array();
	var	$trajCache = array();
	var	$fleets = array();
	var	$avgFPower = null;
	var	$fleetArrivals = array();
	var	$fleetDepartures = array();
	var	$ePower = array();


	function actions_beta5($game) {
		$this->game	= $game;
		$this->lib	= $game->getLib();
		$this->alliance	= $game->getLib('beta5/alliance');
		$this->bq	= $game->getLib('beta5/bq');
		$this->fleets	= $game->getLib('beta5/fleet');
		$this->forums	= $game->getLib('beta5/forums');
		$this->map	= $game->getLib('beta5/map');
		$this->move	= $game->getLib('beta5/moving');
		$this->msgs	= $game->getLib('beta5/msg');
		$this->planets	= $game->getLib('beta5/planet');
		$this->players	= $game->getLib('beta5/player');
		$this->rules	= $game->getLib('beta5/rules');
		$this->sales	= $game->getLib('beta5/sale');
		$this->standby	= $game->getLib('beta5/standby');
		$this->tech	= $game->getLib('beta5/tech');
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// GENERIC FUNCTIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	/** Checks whether a user plays this version; returns null if he
	 * doesn't or his player ID otherwise.
	 */
	function doesUserPlay($u) {
		return $this->lib->call('doesUserPlay', $u);
	}

	/** Checks whether a user has played this version. */
	function hasPlayed($u) {
		return $this->lib->call('hasPlayed', $u);
	}

	/** Get player count. */
	function getPlayerCount() {
		return $this->lib->call('getPlayerCount');
	}

	/** Register to this game */
	function register($uid, $planet, $nick) {
		return $this->lib->call('register', $uid, $planet, $nick);
	}

	function leaveGame($id, $reason) {
		return $this->lib->call('leaveGame', $id, $reason);
	}

	// Called when a player enters vacation mode
	function startVacation($player) {
		return $this->lib->call('startVacation', $player);
	}

	// Called when a player leaves vacation mode
	function leaveVacation($player) {
		return $this->lib->call('leaveVacation', $player);
	}

	// Is the game finished?
	function isFinished() {
		return $this->lib->call('isFinished');
	}

	// Generate a listing for the generic Listing JS component
	function generateListing($data, $conf, $param, $md5) {
		return $this->lib->call('listing', $data, $conf, $param, $md5);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// PLAYER-RELATED FUNCTIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Returns a player's ID using his name to locate him
	function getPlayer($name) {
		return $this->players->call('getPlayerId', $name);
	}

	// Returns player information for the user identified by $uid
	function getPlayerInfo($id, $quitOk = false) {
		return $this->players->call('get', $id, $quitOk);
	}

	// Returns the name of a player's first planet
	function getFirstPlanet($id) {
		return $this->players->call('getFirstPlanet', $id);
	}

	// Returns the name of a player
	function getPlayerName($id) {
		return $this->players->call('getName', $id);
	}

	// Returns the list of a player's planets
	function getPlayerPlanets($pid) {
		return $this->players->call('getPlanets', $pid);
	}

	// Returns the list of a player's fleets
	function getPlayerFleets($pid) {
		return $this->players->call('getFleets', $pid);
	}

	// Returns a player's total fleet power
	function getPlayerFleetPower($pid) {
		return $this->players->call('getPower', $pid);
	}

	// Loads rules for a player
	function loadPlayerRules($pid) {
		return $this->rules->call('get', $pid);
	}

	// Checks whether a player is restrained or not
	function isPlayerRestrained($pid) {
		return $this->players->call('isRestrained', $pid);
	}

	// Transfers cash from an account to another
	function transferFunds($s, $d, $a) {
		return $this->players->call('transferFunds', $s, $d, $a);
	}

	// Checks whether a player is in another's enemy list
	function isPlayerEnemy($pid, $aid) {
		return $this->players->call('isEnemy', $pid, $aid);
	}

	// Checks whether an alliance is in a player's enemy list
	function isAllianceEnemy($pid, $aid) {
		return $this->players->call('isAllianceEnemy', $pid, $aid);
	}

	// Get the list of enemy players
	function getEnemyPlayers($pid) {
		return $this->players->call('getEnemies', $pid);
	}

	// Get the list of enemy alliances
	function getEnemyAlliances($pid) {
		return $this->players->call('getEnemyAlliances', $pid);
	}

	// Remove enemy players from a player's enemy list
	function removeEnemyPlayer($pid, $eid) {
		return $this->players->call('removeEnemy', $pid, $eid);
	}

	// Remove enemy alliances from a player's enemy list
	function removeEnemyAlliance($pid, $eid) {
		return $this->players->call('removeEnemyAlliance', $pid, $eid);
	}

	// Adds a player to the enemy list
	function addEnemyPlayer($pid, $eid) {
		return $this->players->call('addEnemy', $pid, $eid);
	}

	// Adds an alliance to the enemy list
	function addEnemyAlliance($pid, $eid) {
		return $this->players->call('addEnemyAlliance', $pid, $eid);
	}

	// Sets players' fleets to attack mode when they're added to an enemy list
	function makeEnemies($pid, $elist) {
		return $this->players->call('makeEnemies', $pid, $elist);
	}

	// Checks whether a player is in another's ally list
	function isPlayerAlly($pid, $aid) {
		return $this->players->call('isAlly', $pid, $aid);
	}

	// Get the list of a player's trusted allies
	function getPlayerAllies($pid) {
		return $this->players->call('getAllies', $pid);
	}

	// Get the list of players who have a player as a trusted ally
	function getPlayerIsAlly($pid) {
		return $this->players->call('isAllyOf', $pid);
	}

	// Reorders the list of allies after it's been modified
	function reorderPlayerAllies($pid) {
		return $this->players->call('reorderAllies', $pid);
	}

	// Move an ally down the list
	function moveAllyDown($pid, $it) {
		return $this->players->call('moveAllyDown', $pid, $it);
	}

	// Move an ally up the list
	function moveAllyUp($pid, $it) {
		return $this->players->call('moveAllyUp', $pid, $it);
	}

	// Removes an ally from the list
	function removeAlly($pid, $it) {
		return $this->players->call('removeAlly', $pid, $it);
	}

	// Adds an ally to the list
	function addAlly($pid, $aid) {
		return $this->players->call('addAlly', $pid, $aid);
	}

	// Get the list of players in a player's T.A. blacklist
	function getTAListBans($pid) {
		return $this->players->call('getTAListBans', $pid);
	}

	// Adds a player to another's T.A. blacklist
	function addTAListBan($pid, $bpid) {
		return $this->players->call('addTAListBan', $pid, $bpid);
	}

	// Checks a player's T.A. blacklist for another player
	function checkTAListBan($pid, $bpid) {
		return $this->players->call('checkTAListBan', $pid, $bpid);
	}

	// Removes a player from another's T.A. blacklist
	function delTAListBan($pid, $bpid) {
		return $this->players->call('delTAListBan', $pid, $bpid);
	}

	// Returns a summary of the player's diplomatic relations
	function getDiploSummary($pid) {
		return $this->players->call('getDiploSummary', $pid);
	}

	// Returns a player's protection level
	function getProtectionLevel($pid) {
		return $this->players->call('getProtectionLevel', $pid);
	}

	// Marks a player as quitting
	function setPlayerQuit($pid) {
		return $this->players->call('setQuit', $pid);
	}

	// Marks a player as no longer quitting
	function cancelPlayerQuit($pid) {
		return $this->players->call('cancelQuit', $pid);
	}

	// Checks whether a player is currently online
	function isPlayerOnline($pid) {
		return $this->players->call('isOnline', $pid);
	}

	// Checks whether a player is on vacation
	function isOnVacation($pid) {
		return $this->players->call('isOnVacation', $pid);
	}

	// Checks players that have set a player as trusted allies in order to know whether the player can control their fleets
	function checkPlayerAllies($pid) {
		return $this->players->call('checkAllies', $pid);
	}

	// Assign a planet to a player
	function assignPlanet($pid, $p) {
		return $this->player->call('assign', $pid, $p);
	}

	// Assigns the player a new planet after he loses all of his
	function reassignPlanet($player, $name) {
		return $this->players->call('reassign', $player, $name);
	}

	// Returns the amount of planets a player controls
	function getPlanetCount($pl) {
		return $this->players->call('getPlanetCount', $pl);
	}

	// Returns the real number of planets a player owns, not counting planets being sold,
	// abandonned or destroyed
	function getRealPlanetCount($pl) {
		return $this->players->call('getRealPlanetCount', $pl);
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	// RESEARCH AND LAWS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Returns the amount of research points a player gains daily
	function getResearchPoints($pid) {
		return $this->tech->call('getPoints', $pid);
	}

	// Returns an array containing research budget allocations
	function getResearchBudget($pid) {
		return $this->tech->call('getBudget', $pid);
	}

	// Modifies the research budget
	function setResearchBudget($pid, $ba) {
		return $this->tech->call('setBudget', $pid, $ba);
	}

	// Checks whether a player has researched a technology
	function checkPlayerResearch($pid, $rid, $bt = false) {
		return $this->tech->call('has', $pid, $rid, $bt);
	}

	// Checks whether a player has already researched a technology's dependencies
	function checkResearchDependencies($pid, $rid) {
		return $this->tech->call('checkDependencies', $pid, $rid);
	}

	// Get a list of scientific assistance offers sent and received by a player
	function getResearchOffers($pid) {
		return $this->tech->call('getOffers', $pid);
	}

	// Checks whether a player can make an offer or if he has already done so.
	function checkPlayerOffer($pid) {
		return $this->tech->call('checkOffer', $pid);
	}

	// Makes a research offer
	function makeResearchOffer($cpid, $tpid, $tid, $pr) {
		return $this->tech->call('makeOffer', $cpid, $tpid, $tid, $pr);
	}

	// Accepts a research offer
	function acceptResearchOffer($pid, $oid) {
		return $this->tech->call('acceptOffer', $pid, $oid);
	}

	// Declines a research offer
	function declineResearchOffer($pid, $oid) {
		return $this->tech->call('declineOffer', $pid, $oid);
	}

	// Returns the list of research topics for a player; the 'status argument determines
	// the type of list to return: 1 for implemented, 0 for completed, and -1 for "almost
	// completed" (more than 75%)
	function getResearchTopics($pid, $type) {
		return $this->tech->call('getTopics', $pid, $type);
	}

	// Returns data about a research topic: identifier, title, cost, description
	function getResearchData($lang, $id) {
		return $this->tech->call('getTopicData', $lang, $id);
	}

	// Returns the list of laws available to a player, as well as their status
	function getLaws($pl) {
		return $this->tech->call('getLaws', $pl);
	}

	// Implements a technology for a player
	function implementTechnology($pl, $id) {
		return $this->tech->call('implement', $pl, $id);
	}

	// Enacts or revoke a law
	function switchLaw($pl, $id) {
		return $this->tech->call('switchLaw', $pl, $id);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// MAP FUNCTIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Returns statistics about the whole universe
	function getUniverseSummary() {
		return $this->map->call('getUniverse');
	}

	// Returns statistics about a protection zone
	function getProtZoneSummary($pz) {
		return $this->map->call('getProtectionZone', $pz);
	}

	// Returns data about a system at specified coordinates
	function getSystemAt($x, $y) {
		return $this->map->call('at', $x, $y);
	}

	// Returns map data for planets in a specified system
	function getSystemPlanets($sid) {
		return $this->map->call('getSystem', $sid);
	}

	// Get planets within a distance of a set of coordinates
	function getPlanetsAround($x,$y,$d) {
		return $this->map->call('getPlanetsAround', $x, $y, $d);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// MARKETPLACE FUNCTIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	function getSales($pList) {
		return $this->sales->call('getPublicSales', $pList);
	}

	function getDirectSales($pid) {
		return $this->sales->call('getDirectSales', $pid);
	}

	function getSentOffers($pid) {
		return $this->sales->call('getSentOffers', $pid);
	}

	function getSalesHistoryFrom($player) {
		return $this->sales->call('getHistoryFrom', $player);
	}

	function getSalesHistoryTo($player) {
		return $this->sales->call('getHistoryTo', $player);
	}

	function getPlanetSale($id) {
		return $this->sales->call('getPlanetSale', $id);
	}

	function getFleetSale($sId) {
		return $this->sales->call('getFleetSale', $sId);
	}

	function newSale($player, $public, $auction, $expires, $price, $target, $planet, $fleet) {
		return $this->sales->call('sell', $player, $public, $auction, $expires, $price, $target, $planet, $fleet);
	}

	function cancelTransfer($pid,$id) {
		return $this->sales->call('cancelTransfer', $pid,$id);
	}

	function cancelSale($pid,$id) {
		return $this->sales->call('cancel', $pid,$id);
	}

	function isDirectOffer($pid, $oid) {
		return $this->sales->call('isDirectOffer', $pid, $oid);
	}

	function buy($pid, $offer) {
		return $this->sales->call('buy', $pid, $offer);
	}

	function placeBid($pid, $oid, $price) {
		return $this->sales->call('bid', $pid, $oid, $price);
	}

	function declinePrivate($id) {
		return $this->sales->call('decline', $id);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// FLEETS MANAGEMENT FUNCTIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Returns all of the locations at which a player has stationned fleets
	function getFleetLocations($pid) {
		return $this->fleets->call('getPlayerLocations', $pid);
	}

	// Returns the list of fleets in orbit around a planet
	function getFleetsAt($pid, $pl = null) {
		return $this->fleets->call('getLocation', $pid, $pl);
	}

	// Invalidates the fleet cache
	function invFleetCache($id = null) {
		return $this->fleets->call('invCache', $id);
	}

	// Returns data regarding a fleet
	function getFleet($id) {
		return $this->fleets->call('get', $id);
	}

	// Returns data regarding current fleets
	function getFleetStats($pid) {
		return $this->fleets->call('getStats', $pid);
	}

	// Computes a fleet's power
	function getFleetPower($pl, $t, $g, $f, $c, $b) {
		// FIXME: turrets
		$tp = $this->planets->call('getPower', $pl, $t);
		return $this->fleets->call('getPower', $pl, $g, $f, $c, $b) + $tp;
	}

	// Computes a fleet's upkeep
	function getFleetUpkeep($pl, $g, $f, $c, $b) {
		return $this->fleets->call('getUpkeep', $pl, $g, $f, $c, $b);
	}

	// Switches a fleet's status
	function switchFleetStatus($id) {
		return $this->fleets->call('switchStatus', $id);
	}

	// Handles a fleet's arrival
	function fleetArrival($fid, $dest, $from, $nStatus = null) {
		return $this->fleets->call('arrival', $fid, $dest, $from, $nStatus);
	}


	// Changes a fleet's orders
	function setFleetOrders($fid, $newDest, $newDelay, $attack = null) {
		return $this->fleets->call('setOrders', $fid, $newDest, $newDelay, $attack);
	}


	// Sends messages related to fleet movements
	function sendFleetMoveMessages() {
		return $this->fleets->call('sendMoveMessages');
	}


	// Merge fleets
	function mergeFleets($fIds, $okOwners, $newName) {
		return $this->fleets->call('merge', $fIds, $okOwners, $newName);
	}


	// Automatically split a fleet
	function splitFleetAuto($fid, $newName, $count) {
		return $this->fleets->call('autoSplit', $fid, $newName, $count);
	}


	// Manually split a fleet in player-specified fleets
	function splitFleetManually($fid, $newName, $count, $sg, $sf, $sc, $sb) {
		return $this->fleets->call('split', $fid, $newName, $count, $sg, $sf, $sc, $sb);
	}


	// Disbands a fleet and removes any sales / movement / stand-by
	// entries associated with it
	function disbandFleet($fId, $final = false) {
		return $this->fleets->call('disband', $fId, $final);
	}

	// Renames a fleet
	function renameFleet($fid, $name) {
		return $this->fleets->call('rename', $fid, $name);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// MOVING OBJECTS MANAGEMENT FUNCTIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Generate a new moving object entry
	function newMovingObject($srcId, $dstId, $speed, $cruisers, $wait) {
		return $this->move->call('newObject', $srcId, $dstId, $speed, $cruisers, $wait);
	}

	// Clone a moving object
	function cloneMovingObject($oid) {
		return $this->move->call('cloneObject', $oid);
	}

	// Get a moving object's current location
	function getObjectLocation($oid) {
		return $this->move->call('getLocation', $oid);
	}

	// Redirects a moving object to a new destination
	function redirectObject($oid, $newDest, $speed, $cruisers, $newWait) {
		return $this->move->call('redirect', $oid, $newDest, $speed, $cruisers, $newWait);
	}

	// Stop a moving object
	function stopMovement($oid, $newWait) {
		return $this->move->call('stop', $oid, $newWait);
	}

	// Get an object's current trajectory
	function getObjectTrajectory($oid) {
		return $this->move->call('getTrajectory', $oid);
	}

	// Compute an object's trajectory
	function getTrajectory($srcId, $dstId, $speed, $cruisers) {
		return $this->move->call('computeTrajectory', $srcId, $dstId, $speed, $cruisers);
	}


	// Generates a new Hyperspace stand-by order
	function newHSWait($time, $location,$origin = null,$spent = null) {
		return $this->standby->call('create', $time, $location, $origin, $spent);
	}


	// Checks whether fleets can be destroyed while waiting in hyperspace at a given location
	function waitCanDestroy($location, $owner) {
		return $this->standby->call('canDestroy', $location, $owner);
	}


	// Computes the probability for fleet destruction when standing by in Hyperspace
	function waitGetLossProb($timeSpent) {
		return $this->standby->call('getLossProb', $timeSpent);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// PLANET MANAGEMENT
	//--------------------------------------------------------------------------------------------------------------------------------

	// Renames a planet
	function renamePlanet($pid, $name) {
		return $this->planets->call('rename', $pid, $name);
	}

	// Returns data regarding a planet with a specified name
	function getPlanetByName($n) {
		return $this->planets->call('byName', $n);
	}

	// Returns data regarding a planet with a specified identifier
	function getPlanetById($id) {
		return $this->planets->call('byId', $id);
	}

	// Updates a planet's maximum population
	function updateMaxPopulation($id, $formerOwner, $newOwner) {
		return $this->planets->call('updateMaxPopulation', $id, $formerOwner, $newOwner);
	}

	// Gets a planet's owner
	function getPlanetOwner($pid) {
		return $this->planets->call('getOwner', $pid);
	}

	// Computes a planet's income
	function getPlanetIncome($owner, $pop, $happ, $ifact, $mfact, $turrets, $corruption) {
		return $this->planets->call('getIncome', $owner, $pop, $happ, $ifact, $mfact, $turrets, $corruption);
	}

	// Build factories on a planet
	function buildFactories($id, $nb, $t) {
		return $this->planets->call('buildFactories', $id, $nb, $t);
	}

	// Check a planet to see whether it's possible to destroy factories
	function checkDestroyFactories($id, $nb, $t) {
		return $this->planets->call('checkDestroyFactories', $id, $nb, $t);
	}

	// Try to destroy turrets on a planet
	function destroyTurrets($id, $nb) {
		return $this->planets->call('destroyTurrets', $id, $nb);
	}

	// Destroy factories on a planet
	function destroyFactories($id, $nb, $t) {
		return $this->planets->call('destroyFactories', $id, $nb, $t);
	}

	// Updates a planet's factory history and delete old history records
	function updateFactHistory($pid, $mil, $nb) {
		return $this->planets->call('updateFactHistory', $pid, $mil, $nb);
	}

	// Updates happiness for a planet
	function updateHappiness($id) {
		return $this->planets->call('updateHappiness', $id);
	}

	// Abandon a planet
	function setPlanetAbandon($pid, $start = true) {
		return $this->planets->call('setAbandon', $pid, $start);
	}

	// Destroy a planet
	function setPlanetBoom($pid, $start = true) {
		return $this->planets->call('setBoom', $pid, $start);
	}

	function updateAttackStatus($planet) {
		return $this->planets->call('updateMilStatus', $planet);
	}

	// Returns data regarding current planets
	function getPlanetStats($pid) {
		return $this->planets->call('getStats', $pid);
	}


	// Returns the player's empire-wide probe policy
	function getPlayerPolicy($id) {
		$q = dbQuery("SELECT probe_policy FROM player WHERE id=$id");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($r) = dbFetchArray($q);
		return $r;
	}

	// Sets the player's empire-wide probe policy
	function setPlayerPolicy($id, $pol) {
		dbQuery("UPDATE player SET probe_policy='$pol' WHERE id=$id");
	}

	// Returns a planet's probe policy
	function getPlanetPolicy($id) {
		$q = dbQuery("SELECT probe_policy FROM planet WHERE id=$id");
		if (!($q && dbCount($q) == 1)) {
			return null;
		}
		list($r) = dbFetchArray($q);
		return $r;
	}

	// Sets a planet's probe policy
	function setPlanetPolicy($id, $pol) {
		$ps = is_null($pol) ? "NULL" : "'$pol'";
		dbQuery("UPDATE planet SET probe_policy=$ps WHERE id=$id");
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// BUILD QUEUE MANAGEMENT
	//--------------------------------------------------------------------------------------------------------------------------------

	// Returns the contents of a planet's build queue
	function getBuildQueue($id) {
		return $this->bq->call('get', $id);
	}

	// Flush a planet's build queue
	function flushBuildQueue($id) {
		return $this->bq->call('flush', $id);
	}


	// Add items to a planet's build queue
	function addToBuildQueue($id, $nb, $type) {
		return $this->bq->call('append', $id, $nb, $type);
	}

	// Removes an item from a build queue
	function deleteBuildQueueItem($pid, $item) {
		return $this->bq->call('remove', $pid, $item);
	}

	// Reorders the build queue
	function reorderBuildQueue($pid) {
		return $this->bq->call('reorder', $pid);
	}

	// Computes the cost of replacing a set of build queue items
	function getReplacementCost($pid, $items, $rCost) {
		return $this->bq->call('getReplacementCost', $pid, $items, $rCost);
	}

	// Replaces items in a build queue
	function replaceItems($pid, $items, $nb, $type) {
		return $this->bq->call('replace', $pid, $items, $nb, $type);
	}

	// Returns the length of a planet's build queue
	function getQueueLength($pl) {
		return $this->bq->call('getLength', $pl);
	}

	// Moves an item down the build queue
	function moveItemDown($pl, $it) {
		return $this->bq->call('moveDown', $pl, $it);
	}

	// Moves an item up the build queue
	function moveItemUp($pl, $it) {
		return $this->bq->call('moveUp', $pl, $it);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// ALLIANCE MANAGEMENT
	//--------------------------------------------------------------------------------------------------------------------------------

	// Get alliance identifier by tag
	function getAlliance($tag) {
		return $this->alliance->call('getId', $tag);
	}

	// Get alliance data
	function getAllianceInfo($id) {
		return $this->alliance->call('get', $id);
	}

	// Create an alliance
	function createAlliance($tag, $name, $founder) {
		return $this->alliance->call('create', $tag, $name, $founder);
	}

	// Get a list of IDs for the players having a role equivalent to Hyperiums' "Keepers"
	function getAllianceKeepers($a) {
		return $this->alliance->call('getKeepers', $a);
	}

	// Get the list of all people in an alliance that can vote, as well as the current leader
	function getAllianceVoters($a) {
		return $this->alliance->call('getVoters', $a);
	}

	// Sends a request to join an alliance
	function sendJoinRequest($p, $a) {
		return $this->alliance->call('sendRequest', $p, $a);
	}

	// Cancels a request to join an alliance
	function cancelJoinRequest($p, $a) {
		return $this->alliance->call('cancelRequest', $p, $a);
	}

	// Reads the list of ranks associated with an alliance
	function getAllianceRanks($aid) {
		return $this->alliance->call('getRanks', $aid);
	}

	// Returns the privileges associated with a rank
	function getRankPrivileges($id) {
		return $this->alliance->call('getRankPrivileges', $id);
	}

	// Returns the list of privileges a player has inside an alliance
	function getAlliancePrivileges($p) {
		return $this->alliance->call('getPrivileges', $p);
	}

	// Switches an alliance's government mode
	function setAllianceDemo($aid, $demo) {
		return $this->alliance->call('setDemocratic', $aid, $demo);
	}

	// Changes the successor for the current leader
	function setAllianceSuccessor($aid, $sid) {
		return $this->alliance->call('setSuccessor', $aid, $sid);
	}

	// Makes the alliance leader step down from power
	function allianceStepDown($aid, $isLeave = false) {
		return $this->alliance->call('stepDown', $aid, $isLeave);
	}

	// Leave an alliance
	function leaveAlliance($pid, $isKick = false) {
		return $this->alliance->call('leave', $pid, $isKick);
	}

	// Get the list of pending requests for an alliance
	function getAllianceRequests($aid) {
		return $this->alliance->call('getRequests', $aid);
	}

	// Accepts a request to join an alliance
	function acceptAllianceRequest($aid, $pid, $kid) {
		return $this->alliance->call('acceptRequest', $aid, $pid, $kid);
	}

	// Rejects a request to join an alliance
	function rejectAllianceRequest($aid, $pid, $kid) {
		return $this->alliance->call('rejectRequest', $aid, $pid, $kid);
	}

	// Returns the list of planets belonging to players of the alliance
	function getAlliancePlanets($aid) {
		return $this->alliance->call('getPlanets', $aid);
	}

	// Reads the list of alliance members and their ranks
	function getAllianceMembers($aid) {
		return $this->alliance->call('getMembers', $aid);
	}

	// Get the list of alliance candidates as well as the number of votes they have
	function getAllianceCandidates($aid) {
		return $this->alliance->call('getCandidates', $aid);
	}

	// Changes a player's vote
	function setAllianceVote($pid,$v) {
		return $this->alliance->call('setVote', $pid,$v);
	}

	// Adds a new candidate for an alliance's presidency
	function newAllianceCandidate($aid,$pid) {
		return $this->alliance->call('addCandidate', $aid,$pid);
	}

	// Removes a candidate for an alliance's presidency
	function removeAllianceCandidate($aid,$pid) {
		return $this->alliance->call('removeCandidate', $aid,$pid);
	}

	// Makes a player the new president for an alliance
	function takePresidency($aid, $pid) {
		return $this->alliance->call('takePresidency', $aid, $pid);
	}

	// Reads the list of an alliance's forums
	function getAllianceForums($aid) {
		return $this->alliance->call('getForums', $aid);
	}

	// Reads the list of an alliance's forums, with details
	function getAllianceForumsComplete($aid) {
		return $this->alliance->call('getForumsComplete', $aid);
	}

	// Creates an alliance forum
	function newAllianceForum($aid, $name, $userPost, $after, $description) {
		return $this->alliance->call('createForum', $aid, $name, $userPost, $after, $description);
	}

	// Destroys an alliance forum
	function deleteAllianceForum($id) {
		return $this->alliance->call('deleteForum', $id);
	}

	// Moves an alliance forum up or down in the list
	function moveAllianceForum($id, $up) {
		return $this->alliance->call('moveForum', $id, $up);
	}

	// Modifies an alliance forum
	function modifyAllianceForum($id, $name, $userPost, $description) {
		return $this->alliance->call('modifyForum', $id, $name, $userPost, $description);
	}

	// Modifies the access list for an alliance forum
	function setForumAccess($id, $readers, $mods) {
		return $this->alliance->call('setForumAccess', $id, $readers, $mods);
	}

	// Creates a new rank within the alliance
	function createAllianceRank($aid, $name, $privs, $kick, $change, $fread, $fmod) {
		return $this->alliance->call('createRank', $aid, $name, $privs, $kick, $change, $fread, $fmod);
	}

	// Modifies a new rank within the alliance
	function modifyAllianceRank($rid, $name, $privs, $kick, $change, $fread, $fmod) {
		return $this->alliance->call('modifyRank', $rid, $name, $privs, $kick, $change, $fread, $fmod);
	}

	// Deletes a rank from an alliance's ACL
	function deleteAllianceRank($aid, $rId, $nrId) {
		return $this->alliance->call('deleteRank', $aid, $rId, $nrId);
	}

	// Get the number of alliance members at a certain rank
	function getPCountRank($aid, $rId) {
		return $this->alliance->call('getRankSize', $aid, $rId);
	}

	// Change an alliance member's rank
	function changeMemberRank($pid, $rid) {
		return $this->alliance->call('changeRank', $pid, $rid);
	}

	// Kick an alliance member
	function kickAllianceMember($pid) {
		return $this->alliance->call('kick', $pid);
	}

	// Get the list of planets under attack in an alliance
	function getAllianceMilitary($aid) {
		return $this->alliance->call('getMilitary', $aid);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// MESSAGES AND FOLDERS
	//--------------------------------------------------------------------------------------------------------------------------------

	// Lists custom folders for a player
	function getCustomFolders($pid) {
		return $this->msgs->call('getCustomFolders', $pid);
	}

	// Creates a custom folder
	function createCustomFolder($pid, $name) {
		return $this->msgs->call('createFolder', $pid, $name);
	}

	// Renames a custom folder
	function renameCustomFolder($fid, $name) {
		return $this->msgs->call('renameFolder', $fid, $name);
	}

	// Flushes messages from a folder
	function flushFolder($pid, $ft, $fid) {
		return $this->msgs->call('flushFolder', $pid, $ft, $fid);
	}

	// Deletes a custom folder
	function deleteCustomFolder($pid, $fid) {
		return $this->msgs->call('deleteFolder', $pid, $fid);
	}

	// Get count of all messages in a player's folder
	function getAllMessages($pid, $ft, $cfid = null) {
		return $this->msgs->call('getAll', $pid, $ft, $cfid);
	}

	// Get count of new messages in a player's folder
	function getNewMessages($pid, $ft = null, $cfid = null) {
		return $this->msgs->call('getNew', $pid, $ft, $cfid);
	}

	// Get headers for all messages inside a folder
	function getMessageHeaders($pid, $fld, $cfid) {
		return $this->msgs->call('getHeaders', $pid, $fld, $cfid);
	}

	// Returns complete information about a message
	function getCompleteMessage($mId, $pid) {
		return $this->msgs->call('get', $mId, $pid);
	}

	// Sets a message status to "read"
	function setMessageRead($mId) {
		return $this->msgs->call('setRead', $mId);
	}

	// Sets a message as deleted
	function deleteMessage($mId, $pId) {
		return $this->msgs->call('delete', $mId, $pId);
	}

	// Moves a message
	function moveMessage($mId, $pId, $tTp, $tId) {
		return $this->msgs->call('move', $mId, $pId, $tTp, $tId);
	}

	// Sends a message from player to player
	function sendMessagePlayer($src, $dst, $sub, $msg, $rep = null) {
		return $this->msgs->call('sendToPlayer', $src, $dst, $sub, $msg, $rep);
	}

	// Sends a message from player to planet
	function sendMessagePlanet($src, $dst, $sub, $msg, $rep = null) {
		return $this->msgs->call('sendToPlanet', $src, $dst, $sub, $msg, $rep);
	}

	// Sends a message from a player to his own alliance
	function sendMessageAlliance($src, $dst, $sub, $msg, $rep = null) {
		return $this->msgs->call('sendInAlliance', $src, $dst, $sub, $msg, $rep);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// FORUMS
	//--------------------------------------------------------------------------------------------------------------------------------

	function getCatsAndForums($player) {
		return $this->forums->call('getStructure', $player);
	}

	function getForumTopics($f, $first, $count) {
		return $this->forums->call('getTopics', $f, $first, $count);
	}

	function newForumTopic($a, $fid, $sub, $txt, $ec, $es, $st) {
		return $this->forums->call('newTopic', $a, $fid, $sub, $txt, $ec, $es, $st);
	}

	function getForumTopic($tid) {
		return $this->forums->call('getTopic', $tid);
	}

	function getForumPosts($tid, $thr, $o, $cnt, $fst) {
		return $this->forums->call('getPosts', $tid, $thr, $o, $cnt, $fst);
	}

	function getForumPost($pid) {
		return $this->forums->call('getPost', $pid);
	}

	function getForumLatest($aForums, $gForums, $nb, $first) {
		return $this->forums->call('getLatest', $aForums, $gForums, $nb, $first);
	}

	function forumSearchPosts($aForums, $gForums, $nb, $first, $text, $complete, $sField, $sOrder) {
		return $this->forums->call('searchPosts', $aForums, $gForums, $nb, $first, $text, $complete, $sField, $sOrder);
	}

	function forumSearchTopics($aForums, $gForums, $nb, $first, $text, $complete, $sField, $sOrder) {
		return $this->forums->call('searchTopics', $aForums, $gForums, $nb, $first, $text, $complete, $sField, $sOrder);
	}

	function postForumReply($a, $post, $sub, $txt, $ec, $es) {
		return $this->forums->call('reply', $a, $post, $sub, $txt, $ec, $es);
	}

	function editForumPost($a, $pid, $sub, $txt, $ec, $es) {
		return $this->forums->call('edit', $a, $pid, $sub, $txt, $ec, $es);
	}

	function isTopicRead($topic, $player) {
		return $this->forums->call('isRead', $topic, $player);
	}

	function markAsRead($topic, $player) {
		return $this->forums->call('markRead', $topic, $player);
	}

	function markAsUnread($topic, $player) {
		return $this->forums->call('markUnread', $topic, $player);
	}

	function updateForumLast($forum) {
		return $this->forums->call('updateLast', $forum);
	}

	function deleteForumTopic($forum, $topic) {
		return $this->forums->call('deleteTopic', $forum, $topic);
	}

	function switchTopicSticky($forum, $topic) {
		return $this->forums->call('switchSticky', $forum, $topic);
	}

	function moveForumTopic($forum, $topic, $dest, $user) {
		return $this->forums->call('moveTopic', $forum, $topic, $dest, $user);
	}

	function deleteSinglePost($postId) {
		return $this->forums->call('deletePost', $postId);
	}

	function markForumAsRead($fid, $pid) {
		return $this->forums->call('markForumRead', $fid, $pid);
	}

	// Get the amount of read topics in an alliance forum
	function getReadTopics($fid, $pid) {
		return $this->forums->call('getRead', $fid, $pid);
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	// ECM/ECCM COMPUTATIONS
	//--------------------------------------------------------------------------------------------------------------------------------

	function getInformationLevel($ecm, $eccm) {
		return $this->ecm->call('getInformationLevel', $ecm, $eccm);
	}

	function scrambleFleetPower($level, $power) {
		return $this->ecm->call('scrambleFleetPower', $level, $power);
	}
}

?>

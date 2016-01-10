<?

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/prot/library.inc
//
// This library handles everything related to protection in round
// games.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_prot_library {


	public $index = array (
		'checkSystem',
		'isPlayerMarked',
		'updateFleets'
	);


	/** The identifier of the "Peacekeepers" player */
	private $pkPlayerID = null;

	/** The list of actions to execute */
	private $actions = array();

	/** The current time */
	private $now;


	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->msgs	= $this->game->getLib('beta5/msg');
		$this->now	= time();
	}


	/** This function returns the identifier of the Peacekeepers player.
	 */
	public function getPeacekeepers() {
		if (is_null($this->pkPlayerID)) {
			$q = $this->db->query(
				"SELECT p.id FROM player p "
					. "LEFT JOIN account a ON a.id = p.userid "
					. "WHERE a.name = 'AI>Peacekeepers'"
			);
			list($this->pkPlayerID) = dbFetchArray($q);
		}
		return $this->pkPlayerID;
	}


	/** This function adds actions to the list of actions to perform.
	 */
	public function addActions($actions) {
		array_push($this->actions, $actions);
	}


	/** This function executes all actions listed during the latest checks.
	 */
	public function flushStatus() {
		if (!empty($this->actions)) {
			$this->flushActions();
		}
		$this->lib->call('updateFleets');
	}

	private function flushActions() {
		$actions = $this->buildActualActions();
		foreach ($actions['enemy'] as $enemy) {
			$this->declareEnemy($enemy);
		}

		$dMessages = $wMessages = $dSystems = array();
		$wDelay = $this->now + $this->game->ticks['battle']->interval;
		foreach ($actions['other'] as $action) {
			switch ($action['type']) {
			    case 'A':
				$delay = 'NULL';
				break;
			    case 'O':
				$dMessages[$action['player']][] = $action['system'];
				$delay = 'NULL';
				break;
			    case 'W':
				$wMessages[$action['player']][] = $action['system'];
				$delay = $wDelay;
				break;
			}
			if ($action['mustInsert']) {
				$qString = 'INSERT INTO pk_sys_status (system, player, status, switch_at) VALUES ('
					. "{$action['system']}, {$action['player']}, '{$action['type']}', $delay)";
			} else {
				$qString = "UPDATE pk_sys_status SET status = '{$action['type']}', switch_at = $delay "
					. "WHERE system = {$action['system']} AND player = {$action['player']}";
			}
			$this->db->query($qString);
		}

		foreach ($dMessages as $player => $systems) {
			$this->sendMessages('D', $player, $systems);
		}
		foreach ($wMessages as $player => $systems) {
			$this->sendMessages('W', $player, $systems, $wDelay);
		}

		$this->actions = array();
	}


	/** This method merges all records received from system checks.
	 */
	private function buildActualActions() {
		// First merge the lists of players to add as enemies
		$enemies = array();
		foreach ($this->actions as $action) {
			$enemies = array_merge($enemies, $action['enemy']);
		}
		$enemies = array_unique($enemies);

		// List all actions to be executed, unless the player
		// has been declared an enemy
		$actions = array();
		foreach ($this->actions as $action) {
			foreach ($action['other'] as $aRecord) {
				if (!in_array($aRecord['player'], $enemies)) {
					array_push($actions, $aRecord);
				}
			}
		}

		return array(
			"enemy"		=> $enemies,
			"other"		=> $actions
		);
	}


	/** This method declares a player as an enemy of the peacekeepers.
	 */
	private function declareEnemy($playerID) {
		$howLong = $this->now + 20 * $this->game->ticks['day']->interval;

		$q = $this->db->query(
			"SELECT DISTINCT p.id, p.name, p.system FROM planet p, fleet f, system s "
				. "WHERE f.location = p.id AND p.system = s.id AND f.owner = $playerID AND s.prot > 0"
		);
		$planets = array();
		$systems = array();
		while ($r = dbFetchArray($q)) {
			$planets[$r[0]] = $r[1];
			if (!in_array($r[2], $systems)) {
				array_push($systems, $r[2]);
			}
		}

		$this->db->query("INSERT INTO pk_enemy (player, until) VALUES ($playerID, $howLong)");

		$q = $this->db->query("SELECT system FROM pk_sys_status WHERE player = $playerID");
		$inSystems = array();
		while ($r = dbFetchArray($q)) {
			$inSystems[] = $r[0];
		}
		foreach ($systems as $systemID) {
			if (in_array($systemID, $inSystems)) {
				continue;
			}
			$this->db->query(
				"INSERT INTO pk_sys_status (player, system, status) "
					. "VALUES ($playerID, $systemID, 'O')"
			);
		}
		$this->db->query("UPDATE pk_sys_status SET status = 'O' WHERE player = $playerID");

		$msgID = $this->msgs->call('send', $playerID, "pkwarning", array(
			'msg_type'	=> 'E',
			'delay'		=> $howLong
		));

		$pInsert = new db_copy('pkwarning_planet', db_copy::copyTo);
		$pInsert->setAccessor($this->db);
		foreach ($planets as $id => $name) {
			$pInsert->appendRow(array($msgID, $id, $name));
		}
		$pInsert->execute();
	}


	/** This method sends messages to players.
	 */
	private function sendMessages($type, $player, $systems, $delay = null) {
		$q = $this->db->query(
			"SELECT DISTINCT p.id, p.name FROM planet p, fleet f "
				. "WHERE f.location = p.id AND p.system IN (" . join(',', $systems) . ") "
				  . "AND f.owner = $player"
		);
		$planets = array();
		while ($r = dbFetchArray($q)) {
			$planets[$r[0]] = $r[1];
		}

		$msgID = $this->msgs->call('send', $player, "pkwarning", array(
			'msg_type'	=> $type,
			'delay'		=> $delay
		));

		$pInsert = new db_copy('pkwarning_planet', db_copy::copyTo);
		$pInsert->setAccessor($this->db);
		foreach ($planets as $id => $name) {
			$pInsert->appendRow(array($msgID, $id, $name));
		}
		$pInsert->execute();
	}
}

?>

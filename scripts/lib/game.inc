<?php


class game {

	private $__status = null;
	private $__firstTick = null;
	private $__lastTick = null;

	function __construct($version, $name, $namespace, $public, $canJoin, $text) {
		$this->version		= $version;
		$this->name		= $name;
		$this->namespace	= $namespace;
		$this->public		= $public;
		$this->canJoin		= $canJoin;
		$this->text		= $text;
		$this->ticks		= array();
		$this->params		= array();
		$this->descriptions	= array();
		$this->initExternal();
	}


	private function initExternal() {
		$this->dir	= $this->version->getDirectory();
		$this->siteDir	= $this->version->getSiteDirectory();
	}


	function getDBAccess() {
		if ($this->db) {
			return $this->db;
		}

		if (!db::$database) {
			return null;
		}
		return ($this->db = db::$database->getGameAccess($this->namespace));
	}


	function addTick($tick) {
		$td = $tick->definition->script;
		if (is_null($this->ticks[$td])) {
			$this->ticks[$td] = $tick;
		}
	}


	private function __loadActionObject() {
		if ($this->actionObj) {
			return;
		}

		$acn = $this->version->loadActionsClass();
		$this->actionObj = new $acn($this);

		foreach (get_class_methods($acn) as $method) {
			$this->actions[strtolower($method)] = array(false, $method);
		}

		if (is_array($this->actionObj->index)) {
			foreach ($this->actionObj->index as $action) {
				if (strtolower($action) == strtolower($acn)) {
					continue;
				}
				$this->actions[strtolower($action)] = array(true, $action, null);
			}
		}
	}


	private function __doAction($action, $args, $log = false) {
		// Load actions object
		$this->__loadActionObject();

		// Check action
		if (!is_array($this->actions[$action])) {
			l::fatal(23, "Unknown action was '$action' on game '{$this->name}'");
		}

		if ($this->actions[$action][0]) {
			// Load separate action class
			if (!is_object($this->actions[$action][2])) {
				$acn = $this->version->loadAction($this->actions[$action][1]);
				$this->actions[$action][2] = new $acn($this);
			}
			$rv = call_user_func_array(array($this->actions[$action][2], 'run'), $args);
		} else {
			// Call the action instance's method
			$rv = call_user_func_array(array($this->actionObj, $this->actions[$action][1]), $args);
		}

		return $rv;
	}


	public function action() {
		$n = func_num_args();
		if ($n == 0) {
			l::fatal(22, "Empty action call on game '{$this->name}'");
		}

		// Get arguments
		$args = func_get_args();
		$action = strtolower(array_shift($args));

		return $this->__doAction($action, $args);
	}


	public function deprecatedAction() {
		$n = func_num_args();
		if ($n == 0) {
			l::fatal(22, "Empty action call on game '{$this->name}'");
		}

		// Get arguments
		$args = func_get_args();
		$action = strtolower(array_shift($args));

		return $this->__doAction($action, $args);
	}


	function getLib($lib = null) {
		if (is_null($lib)) {
			$lib = $this->version->id;
		}

		if (is_null($this->libraries[$lib])) {
			$this->libraries[$lib] = new library($lib, $this);
		}
		$this->getDBAccess();
		return $this->libraries[$lib];
	}


	function runTick($name, $okNotFound = false) {
		if (!is_null($this->ticks[$name])) {
			$this->ticks[$name]->run();
		} elseif ($okNotFound) {
			$tick = $this->getLib($this->version->id . "/ticks/$name");
			if (is_null($tick)) {
				l::error("TICK: tick library '$libName' not found");
			} else {
				$tick->call('runTick');
			}
		}
	}


	private function earliestTick() {
		$then = time() + 10 * 365 * 24 * 60 * 60;
		foreach ($this->ticks as $td => $tick) {
			if ($tick->first < $then && $tick->definition->public) {
				$then = $tick->first;
			}
		}
		return $then;
	}

	private function latestTick() {
		$then = 0;
		foreach ($this->ticks as $td => $tick) {
			if (is_null($tick->last)) {
				continue;
			}
			if ($tick->last > $then) {
				$then = $tick->last;
			}
		}
		return $then;
	}

	private function computeStatus() {
		$visible = $this->public && $this->canJoin;
		$this->__firstTick = $this->earliestTick($this);
		$this->__lastTick = $this->latestTick($this);
		$now = time();

		if ($this->__lastTick && $this->__lastTick < $now) {
			$this->__status = 'FINISHED';
		} elseif (!$visible) {
			$this->__status = 'PRE';
		} else {
			$running = ($this->__firstTick <= $now);
			if ($running) {
				if ($this->__lastTick) {
					$this->__status = "ENDING";
				} else {
					$gLib = $this->getLib();
					$this->__status = $gLib->call('isFinished') ? 'VICTORY' : 'RUNNING';
				}
			} else {
				$this->__status = 'READY';
			}
		}
	}


	function status() {
		if (is_null($this->__status)) {
			$this->computeStatus();
		}
		return $this->__status;
	}

	function firstTick() {
		if (is_null($this->__firstTick)) {
			$this->computeStatus();
		}
		return $this->__firstTick;
	}

	function lastTick() {
		if (is_null($this->__lastTick)) {
			$this->computeStatus();
		}
		return $this->__lastTick;
	}



	function __sleep() {
		return array('version', 'name', 'namespace', 'public', 'canJoin', 'text', 'descriptions', 'ticks', 'params');
	}

	function __wakeup() {
		$this->initExternal();
	}

	function sessName() {
		if (class_exists('input')) {
			return input::$game->name . "_data";
		}
		return "";
	}

}


?>

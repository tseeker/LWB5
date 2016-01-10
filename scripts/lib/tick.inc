<?php

/****************************************************************************
 * TICK MANAGEMENT CLASSES
 ****************************************************************************/

class tick_definition {

	function __construct($version, $script, $public) {
		$this->version	= $version;
		$this->script	= $script;
		$this->public	= $public;
		$this->text	= array();
		$this->version->addTickDefinition($this);
	}

	function addText($lang, $name, $description) {
		$this->text[$lang] = array($name, $description);
	}

	function getName($lang) {
		return ($this->text[$lang] ? $this->text[$lang][0] : "");
	}

	function getDescription($lang) {
		return ($this->text[$lang] ? $this->text[$lang][1] : "");
	}

	function getPath() {
		return "{$this->version->id}/ticks/{$this->script}";
	}
}


class tick_instance {

	function __construct($game, $script, $first, $interval, $last) {
		$this->game		= $game;
		$this->definition	= $game->version->getTickDefinition($script);
		$this->first		= $first;
		$this->interval		= $interval;
		$this->last		= $last;
		$this->game->addTick($this);
		$this->computeNext();
	}

	function computeNext() {
		$now = time();

		if (!is_null($this->last) && $now >= $this->last) {
			$this->next = null;
			return;
		}

		if ($now < $this->first) {
			$this->next = $this->first;
			return;
		}

		$diff = $now - $this->first;
		$mod = $diff % $this->interval;
		$mul = ($diff - $mod) / $this->interval;
		$this->next = $this->first + ($mul + 1) * $this->interval;
	}

	function run() {
		$libName = $this->definition->getPath();
		$tick = $this->game->getLib($libName);

		l::setPrefix("{$this->game->name}::{$this->definition->script}");
		if (is_null($tick)) {
			l::error("tick library '$libName' not found");
		} else {
			try {
				$tick->call('runTick');
			} catch (Exception $e) {
				l::error("tick failed with exception " . get_class($e));
				l::info("exception message: " . $e->getMessage());
			}
		}
		l::setPrefix("");
	}

	function __sleep() {
		return array('game', 'definition', 'first', 'interval', 'last');
	}

	function __wakeup() {
		$this->computeNext();
	}
}


?>

<?php


class lw_mod extends module {

	public function init() {
	}

	public function destroy() {
	}

	public function nextTick($line, $args) {
		$gameID = $args['nargs'] == 1 ? $args['query'] : null;

		// Get the game
		$game = is_null($gameID) ? config::getDefaultGame() : config::getGame($gameID);
		if (is_null($game)) {
			$msg = "Game ID '" . BOLD . $gameID . BOLD . "' not found";
		} else {
			// Try finding public ticks
			$minTick = null;
			foreach ($game->ticks as $tick) {
				if (! $tick->definition->public) {
					continue;
				}
				$tick->computeNext();
				if (is_null($tick->next)) {
					continue;
				}
				if (is_null($minTick) || $minTick->next > $tick->next) {
					$minTick = $tick;
				}
			}
			
			if (is_null($minTick)) {
				$msg = "[" . BOLD . $game->text . BOLD . "] No more ticks on this game";
			} else {
				$msg = "[" . BOLD . $game->text . BOLD . "] Next tick: " . BOLD
					. $minTick->definition->getName('en') . BOLD . " at " . BOLD
					. gmstrftime("%H:%M:%S", $minTick->next) . BOLD;
				if (gmstrftime("%Y-%m-%d", $minTick->next) != gmstrftime("%Y-%m-%d", time())) {
					$msg .= " on " . BOLD . gmstrftime("%d/%m/%Y", $minTick->next) . BOLD;
				}
			}
		}

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}

	public function listGames($line, $args) {
		static $statusText = array(
			"READY"		=> "open for registration",
			"RUNNING"	=> "running",
			"VICTORY"	=> "victory conditions reached",
			"ENDING"	=> "being terminated",
			"FINISHED"	=> "terminated"
		);

		$to = $line['fromNick'];

		dbConnect();
		foreach (config::getGames() as $game) {
			if ($game->name == 'main' || $game->status() == 'PRE') {
				continue;
			}

			$msg = "(" . BOLD . $game->name . BOLD . ") " . BOLD . $game->text . BOLD . " - Status: "
				. BOLD . $statusText[$game->status()] . BOLD;
			if ($game->status() == "READY") {
				$msg .= " - Starting at " . BOLD . gmstrftime("%H:%M:%S", $game->firstTick())
					. BOLD . " on " . BOLD . gmstrftime("%d/%m/%Y", $game->firstTick()) . BOLD;
			} elseif ($game->status() == "ENDING") {
				$msg .= " - Ending at " . BOLD . gmstrftime("%H:%M:%S", $game->lastTick())
					. BOLD . " on " . BOLD . gmstrftime("%d/%m/%Y", $game->lastTick()) . BOLD;
			}
			$this->ircClass->sendRaw("PRIVMSG $to :$msg");
		}
	}

	public function getRank($line, $args) {
		list($player, $game) = $this->getParams($line, $args);

		dbConnect();
		$rv = $this->fetchGenRank($player, $game);
		dbClose();

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		if (is_array($rv)) {
			$msg = "[" . BOLD . $rv[0] . BOLD . "] Player " . BOLD . $player . BOLD
				. ": " . BOLD . "#{$rv[1]['ranking']}" . BOLD . " (" . BOLD
				. number_format($rv[1]['points']) . BOLD . " points)";
			if (! is_null($rv[2])) {
				$msg .= " - Overall round ranking: " . BOLD . "#{$rv[2]['ranking']}"
					. BOLD . " (" . BOLD . number_format($rv[2]['points'])
					. BOLD . " points)";
			}
		} elseif ($rv == 1) {
			$msg = "Game ID '" . BOLD . $game . BOLD . "' not found";
		} elseif ($rv == 2) {
			$msg = "Player " . BOLD . $player . BOLD . " not found";
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}

	public function getCivRank($line, $args) {
		list($player, $game) = $this->getParams($line, $args);

		dbConnect();
		$rv = $this->fetchDetRank($player, $game, 'p_civ');
		dbClose();

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		if (is_array($rv)) {
			$msg = "[" . BOLD . $rv[0] . BOLD . "] Player " . BOLD . $player . BOLD
				. " (civilisation): " . BOLD . "#{$rv[1]['ranking']}" . BOLD . " (" . BOLD
				. number_format($rv[1]['points']) . BOLD . " points)";
		} elseif ($rv == 1) {
			$msg = "Game ID '" . BOLD . $game . BOLD . "' not found";
		} elseif ($rv == 2) {
			$msg = "Player " . BOLD . $player . BOLD . " not found";
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}

	public function getFinRank($line, $args) {
		list($player, $game) = $this->getParams($line, $args);

		dbConnect();
		$rv = $this->fetchDetRank($player, $game, 'p_financial');
		dbClose();

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		if (is_array($rv)) {
			$msg = "[" . BOLD . $rv[0] . BOLD . "] Player " . BOLD . $player . BOLD
				. " (financial): " . BOLD . "#{$rv[1]['ranking']}" . BOLD . " (" . BOLD
				. number_format($rv[1]['points']) . BOLD . " points)";
		} elseif ($rv == 1) {
			$msg = "Game ID '" . BOLD . $game . BOLD . "' not found";
		} elseif ($rv == 2) {
			$msg = "Player " . BOLD . $player . BOLD . " not found";
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}

	public function getMilRank($line, $args) {
		list($player, $game) = $this->getParams($line, $args);

		dbConnect();
		$rv = $this->fetchDetRank($player, $game, 'p_military');
		dbClose();

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		if (is_array($rv)) {
			$msg = "[" . BOLD . $rv[0] . BOLD . "] Player " . BOLD . $player . BOLD
				. " (military): " . BOLD . "#{$rv[1]['ranking']}" . BOLD . " (" . BOLD
				. number_format($rv[1]['points']) . BOLD . " points)";
		} elseif ($rv == 1) {
			$msg = "Game ID '" . BOLD . $game . BOLD . "' not found";
		} elseif ($rv == 2) {
			$msg = "Player " . BOLD . $player . BOLD . " not found";
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}

	public function getIDR($line, $args) {
		list($player, $game) = $this->getParams($line, $args);

		dbConnect();
		$rv = $this->fetchDetRank($player, $game, 'p_idr');
		dbClose();

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		if (is_array($rv)) {
			$msg = "[" . BOLD . $rv[0] . BOLD . "] Player " . BOLD . $player . BOLD
				. " (inflicted damage): " . BOLD . "#{$rv[1]['ranking']}" . BOLD . " (" . BOLD
				. number_format($rv[1]['points']) . BOLD . " points)";
		} elseif ($rv == 1) {
			$msg = "Game ID '" . BOLD . $game . BOLD . "' not found";
		} elseif ($rv == 2) {
			$msg = "Player " . BOLD . $player . BOLD . " not found";
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}


	private function getParams($line, $args) {
		if ($args['nargs'] == 0) {
			$player = $line['fromNick'];
			$game = null;
		} else {
			$query = explode(' ', trim(preg_replace('/\s+/', ' ', $args['query'])));
			if (preg_match('/^{[a-z0-9]+}$/', $query[0], $matches)) {
				$game = preg_replace('/[{}]/', '', array_shift($query));
			} else {
				$game = null;
			}
			$player = join(' ', $query);
			if ($player == '') {
				$player = $line['fromNick'];
			}
		}
		return array($player, $game);
	}


	private function fetchGenRank($player, $gameID) {
		// Get the game
		$game = is_null($gameID) ? config::getDefaultGame() : config::getGame($gameID);
		if (is_null($game)) {
			return 1;
		}

		// Access the rankings library
		$rLib = $game->getLib('main/rankings');

		// Get player ranking
		$rType = $rLib->call('getType', 'p_general');
		$genRank = $rLib->call('get', $rType, $player);
		if (is_null($genRank['points'])) {
			return 2;
		}

		// Try getting the overall round rankings
		$rType = $rLib->call('getType', 'p_round');
		$orRank = $rLib->call('get', $rType, $player);
		if (is_null($orRank['points'])) {
			$orRank = null;
		}
		return array($game->text, $genRank, $orRank);
	}


	private function fetchDetRank($player, $gameID, $type) {
		// Get the game
		$game = is_null($gameID) ? config::getDefaultGame() : config::getGame($gameID);
		if (is_null($game)) {
			return 1;
		}

		// Access the rankings library
		$rLib = $game->getLib('main/rankings');

		// Get player ranking
		$rType = $rLib->call('getType', $type);
		$rank = $rLib->call('get', $rType, $player);
		if (is_null($rank['points'])) {
			return 2;
		}
		return array($game->text, $rank);
	}


	public function getAllianceRank($line, $args) {
		list($alliance, $game) = $this->getParams($line, $args);

		dbConnect();
		$rv = $this->fetchDetRank($alliance, $game, 'a_general');
		dbClose();

		if ($line['to'] == $this->ircClass->getClientConf('nick')) {
			$to = $line['fromNick'];
		} else {
			$to = $line['to'];
		}

		if (is_array($rv)) {
			$msg = "[" . BOLD . $rv[0] . BOLD . "] Alliance " . BOLD . $alliance . BOLD
				. ": " . BOLD . "#{$rv[1]['ranking']}" . BOLD . " (" . BOLD
				. number_format($rv[1]['points']) . BOLD . " points)";
		} elseif ($rv == 1) {
			$msg = "Game ID '" . BOLD . $game . BOLD . "' not found";
		} elseif ($rv == 2) {
			$msg = "Alliance " . BOLD . $alliance . BOLD . " not found";
		}

		$this->ircClass->sendRaw("PRIVMSG $to :$msg");
	}

	public function help($line, $args) {
		$help = array(
			"" => array(
				"This bot allows you to get some information from the",
				"Legacy Worlds game directly here, on IRC!",
				" ",
				"You can use any of the following commands:",
				" ",
				"  " . BOLD . "Displaying rankings" . BOLD,
				"    " . BOLD . "rank" . BOLD . " - displays players' general and round rankings",
				"     " . BOLD . "civ" . BOLD . " - displays players' civilian rankings",
				"     " . BOLD . "mil" . BOLD . " - displays players' military rankings",
				"     " . BOLD . "fin" . BOLD . " - displays players' financial rankings",
				"     " . BOLD . "idr" . BOLD . " - displays players' inflicted damage rankings",
				"   " . BOLD . "arank" . BOLD . " - displays alliances' rankings",
				" ",
				"  " . BOLD . "General information" . BOLD,
				"  " . BOLD . "games" . BOLD . " - lists available games",
				"   " . BOLD . "tick" . BOLD . " - displays the time and date of the next tick",
				"   " . BOLD . "help" . BOLD . " - help access",
				" ",
				"All commands must start with the '!' character. To get more",
				"information on a specific command, type '!help <command>'"
			),
			"arank" => array(
				"Syntax: " . BOLD . "!arank [{game}] tag",
				"        " . BOLD . "!a [{game}] tag",
				" ",
				"This command gives information about an alliance's ranking.",
				" ",
				"It is possible to select the game by adding the game's ID between",
				"brackets just before the player's name.",
			),
			"rank" => array(
				"Syntax: " . BOLD . "!rank [{game}] [player]",
				"        " . BOLD . "!r [{game}] [player]",
				" ",
				"This command gives information about a player's general ranking",
				"as well as his round ranking if he has one.",
				" ",
				"Using the command without parameters will cause the bot to look",
				"for your current nick, if it's the same as your in-game name.",
				" ",
				"It is possible to select the game by adding the game's ID between",
				"brackets just before the player's name.",
				" ",
				"Examples: !r TSeeker",
				"           -> Displays TSeeker's rankings in the default game",
				"          !r {b5m2}",
				"           -> Displays your rankings in Match 2",
			),
			"civ" => array(
				"Syntax: " . BOLD . "!civ [{game}] [player]",
				" ",
				"This command gives information about a player's civilisation",
				"ranking.",
				" ",
				"Using the command without parameters will cause the bot to look",
				"for your current nick, if it's the same as your in-game name.",
				" ",
				"It is possible to select the game by adding the game's ID between",
				"brackets just before the player's name.",
				" ",
				"See also: " . BOLD . "!help rank" . BOLD
			),
			"mil" => array(
				"Syntax: " . BOLD . "!mil [{game}] [player]",
				" ",
				"This command gives information about a player's military",
				"ranking.",
				" ",
				"Using the command without parameters will cause the bot to look",
				"for your current nick, if it's the same as your in-game name.",
				" ",
				"It is possible to select the game by adding the game's ID between",
				"brackets just before the player's name.",
				" ",
				"See also: " . BOLD . "!help rank" . BOLD
			),
			"fin" => array(
				"Syntax: " . BOLD . "!fin [{game}] [player]",
				" ",
				"This command gives information about a player's financial",
				"ranking.",
				" ",
				"Using the command without parameters will cause the bot to look",
				"for your current nick, if it's the same as your in-game name.",
				" ",
				"It is possible to select the game by adding the game's ID between",
				"brackets just before the player's name.",
				" ",
				"See also: " . BOLD . "!help rank" . BOLD
			),
			"tick" => array(
				"Syntax: " . BOLD . "!tick [game]",
				"        " . BOLD . "!t [game]",
				" ",
				"This command displays the next tick.",
				" ",
				"It is possible to select the game by adding the game's ID",
				"after the command.",
			),
			"games" => array(
				"Syntax: " . BOLD . "!games",
				"        " . BOLD . "!g",
				" ",
				"This command displays the list of available games.",
			),
		);

		$topic = $args['query'];
		if (! array_key_exists($topic, $help)) {
			$topic = "";
		}

		$to = $line['fromNick'];
		for ($i = 0; $i < count($help[$topic]); $i ++) {
			$this->ircClass->privMsg($to, $help[$topic][$i]);
		}
	}
}

?>


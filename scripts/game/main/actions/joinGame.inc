<?


/** This action allows an user to join a game.
 *
 * \parameter $user	User ID of the user who wants to register
 * \parameter $gId	Game ID of the game to join
 * \parameter $dryRun	Indicates whether to register or to check whether the
 *			game can be joined by the user
 * \parameter $planet	Name of the player's first planet
 * \parameter $player	Name of the player if he had played the game before
 *
 * \returns either an integer indicating an internal error or an array
 *	containing the data related to the request.
 *
 *	The following integer values indicate errors:
 *		0	Game not found
 *		1	User already registered
 *		2	Internal error while registering
 *
 *	Two forms of arrays can be returned. If the call was a dry run or if
 *	an error happened, it will contain the following data:
 *
 *		game		Game ID of the game to register to
 *		gName		Title of the game to register to
 *		planet		Selected planet name
 *		planetError	An integer indicating an eventual error (see
 *					below)
 *		player		Selected player name
 *		playerError	An integer indicating an eventual error (see
 *					below)
 *		returning	A boolean indicating whether the user has
 *					played the selected game before
 *
 *	The error codes for the names are:
 *
 *		0	Game not found
 *		1	Name too long
 *		2	Invalid characters
 *		3	Heading/trailing spaces
 *		4	Multiple spaces
 *		5	Name too short
 *		6	Name in use
 *
 *	If the registration was successful, the following array is returned:
 *
 *		registered	ID of the game the user registered to
 *
 */
class main_joinGame {

	function main_joinGame($main) {
		$this->main	= $main;
		$this->lib	= $this->main->getLib();
	}

	function run($user, $gId, $dryRun = true, $planet = null, $player = null) {
		// Get the game and its main library
		$game = config::getGame($gId);
		if (is_null($game)) {
			return array('error' => 0);
		}
		$lib = $game->getLib();

		// Check the game's status
		$status = $game->status();
		if ($status != 'RUNNING' && $status != 'READY' && $status != 'ENDING') {
			return array('error' => 0);
		}

		// Check if the game is available and can be joined
		if (! $lib->call('canJoin')) {
			return array('error' => 0);
		}

		// Does the user play the game already?
		if ((int)$user < 1 || $lib->call('doesUserPlay', $user)) {
			return array('error' => 1);
		}

		// Has the current user played that game in the past?
		$returning = $lib->call("hasPlayed", $user);

		// If the user submitted the form, handle it
		if ($dryRun) {
			// Display a blank form
			return array(
				"game"		=> $gId,
				"gName"		=> $game->text,
				"desc"		=> $game->descriptions[getLanguage()],
				"planet"	=> "",
				"planetError"	=> 0,
				"player"	=> "",
				"playerError"	=> 0,
				"returning"	=> $returning
			);
		}

		// Check the specified planet and player names
		$pErr = $lib->call('checkPlanetName', $planet);
		if ($returning) {
			$pnErr = $this->checkName($player);
		} else {
			$pnErr = 0;
		}

		if (!($pErr || $pnErr)) {
			// Try to register to this game
			$res = $lib->call('register', $user, $planet, $returning ? $player : null);
			switch ($res) :
				case 1: return array('error' => 2);
				case 2: $pnErr = 6; break;
				case 3: $pErr = 6; break;
			endswitch;
		}

		if ($pErr || $pnErr) {
			return array(
				"game"		=> $gId,
				"gName"		=> $game->text,
				"planet"	=> $planet,
				"planetError"	=> $pErr,
				"player"	=> $player,
				"playerError"	=> $pnErr,
				"returning"	=> $returning
			);
		}

		return array("registered" => $gId);
	}


	function checkName($n) {
		if (strlen($n) > 15) {
			return array('error' => 1);
		} elseif (preg_match('/[^A-Za-z0-9_\.\-\+@\/'."'".' ]/', $n)) {
			return array('error' => 2);
		} elseif (preg_match('/^\s/', $n) || preg_match('/\s$/', $n)) {
			return array('error' => 3);
		} elseif (preg_match('/\s\s+/', $n)) {
			return array('error' => 4);
		} elseif (strlen($n) < 2) {
			return array('error' => 5);
		}
		return 0;
	}
}


?>

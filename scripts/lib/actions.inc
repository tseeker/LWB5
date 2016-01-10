<?php


/****************************************************************************
 * GAME ACTIONS
 ****************************************************************************/


/** The game_action class should be used as a base class for all classes
 * implementing specific game actions.
 *
 * It initializes the object by storing the game instance and loading the
 * required libraries.
 */
abstract class game_action {

	/** A reference to the game instance the action is called on.
	 */
	protected $game;

	/** Libraries loaded and instanciated. These libraries will
	 * be accessed to the __get() method.
	 */
	private $__libraries;

	/** The constructor initialises the instance by storing the
	 * associated game instance and loading libraries.
	 */
	protected function __construct($game, $libraries = null) {
		$this->game = $game;

		if (is_null($libraries)) {
			$libraries = array();
		}
		$this->__initLibraries($libraries);
	}

	/** This method loads libraries from the game instance and
	 * stores them inside the $__libraries array.
	 */
	private function __initLibraries($libraries) {
		$libs = array();
		foreach ($libraries as $key => $name) {
			$libs[$key] = $this->game->getLib($name);
		}
		$this->__libraries = $libs;
	}

	/** The __get() overload method is useful to access the
	 * libraries associated with this game action.
	 */
	protected function __get($var) {
		if (array_key_exists($var, $this->__libraries)) {
			return $this->__libraries[$var];
		}
		return null;
	}
}


/** This function allows the user to call a game action function; it must
 * have at least one argument, which may be either a string (the name of the
 * action function to call for the current game version) or an array
 * containing two strings (the version identifier and the name of the action).
 * Other arguments may follow; they will be passed to the action function.
 */
function gameAction() {
	$sitePath = input::$path;

	$n = func_num_args();
	if ($n == 0) {
		l::fatal(22, "Requested game was '$sitePath'");
	}
	$args = func_get_args();
	$gas = array_shift($args);

	if (is_array($gas)) {
		list($v,$a) = $gas;
	} elseif (is_string($gas)) {
		$v = $sitePath;
		$a = $gas;
	} else {
		l::fatal(23, "Requested game was '$sitePath'");
	}
	array_unshift($args, $a);

	$g = config::getGame($v);
	l::deprecated("gameAction($v, $a, ...)");
	return call_user_func_array(array($g, 'deprecatedAction'), $args);
}


?>

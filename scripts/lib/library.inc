<?php

class library {
	var $name;
	var $game;
	var $mainClass	= null;
	var $functions	= array();

	function library($name, $game) {
		$this->name	= $name;
		$this->game	= $game;
	}

	function loadClass($name = null) {
		// Get the path to the class to be loaded
		$path = config::$main['scriptdir'] . "/game/{$this->name}/library";
		if (!is_null($name)) {
			$path .= "/$name";
		}
		$path .= ".inc";

		// Get the class' name
		$cn = preg_replace('#/#', '_', strtolower($this->name));
		$cn .= is_null($name) ? "_library" : "_$name";

		// Load it
		loader::load($path, $cn);
		return $cn;
	}

	function call() {
		$n = func_num_args();
		if ($n == 0) {
			l::fatal(22, "Empty library call for library '{$this->name}' on game '{$this->game->game['site_path']}'");
		}

		// Load the main class if that is needed
		if (!$this->mainClass) {
			$lcn = $this->loadClass();
			$this->mainClass = new $lcn($this);

			foreach (get_class_methods($lcn) as $method) {
				$this->functions[strtolower($method)] = array(false, $method);
			}

			if (is_array($this->mainClass->index)) {
				foreach ($this->mainClass->index as $function) {
					if (strtolower($function) == strtolower($lcn)) {
						continue;
					}
					$this->functions[strtolower($function)] = array(true, $function, null);
				}
			}
		}

		// Check function
		$args = func_get_args();
		$function = strtolower(array_shift($args));
		if (!is_array($this->functions[$function])) {
			l::fatal(23, "Unknown function call '$function' in library '{$this->name}' on game '{$this->game->game['site_path']}'");
		}

		if ($this->functions[$function][0]) {
			// Load separate class
			if (!is_object($this->functions[$function][2])) {
				$fcn = $this->loadClass($this->functions[$function][1]);
				$this->functions[$function][2] = new $fcn($this);
			}
			$rv = call_user_func_array(array($this->functions[$function][2], 'run'), $args);
		} else {
			// Call the function instance's method
			$rv = call_user_func_array(array($this->mainClass, $this->functions[$function][1]), $args);
		}

		return $rv;
	}
}

?>

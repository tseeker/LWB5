<?php

class loader {
	private static $loadedClasses = array();

	static function load($file, $className) {
		if (in_array($className, loader::$loadedClasses)) {
			return;
		}

		if (!(file_exists($file) && is_readable($file) && is_file($file))) {
			l::fatal(19, "File '$file' (for class $className) not found");
		}
		if (!include_once($file)) {
			l::fatal(20, "File '$file' (for class $className) returned false");
		}
		if (!class_exists($className)) {
			l::fatal(21, "Class '$className' not found in file '$file'");
		}

		array_push(loader::$loadedClasses, $className);
	}
}

?>

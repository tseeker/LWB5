<?php

/****************************************************************************
 * MAIN CONFIGURATION MANAGEMENT
 ****************************************************************************/


class config {

	static $config = null;
	static $main = null;

	function __construct($versions, $games, $defGame) {
		$this->versions	= $versions;
		$this->games	= $games;
		$this->defGame	= $defGame;
	}

	private static function parseXML($xmlData) {
		require_once('xml_config.inc');
		return xml_config::parse($xmlData);
	}

	private static function tryLoadSerialized() {
		$config = &config::$main;

		if (!file_exists("{$config['cachedir']}/config.ser")) {
			return false;
		}

		$f = @file_get_contents("{$config['cachedir']}/config.ser");
		if ($f === false) {
			return false;
		}
		return @unserialize($f);
	}

	private static function writeCache($cObj) {
		$config = &config::$main;

		$text = serialize($cObj);
		$mask = umask(0002);
		$w = @file_put_contents("{$config['cachedir']}/config.ser", $text, LOCK_EX);
		umask($mask);
		if ($w === false) {
			l::warn("CONFIG: failed to cache configuration");
		}
	}

	private static function checkUpdate($checksum = null) {
		$config = &config::$main;

		$f = @file_get_contents("{$config['scriptdir']}/legacyworlds.xml");
		if ($f === false) {
			l::error("CONFIG: could not open file '{$config['scriptdir']}/legacyworlds.xml'");
			return false;
		}

		$md5 = md5($f);
		if (!is_null($checksum) && $checksum == $md5) {
			return false;
		}
		l::notice("CONFIG: XML configuration file modified, updating");

		$data = config::parseXML($f);
		if (is_object($data)) {
			$data->checksum = $md5;
			config::writeCache($data);
		}
		return $data;
	}


	static function load() {
		global $config;
		if (!is_null($config) && is_null(config::$main)) {
			config::$main = $config;
		}

		$sData = config::tryLoadSerialized();
		if (!is_object($sData)) {
			$sData = config::checkUpdate();
		} else {
			$nData = config::checkUpdate($sData->checksum);
			if (is_object($nData)) {
				$sData = $nData;
			}
		}

		if (!is_object($sData)) {
			l::fatal(0, "The XML configuration could not be parsed and serialized data could not be read");
		}
		config::$config = $sData;
	}


	static function reload() {
		$nData = config::checkUpdate(config::$config->checksum);
		if (is_object($nData)) {
			config::$config = $nData;
			l::info("CONFIG: configuration updated");
		}
		return is_object($nData);
	}


	static function getGames() {
		return config::$config->games;
	}

	static function getGame($name) {
		return config::$config->games[$name];
	}

	static function getDefaultGame() {
		return config::$config->games[config::$config->defGame];
	}

	static function hasGame($name) {
		return is_object(config::$config->games[$name]);
	}

	static function getParam($name) {
		return config::$config->games['main']->params[$name];
	}
}

config::load();

?>

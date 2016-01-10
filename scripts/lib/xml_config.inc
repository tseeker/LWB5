<?php

/****************************************************************************
 * XML CONFIGURATION PARSER
 ****************************************************************************/

class xml_config {

	private static $mVersion	= null;
	private static $mGame		= null;
	private static $versions	= null;
	private static $games		= null;
	private static $defGame		= null;

	private function parseMainParams($root) {
		$node = $root->firstChild;

		while ($node) {
			if ($node->nodeType == XML_ELEMENT_NODE && $node->nodeName == 'Param') {
				$aName = $node->getAttribute('name');
				$aValue = $node->getAttribute('value');
				if ($aName == "") {
					l::warn("CONFIG: a main parameter is missing a 'name' attribute");
				} elseif (is_null(xml_config::$mGame->params[$aName])) {
					xml_config::$mGame->params[$aName] = $aValue;
				} else {
					l::notice("CONFIG: duplicate main parameter '$aName'");
				}
			} elseif ($node->nodeType == XML_ELEMENT_NODE) {
				l::notice("CONFIG: found unexpected tag '{$node->nodeName}' in MainParams section");
			}
			$node = $node->nextSibling;
		}
	}


	private function parseMainTicks($root) {
		$node = $root->firstChild;

		while ($node) {
			if ($node->nodeType == XML_ELEMENT_NODE && $node->nodeName == 'Tick') {
				$tScript	= $node->getAttribute('script');
				$tFirst		= $node->getAttribute('first');
				$tInterval	= $node->getAttribute('interval');
				$tLast		= $node->getAttribute('last');
				if ($tScript == "" || $tFirst == "" || $tInterval == "") {
					l::warn("CONFIG: a main tick declaration is missing a mandatory attribute");
				} elseif ((int)$tInterval == 0) {
					l::warn("CONFIG: invalid interval for main tick '$tScript'");
				} elseif (is_null(xml_config::$mVersion->getTickDefinition($tScript))) {
					new tick_definition(xml_config::$mVersion, $tScript, false);
					new tick_instance(xml_config::$mGame, $tScript, (int)$tFirst,
						(int)$tInterval, $tLast ? (int)$tLast : null);
				} else {
					l::notice("CONFIG: duplicate main tick '$tScript'");
				}
			} elseif ($node->nodeType == XML_ELEMENT_NODE) {
				l::notice("CONFIG: found unexpected tag '{$node->nodeName}' in MainTicks section");
			}
			$node = $node->nextSibling;
		}
	}


	private function parseTickDefinition($version, $root) {
		$script = $root->getAttribute('script');
		$public = ($root->getAttribute('public') == 1);

		if ($script == "") {
			l::warn("CONFIG: invalid tick definition for version {$version->id}");
			return;
		}
		if (!is_null($version->getTickDefinition($script))) {
			l::notice("CONFIG: duplicate tick definition '$script' for version {$version->id}");
			return;
		}

		$def = new tick_definition($version, $script, $public);
		if (!$public) {
			return;
		}

		$names = array();
		$descs = array();
		$node = $root->firstChild;
		while ($node) {
			if ($node->nodeType != XML_ELEMENT_NODE) {
				$node = $node->nextSibling;
				continue;
			}

			if ($node->nodeName != "Name" && $node->nodeName != 'Description') {
				l::warn("CONFIG: unexpected tag '{$node->nodeName}' found in tick definition "
					. "'{$script}' for version '{$version->id}'");
				$node = $node->nextSibling;
				continue;
			}

			$lang = $node->getAttribute('lang');
			if ($lang == '') {
				l::warn("CONFIG: missing language for {$node->nodeName} in tick definition "
					. "'{$script}' for version '{$version->id}'");
				$node = $node->nextSibling;
				continue;
			}

			$contents = trim($node->textContent);
			if ($contents == '') {
				l::warn("CONFIG: missing contents for {$node->nodeName} in tick definition "
					. "'{$script}' for version '{$version->id}'");
				$node = $node->nextSibling;
				continue;
			}

			if ($node->nodeName == 'Name') {
				$names[$lang] = $contents;
			} else {
				$descs[$lang] = $contents;
			}

			$node = $node->nextSibling;
		}

		foreach ($names as $lang => $name) {
			$def->addText($lang, $name, $descs[$lang]);
		}
	}


	private function parseVersion($root) {
		$id = $root->getAttribute("id");
		$cp = ($root->getAttribute("playable") == 1);
		$tx = $root->getAttribute("text");

		if ($id == "" || $tx == "") {
			l::warn("CONFIG: invalid version definition (missing identifier or text)");
			return null;
		} elseif ($id == 'main') {
			l::warn("CONFIG: invalid version definition (using 'main' identifier)");
			return null;
		}

		$version = new version($id, $cp, $tx);

		$node = $root->firstChild;
		while ($node) {
			if ($node->nodeType == XML_ELEMENT_NODE && $node->nodeName == 'Tick') {
				xml_config::parseTickDefinition($version, $node);
			} elseif ($node->nodeType == XML_ELEMENT_NODE) {
				l::warn("CONFIG: found unexpected tag '{$node->nodeName}' in MainTicks section");
			}
			$node = $node->nextSibling;
		}

		return $version;
	}


	private function parseVersions($root) {
		$node = $root->firstChild;

		$versions = array();
		while ($node) {
			if ($node->nodeType == XML_ELEMENT_NODE && $node->nodeName == 'Version') {
				$v = xml_config::parseVersion($node);
				if (!is_null($v)) {
					if (is_null($versions[$v->id])) {
						$versions[$v->id] = $v;
					} else {
						l::notice("CONFIG: found duplicate definition for version '{$v->id}'");
					}
				}
			} elseif ($node->nodeType == XML_ELEMENT_NODE) {
				l::notice("CONFIG: found unexpected tag '{$node->nodeName}' in Versions section");
			}
			$node = $node->nextSibling;
		}

		return $versions;
	}


	private function parseGame($root) {
		$id = $root->getAttribute('id');
		if ($id == '') {
			l::warn("CONFIG: game definition is missing an identifier");
			return null;
		}
		if ($id == 'main') {
			l::warn("CONFIG: game definition with 'main' identifier ignored");
			return null;
		}

		$version = $root->getAttribute('version');
		if (is_null(xml_config::$versions[$version])) {
			l::warn("CONFIG: game '$id' has unknown version '$version'");
			return null;
		}

		$namespace = $root->getAttribute('namespace');
		if ($namespace == '') {
			l::warn("CONFIG: no namespace defined for game '$id'");
			return null;
		}
		$text = $root->getAttribute('text');
		if ($namespace == '') {
			l::warn("CONFIG: no text for game '$id'");
			return null;
		}

		$public = ($root->getAttribute('public') == 1);
		$canJoin = ($root->getAttribute('canjoin') == 1);

		$game = new game(xml_config::$versions[$version], $id, $namespace, $public, $canJoin, $text);

		$node = $root->firstChild;
		while ($node) {
			if ($node->nodeType != XML_ELEMENT_NODE) {
				$node = $node->nextSibling;
				continue;
			}

			if ($node->nodeName == 'Param') {
				$aName = $node->getAttribute('name');
				$aValue = $node->getAttribute('value');
				if ($aName == "") {
					l::warn("CONFIG: a parameter is missing a 'name' attribute for game '$id'");
				} elseif (is_null($game->params[$aName])) {
					$game->params[$aName] = $aValue;
				} else {
					l::notice("CONFIG: duplicate parameter '$aName'");
				}
			} elseif ($node->nodeName == 'Description') {
				$lang = $node->getAttribute('lang');
				$contents = trim($node->textContent);
				if ($lang == "") {
					l::warn("CONFIG: a description is missing the 'lang' attribute for game '$id'");
				} elseif ($contents == "") {
					l::warn("CONFIG: description in language '$lang' has no contents for game '$id'");
				} elseif (!is_null($game->descriptions[$lang])) {
					l::notice("CONFIG: description in language '$lang' appears twice for game '$id'");
				} else {
					$game->descriptions[$lang] = $contents;
				}
			} elseif ($node->nodeName == 'Tick') {
				$tScript	= $node->getAttribute('script');
				$tFirst		= $node->getAttribute('first');
				$tInterval	= $node->getAttribute('interval');
				$tLast		= $node->getAttribute('last');
				if ($tScript == "" || $tFirst == "" || $tInterval == "") {
					l::warn("CONFIG: a tick declaration is missing a mandatory attribute "
						. "for game '$id'");
				} elseif ((int)$tInterval == 0) {
					l::warn("CONFIG: invalid interval for tick '$tScript' in game '$id'");
				} elseif (is_null($game->version->getTickDefinition($tScript))) {
					l::warn("CONFIG: no definition for tick '$tScript' in game '$id' "
						. "(version '$version')");
				} elseif (is_null($game->ticks[$tScript])) {
					new tick_instance($game, $tScript, (int)$tFirst, (int)$tInterval,
						$tLast ? (int)$tLast : null);
				} else {
					l::notice("CONFIG: duplicate tick initialiser for tick '$tScript' in game '$id'");
				}
			} else {
				l::notice("CONFIG: found unexpected tag '{$node->nodeName}' in game '$id' definition");
			}

			$node = $node->nextSibling;
		}

		return $game;
	}


	private function parseGames($root) {
		$defaultId = $root->getAttribute('default');
		$games = array();

		$node = $root->firstChild;
		while ($node) {
			if ($node->nodeType == XML_ELEMENT_NODE && $node->nodeName == 'Game') {
				$g = xml_config::parseGame($node);
				if (!is_null($g)) {
					if (is_null($games[$g->name])) {
						$games[$g->name] = $g;
					} else {
						l::notice("CONFIG: found duplicate definition for game '{$v->name}'");
					}
				}
			} elseif ($node->nodeType == XML_ELEMENT_NODE) {
				l::notice("CONFIG: found unexpected tag '{$node->nodeName}' in Games section");
			}
			$node = $node->nextSibling;
		}

		if (count($games) && ($defaultId == '' || is_null($games[$defaultId]))) {
			$defaultId = $games[0]->name;
			l::notice("CONFIG: no default game, using '$defaultId'");
		}
		xml_config::$defGame = $defaultId;

		return $games;
	}


	function parse($xmlData) {
		$doc = new DOMDocument();
		if (!$doc->loadXML($xmlData)) {
			l::error("CONFIG: error while parsing XML configuration");
			return false;
		}

		xml_config::$mVersion = new version('main', false, 'Legacy Worlds');
		xml_config::$mGame = new game(xml_config::$mVersion, 'main', '', false, false, 'Legacy Worlds');
		xml_config::$versions = null;
		xml_config::$games = null;
		xml_config::$defGame = null;

		$root = $doc->documentElement;
		$node = $root->firstChild;
		while ($node) {
			if ($node->nodeType == XML_ELEMENT_NODE) {
				switch ($node->nodeName) :
				case 'MainParams':
					xml_config::parseMainParams($node);
					break;

				case 'MainTicks':
					xml_config::parseMainTicks($node);
					break;

				case 'Versions':
					if (is_array(xml_config::$versions)) {
						l::notice("CONFIG: found duplicate set of version definitions");
					} else {
						xml_config::$versions = xml_config::parseVersions($node);
					}
					break;

				case 'Games':
					if (is_array(xml_config::$games)) {
						l::notice("CONFIG: found duplicate set of game definitions");
					} elseif (is_array(xml_config::$versions)) {
						xml_config::$games = xml_config::parseGames($node);
					} else {
						l::notice("CONFIG: game definitions found before version "
							. "definitions, ignored");
					}
					break;

				default:
					l::notice("CONFIG: found unknown tag '{$node->nodeName}' at toplevel");
					break;
				endswitch;
			}
			$node = $node->nextSibling;
		}

		if (!is_array(xml_config::$versions) || count(xml_config::$versions) == 0) {
			l::error("CONFIG: no versions have been defined!");
			return false;
		}

		if (!is_array(xml_config::$games) || count(xml_config::$games) == 0) {
			l::error("CONFIG: no games have been defined!");
			return false;
		}

		xml_config::$versions['main'] = xml_config::$mVersion;
		xml_config::$games['main'] = xml_config::$mGame;
		return new config(xml_config::$versions, xml_config::$games, xml_config::$defGame);
	}

}

?>

<?php

class main_manual_readXMLFile {

	function main_manual_readXMLFile($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($path) {
		logText("manual/readXMLFile: extracting XML file '$path'", LOG_DEBUG);

		// Get the section's name from the path
		$section = basename($path, ".lwdoc");

		// Open the file
		$file = @file_get_contents($path);
		if ($file === FALSE) {
			logText("manual/readXMLFile: failed to read file '$path'", LOG_DEBUG);
			return 1;
		}
		$file = utf8_encode($file);

		// Parse it
		$doc = new DOMDocument();
		if (!$doc->loadXML($file)) {
			logText("manual/readXMLFile: failed to parse file '$path'", LOG_DEBUG);
			return 2;
		}
		$root = $doc->documentElement;
		$node = $root->firstChild;
		$version = $title = $lang = "";

		// Extract the version, language and title
		for ($i=0;$i<3;$i++) {
			while ($node && $node->nodeType != XML_ELEMENT_NODE) {
				$node = $node->nextSibling;
			}

			$name = $node->nodeName;
			$val = $node->textContent;
			if ($name == "version") {
				$version = $val;
			} elseif ($name == "language") {
				$language = $val;
			} elseif ($name == "title") {
				$title = $val;
			} else {
				logText("manual/readXMLFile('$path'): unexpected tag '$name'", LOG_DEBUG);
				return 3;
			}

			$node = $node->nextSibling;
		}
		if (!$node || $version == "" || $language == "" || $title == "") {
			logText("manual/readXMLFile('$path'): incomplete file header", LOG_DEBUG);
			return 4;
		}

		// Generate the main section
		$sections = array(
			$section => array(
				"language"	=> $language,
				"version"	=> $version,
				"in_section"	=> null,
				"is_page"	=> true,
				"in_menu"	=> ($root->getAttribute('hide') !== "1"),
				"title"		=> $title,
				"contents"	=> "",
				"subs"		=> array()
			)
		);

		// Read the sections
		$stack = array();
		while ($node) {
			$cs = empty($stack) ? $section : $stack[count($stack)-1][1];
			if ($node->nodeType == XML_ELEMENT_NODE && $node->nodeName == "section") {
				$nFile = $node->getAttribute('file');
				if ($nFile != '') {
					// This subsection must be read from another file
					if ($nFile{0} != "/")
						$nFile = dirname($path) . "/$nFile";
					$a = $this->run($nFile);
					if (!is_array($a)) {
						logText("manual/readXMLFile('$path'): error $a in sub-file '$nFile'", LOG_DEBUG);
						return $a;
					}
					$id = basename($nFile, '.lwdoc');
					$a[$id]['in_section'] = $cs;
					$sections = array_merge($sections, $a);
					array_push($sections[$cs]['subs'], $id);
					$node = $node->nextSibling;
				} else {
					// Inline subsection
					$id = $node->getAttribute('id');
					array_push($sections[$cs]['subs'], $id);
					$sections[$id] = array(
						"language"	=> $language,
						"version"	=> $version,
						"in_section"	=> $cs,
						"is_page"	=> false,
						"title"		=> $node->getAttribute('title'),
						"linkto"	=> $node->getAttribute('linkto'),
						"contents"	=> $this->getEmbeddedXHTML($node),
						"subs"		=> array()
					);
					array_push($stack, array($node, $id));
					$node = $node->firstChild;
				}
			} else {
				$node = $node->nextSibling;
				if (!empty($stack)) {
					do {
						if (!$node) {
							$x = array_pop($stack);
							$node = $x[0]->nextSibling;
						}
					} while (!$node && !empty($stack));
				}
			}
		}

		return $sections;
	}


	function getEmbeddedXHTML($node) {
		if (!$node->hasChildNodes()) {
			return $node->textContent;
		}

		$st = "";
		for ($i=0;$i<$node->childNodes->length;$i++) {
			$cnode = $node->childNodes->item($i);
			if ($cnode->nodeType == XML_TEXT_NODE) {
				$st .= $cnode->nodeValue;
			} elseif ($cnode->nodeType == XML_ELEMENT_NODE && $cnode->nodeName != 'section') {
				$st .= "<" . $cnode->nodeName;
				if ($attribnodes = $cnode->attributes) {
					$st .= " ";
					foreach ($attribnodes as $anode) {
						$st .= $anode->nodeName . "='" . $anode->nodeValue . "'";
					}
				}
				$nodeText = $this->getEmbeddedXHTML($cnode);
				if (empty($nodeText) && !$attribnodes) {
					$st .= " />";        // unary
				} else {
					$st .= ">" . $nodeText . "</" . $cnode->nodeName . ">";
				}
			}
		}
		return $st;
	}
}

?>

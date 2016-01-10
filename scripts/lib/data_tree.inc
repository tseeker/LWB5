<?php


abstract class data_gen {
	private $name;
	private $attributes;

	function __construct($name) {
		$this->name = $name;
		$this->attributes = array();
	}

	function getName() {
		return $name;
	}

	abstract function getContents();

	abstract function addContents($contents);

	function getAttribute($name) {
		return isset($this->attributes[$name]) ? $this->attributes[$name] : null;
	}

	function setAttribute($name, $value) {
		$this->attributes[$name] = $value;
	}

	function toXML($document) {
		$element = $document->createElement($this->name);
		foreach ($this->attributes as $k => $v) {
			$element->setAttribute($k, $v);
		}
		return $element;
	}

	function toLWData($document) {
		$root = $document->createElement("Node");
		$root->setAttribute("name", $this->name);
		foreach ($this->attributes as $k => $v) {
			$node = $document->createElement("Attr");
			$root->appendChild($node);
			$node->setAttribute("name", $k);
			$text = $document->createTextNode($v);
			$node->appendChild($text);
		}
		return $root;
	}
}


class data_leaf extends data_gen {

	private $contents;

	function __construct($name, $contents = "") {
		parent::__construct($name);
		$this->contents = $contents;
	}

	function getContents() {
		return $this->contents;
	}

	function addContents($contents) {
		$this->contents .= $contents;
	}

	function toXML($document) {
		$element = parent::toXML($document);
		$value = $document->createTextNode($this->contents);
		$element->appendChild($value);
		return $element;
	}

	function toLWData($document) {
		$element = parent::toLWData($document);
		$element->setAttribute("node", 0);
		if ($this->contents != '') {
			$node = $document->createElement("Text");
			$value = $document->createTextNode($this->contents);
			$node->appendChild($value);
			$element->appendChild($node);
		}
		return $element;
	}
}


class data_node extends data_gen {

	private $contents;

	function __construct($name) {
		parent::__construct($name);
		$this->contents = array();
	}

	function getContents() {
		return $this->contents;
	}

	function addContents($data) {
		if ($data instanceof data_gen) {
			array_push($this->contents, $data);
		}
	}

	function toXML($document) {
		$element = parent::toXML($document);
		foreach ($this->contents as $sub) {
			$node = $sub->toXML($document);
			$element->appendChild($node);
		}
		return $element;
	}

	function toLWData($document) {
		$element = parent::toLWData($document);
		$element->setAttribute("node", 1);
		foreach ($this->contents as $sub) {
			$node = $sub->toLWData($document);
			$element->appendChild($node);
		}
		return $element;
	}
}


?>

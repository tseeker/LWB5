<?php

class display_engine {

	function maintenance($maintenance) {
		$this->redirect = makeLink('main', 'index');
		$this->outputData();
	}


	function displayFatalError($code, $i, $i2) {
		header("Location: http://www.legacyworlds.com");
	}

	function handleInput() {
		$this->redirect = handler::$h->redirect(input::$input);
	}

	function outputData() {
		header("Location: {$this->redirect}");
	}


	function initSession() {
		session::handle(false);
	}


	function checkAuth() { }

	function initRPC() { }
}



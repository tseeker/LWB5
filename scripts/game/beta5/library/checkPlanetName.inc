<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/library/checkPlanetName.inc
//
// This function checks whether a planet name is valid or not
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------


class beta5_checkPlanetName {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
	}

	public function run($name) {
		if (trim($name) != $name) {
			$rv = 7;
		} elseif (strlen($name) > 15) {
			$rv = 1;
		} elseif (preg_match('/[^A-Za-z0-9_\.\-\+@\/'."'".' ]/', $name)) {
			$rv = 2;
		} elseif (preg_match('/\s\s+/', $name)) {
			$rv = 3;
		} elseif (strlen($name) < 2) {
			$rv = 4;
		} elseif (!preg_match('/[A-Za-z]/', $name)) {
			$rv = 5;
		} elseif ($this->planets->call('nameExists', $name)) {
			$rv = 6;
		} else {
			$rv = 0;
		}
		return $rv;
	}

}

?>

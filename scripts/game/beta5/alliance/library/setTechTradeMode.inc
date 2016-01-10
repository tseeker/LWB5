<?php

//-----------------------------------------------------------------------
// LegacyWorlds Beta 5
// Game libraries
//
// beta5/alliance/library/setTechTradeMode.inc
//
// This function allows the alliance president to change the tech trade
// mode for the alliance.
//
// Copyright(C) 2004-2008, DeepClone Development
//-----------------------------------------------------------------------

class beta5_alliance_setTechTradeMode {

	static private $okModes = array('N', 'S', 'R');

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	public function run($alliance, $mode) {
		if (!in_array($mode, self::$okModes)) {
			return false;
		}

		$x = $this->db->query("UPDATE alliance SET enable_tt = '$mode' WHERE id = $alliance");
		return !!$x;
	}
}

?>

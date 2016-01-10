<?php

//-------------------------------------------------------------
// "Punishment tick": contains the code to reset planets if
//	they are not renamed in time.
//-------------------------------------------------------------


class beta5_ticks_punishment_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->db	= $lib->game->db;
		$this->b5adm	= $lib->game->getLib('admin/beta5');
	}


	public function runTick() {
		$this->db->safeTransaction(array($this, 'punish'));
	}


	public function punish() {
		// Neutralize planets with funky names
		$q = $this->db->query(
			"SELECT id, name FROM planet "
				. "WHERE mod_check AND force_rename IS NOT NULL "
				  . "AND UNIX_TIMESTAMP(NOW())-force_rename >= 86400"
		);

		while ($r = dbFetchArray($q)) {
			$q2 = $this->db->query(
				"SELECT moderator FROM msg_warnname "
					. "WHERE planet={$r[0]} AND event='WARN' "
					. "ORDER BY id DESC LIMIT 1"
			);
			list($mod) = dbFetchArray($q2);
			if (is_null($mod)) {
				l::error("****** BUG: moderator ID not found for planet {$r[0]}");
				continue;
			}
			l::notice("resetting planet '{$r[1]}' (id: $r[0]) due to funky name");
			$this->b5adm->call('resetPlanet', $r[0], $mod, false);
		}
	}
}

?>

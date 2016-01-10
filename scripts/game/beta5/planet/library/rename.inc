<?php

class beta5_planet_rename {

	function beta5_planet_rename($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Renames a planet
	function run($pid, $name) {
		// Check the old name
		$n = addslashes($name);
		$q = $this->db->query("SELECT name,owner FROM planet WHERE id=$pid");
		list($oName,$oid) = dbFetchArray($q);
		if ($oName == $name) {
			return;
		}

		// Get the list of players who have fleets or probes
		// orbitting or moving towards the planet
		$pl = array();
		$q = $this->db->query("SELECT DISTINCT owner FROM fleet WHERE location=$pid" . (is_null($oid) ? "" : " AND owner<>$oid"));
		while ($r = dbFetchArray($q)) {
			$pl[$r[0]] = 'ORBIT';
		}
		$q = $this->db->query("SELECT DISTINCT f.owner FROM fleet f,moving_object o "
			. "WHERE f.location IS NULL AND f.moving=o.id AND o.m_to=$pid"
			. (is_null($oid) ? "" : " AND f.owner<>$oid"));
		while ($r = dbFetchArray($q)) {
			$pl[$r[0]] = 'MOVE';
		}
		// FIXME: probes

		// Send messages to the players
		$on = addslashes($oName);
		$tm = time();
		foreach ($pl as $plId => $st) {
			$this->db->query("INSERT INTO message(player,sent_on,mtype,ftype,is_new)"
				." VALUES($plId,$tm,'rename','INT',TRUE)");
			$q = $this->db->query("SELECT id FROM message "
				. "WHERE player=$plId AND sent_on=$tm AND mtype='rename' "
				. "ORDER BY id DESC LIMIT 1");
			list($mid) = dbFetchArray($q);
			$this->db->query("INSERT INTO msg_rename VALUES($mid,$pid,'$st','$on','$n')");
		}

		$this->db->query("UPDATE planet SET name='$n',renamed=$tm,mod_check=FALSE WHERE id=$pid");
	}
}

?>

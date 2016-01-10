<?php

class admin_beta5_library {
	var $index	= array();

	function admin_beta5_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->msgs	= $this->lib->game->getLib('beta5/msg');
		$this->planets	= $this->lib->game->getLib('beta5/planet');
	}


	function getPlanetsModList($mode) {
		$modes = array("r", "o", "p", "w");
		if (!in_array($mode, $modes)) {
			$mode = $modes[0];
		}

		$queries = array(
			"r"	=> "SELECT id,name,owner FROM planet WHERE status = 0 AND NOT mod_check AND force_rename IS NULL ORDER BY id",
			"o"	=> "SELECT id,name,owner FROM planet WHERE status = 0 AND mod_check AND force_rename IS NULL ORDER BY id",
			"p"	=> "SELECT id,name,owner FROM planet WHERE status = 0 AND mod_check AND force_rename IS NOT NULL AND force_rename>renamed ORDER BY id",
			"w"	=> "SELECT id,name,owner FROM planet WHERE status = 0 AND NOT mod_check AND force_rename IS NOT NULL ORDER BY id",
		);
		return array($mode,$this->db->query($queries[$mode]));
	}


	function validatePlanetName($id) {
		$this->db->query("UPDATE planet SET mod_check=TRUE,force_rename=NULL WHERE id=$id AND status=0");
	}


	function resetPlanet($id, $from, $manual = true) {
		$q = $this->db->query("SELECT owner,name FROM planet WHERE id=$id AND status=0");
		if (!dbCount($q)) {
			return;
		}
		list($owner, $name) = dbFetchArray($q);

		do {
			$rn = strtoupper(substr(md5(uniqid(rand())), 0, 7));
			$q = $this->db->query("SELECT id FROM planet WHERE name='P-[$rn]'");
		} while(dbCount($q));
		$this->planets->call('rename', $id, "P-[$rn]");

		$this->db->query("UPDATE planet SET ifact=3,mfact=3,turrets=3,pop=2000,mod_check=TRUE,force_rename=NULL WHERE id=$id");

		if (!is_null($owner)) {
			$this->planets->call('ownerChange', $id);
			$this->msgs->call('send', $owner, 'warnname', array(
				"moderator"	=> $from,
				"planet"	=> $id,
				"p_name"	=> $name,
				"event"		=> ($manual ? 'NEUT' : 'ANEUT')
			));
		}
	}


	function sendPlanetWarning($id, $from) {
		$ts = time();
		$q = $this->db->query("SELECT owner,name FROM planet WHERE id=$id");
		list($owner, $name) = dbFetchArray($q);
		
		$this->db->query("UPDATE planet SET mod_check=TRUE,renamed=$ts-(16*86400),force_rename=$ts WHERE id=$id");
		$this->msgs->call('send', $owner, 'warnname', array(
			"moderator"	=> $from,
			"planet"	=> $id,
			"p_name"	=> $name,
			"event"		=> 'WARN'
		));
	}


	function sendSpam($from, $subject, $contents) {
		// Create the "spam record"
		$spamID = $this->db->query(
			"INSERT INTO admin_spam(sent_by, subject, contents) VALUES ("
				. "$from, '" . addslashes($subject) . "', '" . addslashes($contents) . "')"
		);
		if (!$spamID) {
			logText("Admin spam '$subject' could not be sent in game " . $this->game->text, LOG_ERR);
			return;
		}

		// Get the list of all active players
		$q = $this->db->query(
			"SELECT id FROM player WHERE quit IS NULL OR UNIX_TIMESTAMP(NOW()) - quit < 86400"
		);
		if (!($q && dbCount($q))) {
			logText("Could not fetch the list of players while sending admin spam '$subject'", LOG_ERR);
			return;
		}

		// Spam, spam, spam!
		while ($r = dbFetchArray($q)) {
			$this->msgs->call('send', $r[0], 'admin', array(
				"spam"		=> $spamID
			), 'IN');
		}
	}
}

?>

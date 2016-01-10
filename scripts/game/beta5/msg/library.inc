<?php

class beta5_msg_library {
	var $index	= array(
		'get',
		'getHeaders',
		'send',
		'sendInAlliance',
		'sendToPlanet',
		'sendToPlayer'
	);

	function beta5_msg_library($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	// Lists custom folders for a player
	function getCustomFolders($pid) {
		$q = $this->db->query("SELECT id,name FROM custom_folder WHERE player = $pid ORDER BY name");
		$a = array();
		while ($r = dbFetchArray($q)) {
			$a[$r[0]] = $r[1];
		}
		return $a;
	}

	// Creates a custom folder
	function createFolder($pid, $name) {
		$this->db->query("INSERT INTO custom_folder(name,player) VALUES('".addslashes($name)."',$pid)");
	}

	// Renames a custom folder
	function renameFolder($fid, $name) {
		$this->db->query("UPDATE custom_folder SET name = '".addslashes($name)."' WHERE id=$fid");
	}

	// Flushes messages from a folder
	function flushFolder($pid, $ft, $fid) {
		$qs = "UPDATE message SET deleted=TRUE WHERE player=$pid AND ftype='$ft'";
		if ($ft == 'CUS') {
			$qs .= " AND fcustom=$fid";
		}
		$this->db->query($qs);
	}

	// Deletes a custom folder
	function deleteFolder($pid, $fid) {
		$this->db->query("UPDATE message SET deleted=TRUE WHERE player=$pid AND ftype='CUS' AND fcustom=$fid");
		$this->db->query("DELETE FROM custom_folder WHERE player=$pid AND id=$fid");
	}

	// Get count of all messages in a player's folder
	function getAll($pid, $ft, $cfid = null) {
		$qs = "SELECT COUNT(*) FROM message WHERE player=$pid AND ftype='$ft' AND NOT deleted";
		if (!is_null($cfid)) {
			$qs .= " AND fcustom = $cfid";
		}
		$q = $this->db->query($qs);
		if (!($q && count($q) == 1)) {
			return 0;
		}
		list($r) = dbFetchArray($q);
		return $r;
	}

	// Get count of new messages in a player's folder
	function getNew($pid, $ft = null, $cfid = null) {
		$qs = "SELECT COUNT(*) FROM message WHERE player=$pid AND is_new AND NOT deleted";
		if (!is_null($ft)) {
			$qs .= " AND ftype='$ft'";
			if (!is_null($cfid)) {
				$qs .= " AND fcustom = $cfid";
			}
		}
		$q = $this->db->query($qs);
		if (!($q && count($q) == 1)) {
			return 0;
		}
		list($r) = dbFetchArray($q);
		return $r;
	}

	// Sets a message status to "read"
	function setRead($mId) {
		$this->db->query("UPDATE message SET is_new = FALSE WHERE id = $mId");
	}

	// Sets a message as deleted
	function delete($mId, $pId) {
		$this->db->query("UPDATE message SET deleted = TRUE WHERE id = $mId AND player = $pId");
	}

	// Moves a message
	function move($mId, $pId, $tTp, $tId) {
		$qs = "ftype='$tTp',fcustom=" . ($tTp == "CUS" ? $tId : "NULL");
		$this->db->query("UPDATE message SET $qs WHERE id = $mId AND player = $pId");
	}


	// Loads message formatting scripts
	function loadFormat($type, $lang, $player) {
		if (!is_null($this->formats[$type])) {
			return $this->formats[$type];
		}

		$path = config::$main['scriptdir'] . "/game/beta5/msgformat/$lang/$type.inc";
		if (!(file_exists($path) && is_readable($path))) {
			return null;
		}
		require_once($path);
		eval('$this->formats[$type] = new msgformat_'.$type.'($this->lib->game);');
		$this->formats[$type]->game = $this->game;
		$this->formats[$type]->player = $player;
		return $this->formats[$type];
	}
}

?>

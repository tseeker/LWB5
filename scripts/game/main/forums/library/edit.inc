<?php

class main_forums_edit {

	function main_forums_edit($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($a, $pid, $sub, $txt, $ec, $es) {
		$q = $this->db->query("SELECT topic FROM f_post WHERE id=$pid AND deleted IS NULL");
		list($tid) = dbFetchArray($q);
		$this->lib->call('markUnread', $tid,$a);
		$tm = time();
		$qs = "UPDATE f_post SET edited=$tm,edited_by=$a,title='".addslashes($sub)."',contents='"
			.addslashes($txt)."',enable_code=".dbBool($ec).",enable_smileys="
			. dbBool($es) . " WHERE id=$pid AND deleted IS NULL";
		return !is_null($this->db->query($qs));
	}
}

?>

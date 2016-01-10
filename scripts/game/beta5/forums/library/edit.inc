<?php

class beta5_forums_edit {

	function beta5_forums_edit($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	function run($a, $pid, $sub, $txt, $ec, $es) {
		$tm = time();
		$q = $this->db->query("SELECT topic FROM af_post WHERE id=$pid");
		list($tid) = dbFetchArray($q);
		$this->lib->call('markUnread', $tid,$a);
		$qs = "UPDATE af_post SET edited=$tm,edited_by=$a,title='".addslashes($sub)."',contents='"
			.addslashes($txt)."',enable_code=".dbBool($ec).",enable_smileys="
			. dbBool($es) . " WHERE id=$pid";
		return !is_null($this->db->query($qs));
	}
}

?>

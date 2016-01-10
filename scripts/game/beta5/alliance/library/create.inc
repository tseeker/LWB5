<?php

class beta5_alliance_create {

	function beta5_alliance_create($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}


	// Create an alliance
	function run($tag, $name, $founder) {
		$t = addslashes($tag);

		$this->db->query("INSERT INTO alliance_grade(name) VALUES ('DEFGRADE-$t')");
		$q = $this->db->query("SELECT id FROM alliance_grade WHERE name='DEFGRADE-$t' AND alliance IS NULL");
		list($gid) = dbFetchArray($q);

		$this->db->query(
			"INSERT INTO alliance(tag,name,leader,successor,default_grade) VALUES('$t','"
			. addslashes($name) . "',$founder,NULL,$gid)"
		);

		$i = $this->lib->call('getId', $tag);
		if (is_null($i)) {
			return null;
		}
		$this->db->query("UPDATE alliance_grade SET name=NULL,alliance=$i WHERE name='DEFGRADE-$t' AND alliance IS NULL");
		$this->db->query("UPDATE player SET alliance=$i,a_status='IN',a_vote=NULL WHERE id=$founder");

		$rkLib =& $this->lib->game->getLib('main/rankings');
		$rt = $rkLib->call('getType', 'a_general');
		$rkLib->call('append', $rt, $tag);

		return $i;
	}
}

?>

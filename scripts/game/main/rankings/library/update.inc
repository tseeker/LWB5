<?php

class main_rankings_update {

	function main_rankings_update($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($id, $data) {
		// $data = array($points => array($name));

		$plist = array_keys($data);
		sort($plist);
		$plist = array_reverse($plist);

		$a = array(); $cr = 1;
		for ($i=0;$i<count($plist);$i++) {
			$s = 0;
			foreach	($data[$plist[$i]] as $pn) {
				array_push($a, array(
					'name'		=> $pn,
					'points'	=> $plist[$i],
					'ranking'	=> $cr
				));
				$s ++;
			}
			$cr += $s;
		}

		$this->db->query("DELETE FROM ranking WHERE r_type=$id");
		foreach	($a as $entry) {
			$this->db->query("INSERT INTO ranking(r_type,id,additional,points,ranking) VALUES("
				. "$id, '" . addslashes($entry['name']) . "',NULL,".$entry['points'].",".$entry['ranking'].")");
		}
	}
}

?>

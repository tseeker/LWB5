<?php

class main_forums_substitute {
	var $code	= null;
	var $smiley	= null;

	function main_forums_substitute($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($text, $ec, $es) {
		$src = array('/\\n/','/\\r/'); $dst = array("<br/>",'');
		$text = utf8entities($text, ENT_NOQUOTES);
		$ec = ($ec == 't');
		$es = ($es == 't');

		if ($ec) {
			if (is_array($this->code)) {
				foreach	($this->code as $s => $d) {
					array_push($src, '/'.$s.'/i');
					array_push($dst, $d);
				}
			} else {
				$this->codes = array();
				$q = $this->db->query("SELECT * FROM f_code");
				while ($r = dbFetchArray($q)) {
					$this->code[$r[0]] = $r[1];
					array_push($src, '/'.$r[0].'/i');
					array_push($dst, $r[1]);
				}
			}
		}
		if ($es) {
			if (is_array($this->smiley)) {
				foreach	($this->smiley as $s => $d) {
					array_push($src, '/'.$s.'/i');
					array_push($dst, $d);
				}
			} else {
				$this->smiley = array();
				$q = $this->db->query("SELECT * FROM f_smiley");
				while ($r = dbFetchArray($q)) {
					$fn = getStatic("main/pics/smiles/icon_".$r[1].".gif");
					if (is_null($fn)) {
						continue;
					}
					$code = "<img src='$fn' alt='[S]' />";
					$this->smiley[$r[0]] = $code;
					array_push($src, '/'.$r[0].'/i');
					array_push($dst, $code);
				}
			}
		}

		return preg_replace($src, $dst, $text);
	}
}

?>

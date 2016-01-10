<?php

class main_manual_updateSections {

	function main_manual_updateSections($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($sections) {
		$versions = array();
		foreach ($sections as $sName => $sData) {
			if (is_null($versions[$sData['version']])) {
				$versions[$sData['version']] = "'" . addslashes($sData['version']) . "'";
			}
			$this->db->query("DELETE FROM man_section WHERE name='" . addslashes($sName)
				. "' AND version=" . $versions[$sData['version']]
				. " AND lang='" . addslashes($sData['language']) . "'");
		}

		// Insert the data itself
		$now = time();
		foreach ($sections as $sName => $sData) {
			if (is_null($versions[$sData['version']])) {
				continue;
			}
			$this->db->query("INSERT INTO man_section (version,lang,name,last_update,is_page,in_menu,title,contents) VALUES("
				. $versions[$sData['version']] . ",'" . addslashes($sData['language']) . "','"
				. addslashes($sName) . "',$now," . dbBool($sData['is_page']) . ","
				. dbBool($sData['in_menu']) . ",'" . addslashes($sData['title'])
				. "','" . addslashes($sData['contents']) . "')");
			$q = $this->db->query("SELECT id FROM man_section WHERE name='" . addslashes($sName)
				. "' AND version=" . $versions[$sData['version']]
				. " AND lang='" . addslashes($sData['language']) . "'");
			list($sections[$sName]['dbid']) = dbFetchArray($q);
		}

		// Create links between data
		foreach ($sections as $sName => $sData) {
			if (count($sData['subs'])) {
				// In / After section
				foreach ($sData['subs'] as $i => $sn) {
					$ri = $i - 1;
					while ($ri >= 0) {
						$sn2 = $sections[$sData['subs'][$ri]];
						if ($sn2['is_page'] == $sections[$sn]['is_page']) {
							$previous = $sn2['dbid'];
							break;
						}
						$ri --;
					}
					if ($ri == -1) {
						$previous = $sData['dbid'];
					}

					$this->db->query("UPDATE man_section SET in_section={$sData['dbid']},after_section=$previous WHERE id={$sections[$sn]['dbid']}");
				}
			}

			if ($sData['linkto'] != '') {
				if (is_array($sections[$sData['linkto']])) {
					$this->db->query("UPDATE man_section SET link_to={$sections[$sData['linkto']]['dbid']} WHERE id={$sData['dbid']}");
				} else {
					$q = $this->db->query("SELECT id FROM man_section WHERE name='" . addslashes($sData['linkto']) . "' AND version="
						. $versions[$sData['version']] . " AND lang='" . addslashes($sData['language']) . "'");
					if ($q && dbCount($q) == 1) {
						list($toid) = dbFetchArray($q);
						$this->db->query("UPDATE man_section SET link_to=$toid WHERE id={$sData['dbid']}");
					}
				}
			}
		}

		// Update indices
		$banwords = array();
		foreach ($sections as $sName => $sData) {

			if (!is_array($banwords[$sData['language']])) {
				$ban = array();
				$q = $this->db->query("SELECT word FROM man_index_ban WHERE lang='" . addslashes($sData['language']) . "'");
				while ($r = dbFetchArray($q)) {
					array_push($ban, $r[0]);
				}
				$banwords[$sData['lang']] = $ban;
			}

			$text = preg_replace('/<[^>]+>/', '', $sData['title']) . " ";
			$text .= preg_replace('/<[^>]+>/', '', $sData['contents']);
			$text = preg_replace('/["\s\.,;:!\(\)<>&]+/', ' ', $text);

			$tl = explode(' ', $text);
			$rtl = array();
			foreach ($tl as $word) {
				$word = strtolower($word);
				if ($word != '' && !in_array($word, $banwords[$sData['lang']])) {
					$rtl[$word] ++;
				}
			}
			foreach ($rtl as $word => $count) {
				$this->db->query("INSERT INTO man_index(word,wcount,lang,section) VALUES ('" . addslashes($word)
					. "',$count,'" . addslashes($sData['language']) . "',{$sData['dbid']})");
			}
		}
	}
}

?>

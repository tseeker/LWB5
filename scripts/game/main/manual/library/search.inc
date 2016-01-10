<?php

class main_manual_search {

	function main_manual_search($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	function run($text, $lang) {
		$version = "'{$this->lib->mainClass->version}'";
		$text = preg_replace('/\s+/', ' ', trim($text));

		// Read the banned words list
		$banList = array();
		$q = $this->db->query("SELECT word FROM man_index_ban WHERE lang='$lang'");
		while ($r = dbFetchArray($q)) {
			array_push($banList, $r[0]);
		}

		// Generate the three lists: required, excluded and normal words
		$required = $excluded = $normal = array();
		$tl = explode(' ', $text);
		foreach ($tl as $word) {
			if ($word{0} == '+' || $word{0} == '-') {
				$qual = $word{0};
				$word = substr($word, 1);
			} else {
				$qual = '';
			}

			$word = preg_replace('/["\s\.,;:!\(\)<>&]+/', ' ', $word);
			$rwl = explode(' ', $word);
			foreach ($rwl as $rword) {
				$rword = strtolower($rword);
				if (in_array($rword, $banList)) {
					continue;
				}
				$rword = addslashes($rword);

				if ($qual == '') {
					array_push($normal, $rword);
				} elseif ($qual == '+') {
					array_push($required, $rword);
				} else {
					array_push($excluded, $rword);
				}
			}
		}
		$required = array_unique($required);
		$excluded = array_unique($excluded);
		$normal = array_unique($normal);

		// Read the index for each list
		$words = array($required, $normal, $excluded);
		$lists = array(array(), array(), array());
		for ($i=0;$i<count($words);$i++) {
			if (empty($words[$i])) {
				continue;
			}

			$q = $this->db->query("SELECT i.word,i.wcount,i.section"
					 . " FROM man_index i,man_section s "
					. "WHERE i.lang='$lang' AND s.id=i.section AND s.version=$version "
					  . "AND word in ('" . join("','", $words[$i]) . "')");
			while ($r = dbFetchArray($q)) {
				if (!is_array($lists[$i][$r[0]])) {
					$lists[$i][$r[0]] = array();
				}
				$lists[$i][$r[0]][$r[2]] = $r[1];
			}
		}

		if (count($words[0])) {
			// Look for sections that have all required words
			$secReq = array();
			foreach ($required as $word) {
				if (!is_array($lists[0][$word])) {
					continue;
				}
				foreach (array_keys($lists[0][$word]) as $s) {
					$secReq[$s] ++;
				}
			}

			$secOk = array();
			foreach ($secReq as $s => $n) {
				if ($n == count($words[0]) && !in_array($s, $secOk)) {
					array_push($secOk, $s);
				}
			}
		} else {
			// Look for sections that have these words
			$secOk = array();
			if (count($words[1])) {
				foreach ($lists[1] as $word => $sections) {
					foreach (array_keys($sections) as $section) {
						if (!in_array($section, $secOk)) {
							array_push($secOk, $section);
						}
					}
				}
			}
		}

		// Remove sections that have excluded words
		if (count($words[2])) {
			// Generate a list of banned sections
			$bannedSections = array();
			foreach ($lists[2] as $word => $sections) {
				foreach ($sections as $s => $c) {
					if (!in_array($s, $bannedSections)) {
						array_push($bannedSections, $s);
					}
				}
			}

			// Remove banned sections from the list
			$newList = array();
			foreach ($secOk as $s) {
				if (!in_array($s, $bannedSections)) {
					array_push($newList, $s);
				}
			}
			$secOk = $newList;
		}

		/*
		 * For each section in the generated list, find out:
		 *  - how many occurences of each required or normal word it
		 * has,
		 *  - the maximum amount of occurences a section has of each
		 * required or normal word.
		 */

		// Initialise the word list
		$sWords = array_unique(array_merge($words[0], $words[1]));
		$mWords = array();
		foreach ($sWords as $word) {
			$mWords[$word] = 0;
		}

		// Generate the section list and update maximum counts
		$wPerSec = array();
		foreach ($secOk as $section) {
			$wPerSec[$section] = array();
			foreach ($sWords as $word) {
				$count = $lists[is_null($lists[0][$word]) ? 1 : 0][$word][$section];
				if (!is_null($count)) {
					$wPerSec[$section][$word] = $count;
					if ($count > $mWords[$word]) {
						$mWords[$word] = $count;
					}
				} else {
					$wPerSec[$section][$word] = 0;
				}
			}
		}

		/* Now we assign a "distance" from the optimal value to
		 * each section by using each word as a dimension; the closer
		 * a section to the optimal value, the higher it will be in
		 * the displayed results.
		 */

		// Eliminate words that are not on any section
		$nWords = array();
		foreach ($sWords as $word) {
			if ($mWords[$word] > 0) {
				array_push($nWords, $word);
			}
		}

		// Compute the maximum distance
		$sum = 0;
		foreach ($nWords as $word) {
			$d = $mWords[$word];
			$sum += $d * $d;
		}
		$dMax = sqrt($sum);

		// Compute the revelance of each section
		$dSec = array();
		foreach ($secOk as $section) {
			$sum = 0;
			foreach ($nWords as $word) {
				$d = $mWords[$word] - $wPerSec[$section][$word];
				$sum += $d * $d;
			}
			$dSec[$section] = 1 - (sqrt($sum) / $dMax);
		}

		// Find out the page for each section
		$pages = array();
		$nSecs = array();
		foreach ($secOk as $section) {
			$pageId = $this->lib->call('getPageId', $section);
			if (is_null($pageId)) {
				continue;
			}
			$pages[$pageId] += $dSec[$section];
			$nSecs[$pageId] += 1;
		}

		// Sort the pages according to their points
		$pts = array();
		foreach (array_keys($pages) as $p) {
			$pp = sprintf("%.2f", $pages[$p] / $nSecs[$pageId]);
			if (!is_array($pts[$pp])) {
				$pts[$pp] = array();
			}
			array_push($pts[$pp], $p);
		}
		$k = array_keys($pts);
		sort($k);

		// Return the results
		$results = array();
		foreach (array_reverse($k) as $points) {
			foreach ($pts[$points] as $p) {
				array_push($results, $p);
			}
		}

		return $results;
	}
}

?>

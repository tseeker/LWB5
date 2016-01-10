<?php

class beta5_listing {

	function beta5_listing($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
	}

	// Generate a listing for the generic Listing JS component
	function run($data, $conf, $param, $md5) {
		// Extract parameters
		$pArray = explode('#', $param);
		$page		= (int)array_shift($pArray);
		$perPage	= (int)array_shift($pArray);
		$sField		= array_shift($pArray);
		$sDir		= (array_shift($pArray) == 1);
		$search		= join('#', $pArray);
		$pArray = explode('!', $search);
		$srMode		= (int)array_shift($pArray);
		$srText		= join('!', $pArray);

		// Apply search parameters
		if (is_array($conf['searchModes']) && count($conf['searchModes'])
				&& $srText != "" && count($data)) {
			if (is_null($conf['searchModes'][$srMode])) {
				$srMode = 0;
			}
			$field = $conf['searchModes'][$srMode];

			$nRes = array();
			foreach	($data as $e) {
				if (stristr($e[$field], $srText) === false) {
					continue;
				}
				array_push($nRes, $e);
			}
			$data = $nRes;
		}

		// Sort it
		if (is_array($conf['sortable']) && count($conf['sortable']) && count($data)) {
			$sFields = array_keys($conf['sortable']);
			if (!in_array($sField, $sFields)) {
				$sField = $sFields[0];
			}
			$sType = $conf['sortable'][$sField];

			$sArr = array();
			foreach ($data as $i => $e) {
				$v = $e[$sField];
				if (is_null($sArr[$v])) {
					$sArr[$v] = array();
				}
				array_push($sArr[$v], $i);
			}

			$sVals = array_keys($sArr);
			sort($sVals, $sType);
			if ($sDir)
				$sVals = array_reverse($sVals);

			$nRes = array();
			foreach ($sVals as $v) {
				foreach ($sArr[$v] as $i) {
					array_push($nRes, $data[$i]);
				}
			}
			$data = $nRes;
		}

		// Apply paging
		if (is_array($conf['perPage']) && count($conf['perPage'])) {
			if (!in_array($perPage, $conf['perPage'])) {
				$perPage = $conf['perPage'][0];
			}

			$c = count($data);
			$m = $c % $perPage;
			$nPages = ($c - $m) / $perPage + ($m ? 1 : 0);

			if (count($data)) {
				if ($page < 0) {
					$page = 0;
				} else if ($page >= $nPages) {
					$page = $nPages - 1;
				}

				$nRes = array();
				$fi = $page * $perPage;
				for ($i=$fi;$i<min($c, $fi + $perPage);$i++) {
					array_push($nRes, $data[$i]);
				}
				$data = $nRes;
			}
		}

		// Check MD5 value
		$nMD5 = md5(serialize($data));
		if ($md5 == $nMD5) {
			return "KEEP#$nPages";
		}

		// Generate output
		$rStr = array("$nMD5#$nPages");
		foreach ($data as $e) {
			$a = array();
			foreach ($conf['output'] as $f) {
				array_push($a, utf8entities($e[$f]));
			}
			array_push($rStr, join($a, "#"));
		}

		return join("\n", $rStr);
	}
}

?>

<?php

//---------------------------------------------------------------
// "Universe tick": generates sections of the universe
// Executed once every 5 minutes, starting from 7:02:00 ST
//---------------------------------------------------------------


class beta5_ticks_universe_library {

	public function __construct($lib) {
		$this->lib	= $lib;
		$this->game	= $this->lib->game;
		$this->db	= $this->game->db;
		$this->main	= $this->game->getLib('main');
		$this->planets	= $this->game->getLib('beta5/planet');
	}

	public function runTick() {
		$this->db->safeTransaction(array($this, 'genUniverse'));
	}

	public function genUniverse() {
		// Run a special version of the tick if we are using a CTF map
		$map = (int) $this->game->params['usemap'];
		if ($map > 0) {
			$this->handleCTFMap($map);
			return;
		}

		// Read the game's parameters
		$np = $this->game->params['nebulaprob'];
		$this->nebulaProb = ($np > -1 && $np < 21) ? $np : 2;
		$this->minSystems = $this->game->params['minsystems'];
		$this->maxSystems = $this->game->params['maxsystems'];
		$zs = $this->game->params['zonesize'];
		$this->zoneSize = ($zs <= 0 || $zs > 10) ? 2 : $zs;

		// If there is a maximum value, check it
		if ($this->maxSystems) {
			$ns = $this->getAllSystems();
			if ($ns >= $this->maxSystems) {
				return;
			}
		}

		// Get the amount of free systems
		$this->reassignEmpty();
		$ns = $this->getFreeSystems();
		if ($ns >= $this->minSystems) {
			return;
		}

		// Get the latest system ID
		$q = $this->db->query("SELECT MAX(id) FROM system");
		list($id1) = dbFetchArray($q);

		// Read the universe generator's parameters
		list($dir, $size) = $this->getSystemGenParm();
		$ldir = $dir; $lsize = $lsize;

		// Generate zones
		while ($ns < $this->minSystems) {
			list($x1, $y1, $x2, $y2) = $this->getNextRectangle($dir, $size);
			$this->makeZone($x1, $y1, $x2, $y2);
			$ns = $this->getFreeSystems();
		}

		if ($ldir != $dir || $lsize != $size) {
			// Update the universe generator's parameters
			if ($ldir != $dir) {
				$this->db->query("UPDATE gdata SET value='$dir' WHERE id='sg_dir'");
			}
			if ($lsize != $size) {
				$this->db->query("UPDATE gdata SET value='$size' WHERE id='sg_len'");
			}

			// Make sure we have a first ID
			if (is_null($id1)) {
				$q = $this->db->query("SELECT MIN(id) FROM system");
				list($id1) = dbFetchArray($q);
			} else {
				$id1++;
			}

			// Read the last ID
			$q = $this->db->query("SELECT MAX(id) FROM system");
			list($id2) = dbFetchArray($q);

			// Generate the systems' planets
			for ($i=$id1;$i<=$id2;$i++) {
				$this->genSystem($i);
			}

			// Get the size of the next area
			list($x1,$y1,$x2,$y2) = $this->getNextRectangle($dir, $size);
			$w = abs($x2 - $x1) + 1;
			$h = abs($y2 - $y1) + 1;

			$q = $this->db->query("SELECT MAX(id) FROM planet");
			list($idp) = dbFetchArray($q);

			// Generate new planets
			$this->main->call('requestGenPlanets', $idp + 1, ($w + 5) * $h * 6);
		}
	}


	private function handleCTFMap($mapID) {
		// Check if there are systems
		$q = $this->db->query("SELECT COUNT(*) FROM system");
		if (!($q && dbCount($q))) {
			return;
		}
		list($nSys) = dbFetchArray($q);
		if ($nSys > 0) {
			return;
		}

		// No systems -> we must generate the map
		// Start by loading the template's definition
		$q = $this->db->query("SELECT width,height FROM ctf_map_def WHERE id=$mapID");
		if (!($q && dbCount($q))) {
			logText("Could not load map definition #$mapID", LOG_ERR);
			return;
		}
		list($width, $height) = dbFetchArray($q);

		// Load the template's layout
		$q = $this->db->query("SELECT * FROM ctf_map_layout WHERE map=$mapID");
		if (!($q && dbCount($q) == $width * $height)) {
			logText("Could not load map layout for template #$mapID", LOG_ERR);
			return;
		}

		// Create systems and generate CTF-specific data along the way
		$ids = array();
		$targets = array();
		while ($row = dbFetchHash($q)) {
			// Insert the map system
			$qStr = "INSERT INTO system(x,y,prot,assigned,nebula) VALUES ("
				. "{$row['sys_x']},{$row['sys_y']},0,FALSE,"
				. (($row['sys_type'] == 'S') ? "0" : $row['sys_type']) . ")";
			$sys = $this->db->query($qStr);

			if (!$sys) {
				logText("Error inserting system at ({$row['sys_x']},{$row['sys_y']})", LOG_ERR);
				return;
			}
			array_push($ids, $sys);

			if ($row['sys_type'] != 'S') {
				continue;
			}

			if ($row['alloc_for'] == 0) {
				// Insert a target record
				$this->db->query("INSERT INTO ctf_target(system) VALUES ($sys)");
				array_push($targets, $sys);
			} else {
				// Insert an alliance area record
				$this->db->query("INSERT INTO ctf_alloc(system,alliance,spawn_point) VALUES ("
					. "$sys,{$row['alloc_for']},'{$row['spawn_here']}')");
			}
		}

		// Generate system contents
		foreach ($ids as $sys) {
			$this->genSystem($sys);
		}

		// Upgrade target planets
		$q = $this->db->query("SELECT id FROM planet WHERE system IN (" . join(',', $targets) . ")");
		while ($r = dbFetchArray($q)) {
			$q = $this->db->query("UPDATE planet SET ifact=33, mfact=33, turrets=90 WHERE id = {$r[0]}");
			$this->planets->call('updateHappiness', $r[0]);
		}
	}


	private function reassignEmpty() {
		$q = $this->db->query("SELECT s.id,COUNT(p.*) AS cnt FROM system s,planet p "
			. "WHERE s.assigned AND s.nebula=0 AND p.owner IS NULL AND p.status=0 AND p.system=s.id "
			. "GROUP BY s.id HAVING COUNT(p.*)=6");
		if (!($q && dbCount($q))) {
			return;
		}

		$sids = array();
		while ($r = dbFetchArray($q)) {
			$allocate = true;

			$q2 = $this->db->query("SELECT id FROM planet WHERE system = {$r[0]}");
			while ($allocate && $r2 = dbFetchArray($q2)) {
				$allocate = $this->planets->call('canAllocate', $r2[0]);
			}

			if ($allocate) {
				array_push($sids, $r[0]);
			}
		}

		if (empty($sids)) {
			return;
		}

		$q = $this->db->query("SELECT DISTINCT p.system FROM fleet f,planet p "
			. "WHERE f.location IS NOT NULL AND f.location=p.id AND p.system IN (" . join(',', $sids) . ")");
		if (!$q) {
			return;
		}

		$flids = array();
		while ($r = dbFetchArray($q)) {
			array_push($flids, $r[0]);
		}

		$rsids = array();
		foreach ($sids as $sid) {
			if (!in_array($sid, $flids)) {
				array_push($rsids, $sid);
				logText("beta5/universe: system $sid will be reassigned");
			}
		}

		if (empty($rsids)) {
			return;
		}

		$this->db->query("UPDATE system SET assigned=FALSE WHERE id IN (" . join(',', $rsids) . ")");
	}


	private function getAllSystems() {
		$q = $this->db->query("SELECT COUNT(*) FROM system WHERE nebula = 0");
		list($n) = dbFetchArray($q);
		return $n;
	}


	private function getFreeSystems() {
		$q = $this->db->query("SELECT COUNT(*) FROM system WHERE NOT assigned AND nebula = 0");
		list($n) = dbFetchArray($q);
		return $n;
	}


	private function getSystemGenParm() {
		$q = $this->db->query("SELECT id,value FROM gdata WHERE id IN ('sg_dir','sg_len')");
		$rs = array();
		while ($r = dbFetchArray($q)) {
			$rs[$r[0] == "sg_dir" ? 0 : 1] = $r[1];
		}
		return $rs;
	}


	private function getNextRectangle(&$dir, &$size) {
		$zs = $this->zoneSize;
		$zsT = 1 + 2*$zs;
		$zsI = $zsT - 1;

		if ($dir < 2) {
			//$x1 = -2 - 5*$size/2;
			$x1 = -$zs - $zsT * $size / 2;
			// $y1 = ($dir == 0) ? (-2 - 5*$size/2) : (3 + 5*$size/2);
			$y1 = ($dir == 0) ? (-$zs - $zsT * $size / 2) : ($zs + 1 + $zsT * $size / 2);
			//$x2 = $x1 + 4 + (($dir == 1) ? $size * 5 : 0);
			$x2 = $x1 + $zs * 2 + (($dir == 1) ? $size * $zsT : 0);
			//$y2 = $y1 + 4 + (($dir == 0) ? $size * 5 : 0);
			$y2 = $y1 + $zs * 2 + (($dir == 0) ? $size * $zsT : 0);
		} else {
			$m = $size % 2;
			//$x2 = 7 + 5*($size-$m)/2;
			$x2 = $zsT + $zs + $zsT * ($size-$m) / 2;
			//$y2 = ($dir == 2) ? (7 + 5*($size-$m)/2) : (-3 - 5*($size-$m)/2);
			$y2 = ($dir == 2) ? ($zsT + $zs + $zsT * ($size - $m) / 2) : (- $zs - 1 - $zsT * ($size - $m) /2);
			//$x1 = $x2 - 4 - (($dir == 3) ? $size * 5 : 0);
			$x1 = $x2 - $zsT + 1 - (($dir == 3) ? $size * $zsT : 0);
			//$y1 = $y2 - 4 - (($dir == 2) ? $size * 5 : 0);
			$y1 = $y2 - $zsT + 1 - (($dir == 2) ? $size * $zsT : 0);
		}

		$dir = ($dir + 1) % 4;
		if ($dir % 2 == 0) {
			$size ++;
		}
		return  array($x1,$y1,$x2,$y2);
	}


	private function findBorderNebulas($x1, $y1, $x2, $y2) {
		$x1--;$y1--;$x2++;$y2--;

		$q = $this->db->query(
			"SELECT COUNT(*) FROM system WHERE nebula > 0 AND ("
			. "((x=$x1 OR x=$x2) AND y>=$y1 AND y<=$y2) OR ((y=$y1 OR y=$y2) AND x>$x1 AND x<$y2))"
		);
		list($c) = dbFetchArray($q);
		return $c;
	}


	private function getAdjacentNebula($x, $y) {
		list($x1,$x2,$y1,$y2) = array($x+1, $x-1, $y+1, $y-1);
		$q = $this->db->query(
			"SELECT SUM(nebula) FROM system "
				. "WHERE (x=$x AND y=$y1) OR (x=$x AND y=$y2) OR (x=$x1 AND y=$y) OR (x=$x2 AND y=$y)"
		);
		list($r) = dbFetchArray($q);
		return min((int)$r, 4);
	}


	private function nebulaPass($x1, $y1) {
		$ds = array(0,1,1,1,2,2,2,2,2,3);
		$c = 0;

		$zsT = $this->zoneSize * 2 + 1;
		for ($x = $x1; $x < $x1 + $zsT; $x++) {
			for ($y = $y1; $y < $y1 + $zsT; $y++) {
				$q = $this->db->query("SELECT assigned,nebula FROM system WHERE x=$x AND y=$y");
				list($as,$nb) = dbFetchArray($q);
				if ($as == 'f' || $nb > 0)
					continue;

				$ap = $this->getAdjacentNebula($x,$y);
				if ($ap == 0) {
					continue;
				}

				$ap -= $ds[rand(0,9)];
				if ($ap <= 0) {
					$this->db->query("UPDATE system SET assigned=FALSE,nebula=0 WHERE x=$x AND y=$y");
				} else {
					$this->db->query("UPDATE system SET nebula=$ap WHERE x=$x AND y=$y");
				}
				$c = 1;
			}
		}

		return $c;
	}


	private function makeNebulas($x1,$y1) {
		do {
			$c = $this->nebulaPass($x1,$y1);
		} while ($c);
	}


	private function makeCluster($x1,$y1) {
		$np = array(1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,4);

		$zs = $this->zoneSize * 2;
		$x = $x1 + $zs; $y = $y1 + $zs;
		$nl = $this->findBorderNebulas($x1,$y1,$x,$y);

		$zsT = $zs + 1;
		for ($x = $x1; $x < $x1 + $zsT; $x++) {
			for ($y = $y1; $y < $y1 + $zsT; $y++) {
				if (rand(0,99) <= $this->nebulaProb && $nl == 0) {
					$v = $np[rand(0,19)];
					$nl ++;
				} else {
					$v = 0;
				}
				$this->db->query("INSERT INTO system(x,y,prot,assigned,nebula) VALUES($x,$y,0,TRUE,$v)");
			}
		}

		if ($nl > 0) {
			$this->makeNebulas($x1,$y1);
		}
		$this->db->query("UPDATE system SET assigned=FALSE WHERE x>=$x1 AND y>=$y1 AND x<$x AND y<$y AND nebula=0");
	}


	private function makeZone($x1,$y1,$x2,$y2) {
		$zsT = $this->zoneSize * 2 + 1;
		for ($x=$x1;$x<$x2;$x+=$zsT) {
			for ($y=$y1;$y<$y2;$y+=$zsT) {
				$this->makeCluster($x,$y);
			}
		}
	}


	private function genPlanets($system) {
		$npl = 6;

		// Generate the turrets
		$ttn = 17;
		$i = -1;
		$tna = array();
		for ($i=0;$i<$npl;$i++) {
			$tna[$i] = 3;
		}
		while ($ttn) {
			$i = ($i + 1) % $npl;
			if ($tna[$i] == 8) {
				continue;
			}
			$a = rand(0,1);
			$ttn -= $a; $tna[$i] += $a;
		}

		// Generate the planets' maximum populations for each tech level
		$mpa = array();
		$levels = 4;
		for ($tl=0;$tl<$levels;$tl++) {
			$minPPop = 8500 + $tl * 10000;
			$maxPPop = 11500 + $tl * 10000;
			$ttp = rand(8800,9200);
			$mpa[$tl] = array();

			for ($i=0;$i<$npl;$i++) {
				$mpa[$tl][$i] = $minPPop;
			}
			while ($ttp) {
				$i = ($i + 1) % $npl;
				if ($mpa[$tl][$i] >= $maxPPop) {
					continue;
				}
				$a = rand(1, min($maxPPop - $mpa[$tl][$i], $ttp));
				$ttp -= $a;
				$mpa[$tl][$i] += $a;
			}
		}

		// Create the planets
		for ($i=0;$i<$npl;$i++) {
			$qs = "INSERT INTO planet(system,orbit,name,turrets,max_pop) VALUES($system,$i,";
			do {
				$rn = strtoupper(substr(md5(uniqid(rand())), 0, 7));
				$q = $this->db->query("SELECT id FROM planet WHERE name='P-[$rn]'");
			} while(dbCount($q));
			$qs .= "'P-[$rn]',{$tna[$i]},{$mpa[0][$i]})";
			$this->db->query($qs);
		}

		// Compute the new planets' happiness and store their maximal populations
		$q = $this->db->query("SELECT id FROM planet WHERE system=$system");
		$np = 0;
		while ($r = dbFetchArray($q)) {
			for ($i=0;$i<$levels;$i++) {
				$this->db->query("INSERT INTO planet_max_pop(planet,tech_level,max_pop) VALUES ({$r[0]},$i,{$mpa[$i][$np]})");
			}
			$this->planets->call('updateHappiness', $r[0]);
			$np ++;
		}
	}


	private function genNebulaOrbit($system, $n) {
		$npl = 6;
		for ($i=0;$i<$npl;$i++) {
			$qs = "INSERT INTO planet(system,orbit,name,status,pop,max_pop,ifact,mfact,turrets,happiness) VALUES($system,$i,";
			do {
				$rn = strtoupper(substr(md5(uniqid(rand())), 0, 10));
				$q = $this->db->query("SELECT id FROM planet WHERE name='NB-[$rn]'");
			} while(dbCount($q));
			$md = min(4, max($n + rand(0,2) - 1, 1)) + 1;
			$qs .= "'NB-[$rn]',$md,0,0,0,0,0,0)";
			$this->db->query($qs);
		}
	}


	private function genSystem($i) {
		$q = $this->db->query("SELECT nebula FROM system WHERE id=$i");
		list($n) = dbFetchArray($q);
		if ($n == 0) {
			$this->genPlanets($i);
		} else {
			$this->genNebulaOrbit($i, $n);
		}
	}
}


?>

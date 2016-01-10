<?php

class beta5_moving_computeTrajectory {
	var $trajCache = null;

	function beta5_moving_computeTrajectory($lib) {
		$this->lib	= $lib;
		$this->db	= $this->lib->game->db;
		$this->planets	= $this->lib->game->getLib('beta5/planet');
	}

	// Get a fleet's trajectory (using the trajectory cache)
	function run($srcId, $dstId, $speed, $cruisers) {
		$s = "$srcId:$dstId:$speed:".($cruisers?1:0);
		if (is_null($this->trajCache[$s])) {
			$this->trajCache[$s] = $this->computeTrajectory($srcId, $dstId, $speed, $cruisers);
		}
		return $this->trajCache[$s];
	}

	// Computes a fleet's trajectory
	function computeTrajectory($srcId, $dstId, $speed, $cruisers) {
		$src = $this->planets->call('byId', $srcId);
		$dst = $this->planets->call('byId', $dstId);

		// Get planet list and base travel time
		$pList = array();
		if ($src['x'] == $dst['x'] && $src['y'] == $dst['y']) {
			// Planets are in the same system
			$minId = min($srcId,$dstId); $maxId = max($srcId,$dstId);
			logText("min/max id = $minId / $maxId");
			$q = $this->db->query("SELECT id,status,system FROM planet WHERE id>=$minId AND id<=$maxId ORDER BY id "
				. ($srcId==$minId ? 'ASC' : 'DESC'));
			$bTTime = 0;
			while ($r = dbFetchArray($q)) {
				array_push($pList, array($r[0], $r[1], $r[2]));
				$bTTime += ($r[0] == $minId || $r[0] == $maxId) ? 6 : 12;
			}
		} else {
			// Planets are in different systems
			if ($src['x'] != $dst['x']) {
				// Trajectory looks like y = ax + b
				$slope = ($dst['y'] - $src['y']) / ($dst['x'] - $src['x']);
				$displ = $src['y'] - $src['x'] * $slope;
				$vert = false;

				$minX = min($dst['x'],$src['x']); $maxX = max($dst['x'],$src['x']);
				$minY = min($dst['y'],$src['y']); $maxY = max($dst['y'],$src['y']);
				$qs = "SELECT id,x,y FROM system WHERE x>=$minX AND x<=$maxX AND y>=$minY AND y<=$maxY AND ";
				if ($slope > 0)
					$qs .= "y+0.5>$slope*(x-0.5)+$displ AND y-0.5<$slope*(x+0.5)+$displ";
				else
					$qs .= "y+0.5>$slope*(x+0.5)+$displ AND y-0.5<$slope*(x-0.5)+$displ";
				$qs .= " ORDER BY x " .($minX == $dst['x'] ? 'DESC' : 'ASC') . ", y " . ($src['y'] < $dst['y'] ? "ASC" : "DESC");
			} else {
				// Trajectory looks like x = c
				$vert = true;
				$minY = min($dst['y'],$src['y']); $maxY = max($dst['y'],$src['y']);
				$qs = "SELECT id,x,y FROM system WHERE y>=$minY AND y<=$maxY AND x=".$src['x'];
				$qs .= " ORDER BY y " .($minY == $dst['y'] ? 'DESC' : 'ASC');
			}

			$dX = $src['x'] - $dst['x']; $dY = $src['y'] - $dst['y'];
			$distance = sqrt($dX*$dX+$dY*$dY);
			$bTTime = round($distance * (70 - $speed * 10) * ($cruisers ? 2 : 1));

			// List orbits
			$q = $this->db->query($qs);
			while ($r = dbFetchArray($q)) {
				$dir = "DESC";

				// If we are in source or destination system, get all planets with orbits >= source/dest planet
				if ($r[1] == $src['x'] && $r[2] == $src['y']) {
					$dir = "ASC";
					$minOrbit = $src['orbit'];
				} elseif ($r[1] == $dst['x'] && $r[2] == $dst['y']) {
					$minOrbit = $dst['orbit'];
				} elseif ($vert) {
					// Vertical trajectories: 4 orbits out of 6 get intersected (1 / sqrt(2))
					$minOrbit = 2;
				} else {
					// If we are intersecting another system, 6 different cases:
					// (1) passing through x = xs - 0.5 and y = ys - 0.5
					// (2) passing through x = xs - 0.5 and y = ys + 0.5
					// (3) passing through x = xs - 0.5 and x = xs + 0.5
					// (4) passing through x = xs + 0.5 and y = ys - 0.5
					// (5) passing through x = xs + 0.5 and y = ys + 0.5
					// (6) passing through y = ys - 0.5 and y = ys + 0.5
					$y1 = ($r[1] - 0.5) * $slope + $displ;
					if ($y1 < $r[2] - 0.5 || $y1 > $r[2] + 0.5) {
						// Case (4), (5) or (6)
						$y1 = ($r[1] + 0.5) * $slope + $displ;
						if ($y1 < $r[2] - 0.5 || $y1 > $r[2] + 0.5) {
							// Case (6)
							$y1 = $r[2] - 0.5;
							$x1 = ($y1 - $displ) / $slope;
							$y2 = $r[2] + 0.5;
							$x2 = ($y2 - $displ) / $slope;
						} else {
							// Case (4) or (5)
							$x1 = $r[1] + 0.5;
							$x2 = ($r[2] - 0.5 - $displ) / $slope;
							if ($x2 < $r[1] - 0.5 || $x2 > $r[1] + 0.5) {
								// Case (5)
								$y2 = $r[2] + 0.5;
								$x2 = ($y2 - $displ) / $slope;
							} else {
								// Case (4)
								$y2 = $r[2] - 0.5;
							}
						}
					} else {
						// Case (1), (2) or (3)
						$x1 = $r[1] - 0.5;
						$y2 = ($r[1] + 0.5) * $slope + $displ;
						if ($y2 < $r[2] - 0.5 || $y2 > $r[2] + 0.5) {
							// Case (1) or (2)
							$x2 = ($r[2] - 0.5 - $displ) / $slope;
							if ($x2 < $r[1] - 0.5 || $x2 > $r[1] + 0.5) {
								// Case (2)
								$y2 = $r[2] + 0.5;
								$x2 = ($y2 - $displ) / $slope;
							} else {
								// Case (1)
								$y2 = $r[2] + 0.5;
							}
						} else {
							// Case (3)
							$x2 = $r[1] + 0.5;
						}
					}

					$dX = $x1 - $x2; $dY = $y1 - $y2; $d = sqrt($dX*$dX+$dY*$dY);
					$no = round(1 + 5 * $d / sqrt(2));
					$minOrbit = 6 - $no;
				}

				// List orbits for this system
				$q2 = $this->db->query("SELECT id,status FROM planet WHERE system=".$r[0]." AND orbit>=$minOrbit ORDER BY orbit $dir");
				while ($r2 = dbFetchArray($q2)) {
					array_push($pList, array($r2[0], $r2[1], $r[0]));
				}
			}
		}

		// Generate trajectory
		$trajectory = array(
			"hs"	=> ($src['x'] != $dst['x'] || $src['y'] != $dst['y']),
			"list"	=> array()
		);

		$opDelay = array(0,26,26,38,56,80);
		$tTime = $bTTime;
		$nPlanets = count($pList);
		$perPlanet = floor($bTTime / ($nPlanets - 1));
		$perEndpoint = ($bTTime - ($perPlanet * ($nPlanets - 2))) / 2;

		for ($i=0;$i<$nPlanets;$i++) {
			$otime = $opDelay[$pList[$i][1]];

			if ($i == 0) {
				$trtime = floor($perEndpoint);
				$otime /= 2;
			} elseif ($i == $nPlanets - 1) {
				$trtime = ceil($perEndpoint);
				$otime /= 2;
			} else {
				$trtime = $perPlanet;
			}

			$tTime += $otime;
			array_push($trajectory['list'], array(
				"pid"	=> $pList[$i][0],
				"time"	=> $trtime + $otime
			));
		}
		$trajectory['time'] = $tTime;

		return $trajectory;
	}
}

?>

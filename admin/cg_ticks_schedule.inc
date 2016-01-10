<?php

function __computeTicks($year, $month, $day, $speed, $shift) {

	$tickDefs = array(
		// Modifiable ticks
		"hour" => array(
			'now'		=> false,
			'change'	=> true,
			'interval'	=> 60 * 60,
			'desync'	=> 60 * 60
		),
		"battle" => array(
			'now'		=> false,
			'change'	=> true,
			'interval'	=> 4 * 60 * 60,
			'desync'	=> 55 * 60,
		),
		"cash" => array(
			'now'		=> false,
			'change'	=> true,
			'interval'	=> 12 * 60 * 60,
			'desync'	=> (12 * 60 + 5) * 60
		),
		"day" => array(
			'now'		=> false,
			'change'	=> true,
			'interval'	=> 24 * 60 * 60,
			'desync'	=> (24 * 60 + 10) * 60
		),
		"move" => array(
			'now'		=> false,
			'change'	=> true,
			'interval'	=> 60,
			'desync'	=> 60 * 60 + 27
		),

		// Immediate ticks
		'universe' => array(
			'now'		=> true,
			'change'	=> false,
			'interval'	=> 60 * 5,
			'desync'	=> 57
		),
		'punishment' => array(
			'now'		=> true,
			'change'	=> false,
			'interval'	=> 39,
			'desync'	=> 28
		),

		// Immutable ticks
		'quit' => array(
			'now'		=> false,
			'change'	=> false,
			'interval'	=> 50,
			'desync'	=> 7
		),
		'sales' => array(
			'now'		=> false,
			'change'	=> false,
			'interval'	=> 21,
			'desync'	=> 13
		),
	);

	$startTime = gmmktime(7, 0, 0, $month, $day, $year);
	$now = time();
	$results = array();
	foreach ($tickDefs as $tid => $def) {
		$interval = $def['change'] ? ceil($def['interval']/$speed) : $def['interval'];
		if ($def['now']) {
			$first = $startTime + $def['desync'];
			while ($first > $now) {
				$first -= $def['interval'];
			}
		} else {
			$first = $startTime + ($def['change'] ? ceil($def['desync']/$speed) : $def['desync']);
		}
		$results[$tid] = array($first + $shift * 60, $interval);
	}

	return $results;
}

?>

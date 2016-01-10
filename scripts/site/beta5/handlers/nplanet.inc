<?php

class	page_handler
{
	var	$needsAuth = true;

	function	checkName($n)
	{
		if	(strlen($n) > 15)
			return	1;
		if	(preg_match('/[^A-Za-z0-9_\.\-\+@\/'."'".' ]/', $n))
			return	2;
		if	(	preg_match('/^\s/', $n)
			||	preg_match('/\s$/', $n)
			)
			return	3;
		if	(preg_match('/\s\s+/', $n))
			return	4;
		if	(strlen($n) < 2)
			return	5;
		return	0;
	}

	function handle($input) {
		if ($this->game->params['norealloc'] == 1) {
			$this->output = 'norealloc';
			return;
		}

		$this->output = 'nplanet';
		$pl = gameAction('getPlayerPlanets', $_SESSION[game::sessName()]['player']);
		if	(count($pl))
		{
			$this->data = false;
			return;
		}

		if	($input['name'] == '')
		{
			$this->data = array('ok' => false, 'err' => 0, 'name' => '');
			return;
		}
		$pErr = $this->checkName($input['name']);
		if	(!$pErr)
		{
			$p = gameAction('getPlanetByName', $input['name']);
			if	(!is_null($p))
				$pErr = 6;
		}
		if	($pErr)
		{
			$this->data = array('ok' => false, 'err' => $pErr, 'name' => $input['name']);
			return;
		}

		$nplanet = gameAction('reassignPlanet', $_SESSION[game::sessName()]['player'], $input['name']);
		$this->data = array('ok' => true, 'name' => $input['name'], 'id' => $nplanet);
	}
}

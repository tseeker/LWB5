<?php

class	page_handler
{
	var	$needsAuth = true;
	var	$ajax = array(
			'init'	=> 'readInitString()',
			'func'	=> array('updateTicks')
		);

	function	updateTicks()
	{
		$tickLib = $this->game->getLib('main');
		$tinf = $tickLib->call('getTicks', getLanguage());
		$now = time();
		$dt = array($now);
		foreach	($ticks as $tid => $td)
			if	($td['game'])
				array_push($dt, "$tid#" . $td['last']);
		return	join('#', $dt);
	}

	function	handle($input)
	{
		$lang = getLanguage();
		$tickLib = $this->game->getLib('main');
		$this->data = $tickLib->call('getTicks', getLanguage());
		$this->output = "ticks";
	}
}

?>

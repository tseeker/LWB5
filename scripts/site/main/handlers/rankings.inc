<?php

class page_handler {
	var $noTracking = true;
	var $needsAuth = false;
	var $ajax = array();

	function handle($input) {
		$gTexts = $gList = array();
		foreach (config::getGames() as $id => $d) {
			if ($id == 'main' || $d->status() == 'PRE') {
				continue;
			}
			$gTexts[$id] = $d->text;
			array_push($gList, $id);
		}

		if ($input['t'] != '') {
			tracking::$data['rkGame'] = $input['g'];
		}
		if (is_null(tracking::$data['rkGame']) || !in_array(tracking::$data['rkGame'], $gList)) {
			tracking::$data['rkGame'] = $gList[0];
		}

		$game		= config::getGame(tracking::$data['rkGame']);
		$lib		= $game->getLib('main/rankings');
		$rkTypes	= array_values($lib->call('getTypes'));
		$lang		= getLanguage();
		$rkText		= array();

		foreach ($rkTypes as $id) {
			$rkText[$id] = $lib->call('getText', $id, $lang);
		}

		if ($input['t'] != '') {
			tracking::$data['rkType'] = $input['t'];
		}
		if (is_null(tracking::$data['rkType']) || !in_array(tracking::$data['rkType'], $rkTypes)) {
			tracking::$data['rkType'] = $rkTypes[0];
		}
		$cType = tracking::$data['rkType'];

		$this->output = "rankings";
		$this->data = array(
			"games"		=> $gTexts,
			"cGame"		=> tracking::$data['rkGame'],
			"types"		=> $rkText,
			"cType"		=> $cType,
			"rankings"	=> $lib->call('getAll', $cType)
		);
	}
}

?>

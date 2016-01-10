<?php

class page_handler {
	public $needsAuth = true;
	public $ajax = array(
		"func"	=> array("getEmpireData"),
		"init"	=> "makeEmpireTooltips();\nempire_write(document.getElementById('init-data').value);"
	);


	public function getEmpireData() {
		$data = $this->game->action('getEmpireOverview', $_SESSION[game::sessName()]['player']);
		if (is_null($data)) {
			return;
		}

		$s = "";
		foreach ($data['planets'] as $id => $n) {
			$s .= ($s == "" ? "" : "#") . "$id#$n";
		}

		$str = join('#', $data['planetStats'])
			. "\n$s\n{$data['fleetStats']['fleets']}#{$data['fleetStats']['battle']}#"
			. "{$data['fleetStats']['power']}#{$data['fleetStats']['upkeep']}#"
			. "{$data['fleetStats']['at_home']}#{$data['fleetStats']['home_battle']}#"
			. "{$data['fleetStats']['foreign']}#{$data['fleetStats']['foreign_battle']}#"
			. "{$data['fleetStats']['moving']}#{$data['fleetStats']['waiting']}#"
			. "{$data['fleetStats']['gaships']}#{$data['fleetStats']['fighters']}#"
			. "{$data['fleetStats']['cruisers']}#{$data['fleetStats']['bcruisers']}\n"
			. "{$data['techStats']['points']}#" . join('#', $data['techStats']['budget'])
			. "#{$data['techStats']['new']}#{$data['techStats']['foreseen']}\n"
			. "{$data['income']}#{$data['profit']}";

		return	$str;
	}

	function handle($input) {
		$this->data = $this->getEmpireData();
		$this->output = "empire";
	}
}

?>

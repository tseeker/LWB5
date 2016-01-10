<?php

class page_handler {
	public $needsAuth = true;
	public $ajax = array(
		'func'	=> array('getOverview', 'switchOvMode', 'breakProtection'),
		'init'	=> "makeOverviewTooltips();\ninitPage();"
	);

	public function getOverview() {
		$overview = $this->game->action('getOverview', $_SESSION[game::sessName()]['player'],
			getLanguage());
		$result = array($_SESSION[game::sessName()]['ov_complete'] ? "1" : "0");

		// Protection level
		array_push($result, $overview['protection']);

		// Communications overview
		$data = $overview['comms'];
		array_push($result, count($data['folders']['CUS']) . "#" . count($data['forums']['general'])
			. "#" . count($data['forums']['alliance']) . "#" . $data['forums']['allianceID']);

		// Messages in default folders
		$dFld = array('IN', 'INT', 'OUT');
		foreach ($dFld as $f) {
			array_push($result, join('#', $data['folders'][$f]));
		}

		// Custom folders
		foreach ($data['folders']['CUS'] as $id => $folder) {
			$folder[2] = utf8entities($folder[2]);
			array_unshift($folder, $id);
			array_push($result, join('#', $folder));
		}

		// Forums
		foreach ($data['forums']['general'] as $cat) {
			array_push($result, "{$cat['id']}#{$cat['type']}#" . count($cat['forums'])
				. "#" . utf8entities($cat['title']));

			foreach	($cat['forums'] as $f) {
				$f[3] = utf8entities($f[3]);
				array_push($result, join('#', $f));
			}
		}
		foreach ($data['forums']['alliance'] as $f) {
			$f[3] = utf8entities($f[3]);
			array_push($result, join('#', $f));
		}

		// Empire overview
		$data = $overview['empire'];
		array_push($result, join('#', $data['planetStats']));
		array_push($result, "{$data['fleetStats']['power']}#{$data['fleetStats']['fleets']}"
			. "#{$data['fleetStats']['battle']}");
		array_push($result, "{$data['income']}#{$data['fleetStats']['upkeep']}");
		array_push($result, $data['techStats']['new']);

		// Universe overview
		$data = $overview['universe'];
		array_push($result, $data['summary'][0] . "#" . $data['summary'][2] . "#" . $data['summary'][0]);
		array_push($result, join("#", $data['rankings']));
		array_push($result, time());
		foreach ($data['ticks'] as $tick) {
			array_push($result, join('#', $tick));
		}

		return join("\n", $result);
	}

	public function breakProtection() {
		$pLib = $this->game->getLib('beta5/player');
		$pLib->call('breakProtection', $_SESSION[game::sessName()]['player'], 'BRK');
		return $this->getOverview();
	}

	public function switchOvMode() {
		$_SESSION[game::sessName()]['ov_complete'] = ! $_SESSION[game::sessName()]['ov_complete'];
	}

	public function handle($input) {
		$this->data = $this->getOverview();;
		$this->output = "overview";
	}
}

?>

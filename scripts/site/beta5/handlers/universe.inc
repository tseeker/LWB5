<?php

class page_handler {
	public $needsAuth = true;
	public $ajax = array(
			'func'	=> array('getInformation'),
			'init'	=> 'initPage();'
		);

	public function getInformation() {
		$universe = $this->game->action('getUniverseOverview',
			$_SESSION[game::sessName()]['player'], getLanguage()
		);

		$ticks = array();
		foreach ($universe['ticks'] as $tick) {
			array_push($ticks, join('#', $tick));
		}

		$main = array(join('#', $universe['summary']), join('#', $universe['rankings']), time());

		return join("\n", array_merge($main, $ticks));
	}

	public function handle($input) {
		$this->data = $this->getInformation();
		$this->output = "universe";
	}
}

?>

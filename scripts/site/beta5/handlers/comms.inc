<?php

class page_handler {

	public $needsAuth = true;
	public $ajax = array(
			'func'	=> array('getCommsData'),
			'init'	=> "makeCommsTooltips();\ninitPage();"
		);

	public function getCommsData() {
		// Get the data
		$data = $this->game->action('getCommsOverview', $_SESSION[game::sessName()]['player']);

		// Issue first line: nCFolders#nGenCats#nAForums
		$result = array();
		array_push($result, count($data['folders']['CUS']) . "#" . count($data['forums']['general'])
			. "#" . count($data['forums']['alliance']));

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

		return	join("\n", $result);
	}

	public function handle($input) {
		$this->data = $this->getCommsData();
		$this->output = "comms";
	}
}

?>

<?php

class page_layout {

	public function __construct() {
		$this->dir = config::$main['scriptdir'] . "/site/main";
		$this->static = config::$main['staticdir'] . "/main";
	}

	private function _include($file, $args = array()) {
		foreach ($args as $k => $v) {
			$nk = "__$k";
			$$nk = $v;
		}
		include($file);
	}

	private function addStylesheet($name) {
		addFileResource('css', "{$this->static}/css/$name.css");
	}

	private function addScript($name) {
		addFileResource('js', "{$this->static}/js/$name.js");
	}

	private function startContents() {
		$this->addStylesheet("content");
		echo "<div id='cframe'>";
	}

	private function endContents() {
		echo "</div>";
	}

	public function header($pg, $lg) {
		if ($pg == "ppipn" || $pg == "pcheck") {
			return;
		}

		// Output the HTML header
		echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		$this->_include("{$this->dir}/layout/header.inc", array("lang" => $lg));
		$this->addStylesheet('main');
		$this->addScript('jquery');
		$this->addScript('adapt');
	}

	public function includeFile($file, $args = array()) {
		ob_start();
		$this->_include($file, is_array($args) ? $args : array($args));
		$this->pageContents = ob_get_contents();
		ob_end_clean();
	}

	public function footer($pg, $lg) {
		if ($pg == "ppipn" || $pg == "pcheck") {
			echo $this->pageContents;
			return;
		}

		$this->cssRes = storeResource("css", 345600);
		$this->jsRes = storeResource("js", 345600);

		$aLib = config::getGame('main')->getLib('main/account');
		list($tAcc, $oAcc) = $aLib->call('getAccounts');

		if ($_SESSION['authok']) {
			$vacation = config::getGame('main')->getLib('main/vacation')->call(
				'isOnVacation', $_SESSION['userid']);
		} else {
			$vacation = false;
		}
		$this->_include("{$this->dir}/layout/actual-header.$lg.inc", array(
			"accounts"	=> $tAcc,
			"online"	=> $oAcc,
			"vacation"	=> $vacation
		));

		echo $this->pageContents;
		$this->_include("{$this->dir}/layout/footer.inc");
	}

}

?>

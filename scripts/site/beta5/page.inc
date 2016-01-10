<?php

class page_layout {

	function page_layout() {
		$this->dir = config::$main['scriptdir'] . "/site/beta5";
		$this->static = config::$main['staticdir'] . "/beta5";
	}


	function header($pg, $lg) {
		// Output the HTML header
		echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 4.01 Transitional//EN\">\n"
			. "<html><head><title>Legacy Worlds " . input::$game->text . "</title>"
			. "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>";

		// Read the user preferences
		$tt = prefs::get('main/tooltips', 2);
		$fs = prefs::get('main/font_size', 2);
		$col = prefs::get('main/colour', 'purple');
		$thm = prefs::get('beta5/theme', "default");

		// Generate CSS and JS resources
		$this->generateStylesheets($pg, $thm, $fs, $col);
		if (is_null(handler::$h->customJS)) {
			$this->generateJS($pg, $thm, $col, $lg, $tt, $fs);
		} else {
			$this->jsRes = handler::$h->customJS;
		}

		if (!is_null($this->cssRes)) {
			echo "<link rel='stylesheet' type='text/css' href='".makeLink('css','main',"css")."?id={$this->cssRes}' />";
		}
		if (!is_null($this->jsRes)) {
			echo "<script language='JavaScript' type='text/javascript' charset='utf-8' src='".makeLink('js','main',"js")."?id={$this->jsRes}'></script>";
		}

		// Generate HTML body tag
		echo "</head><body";
		if (is_null(handler::$h->customJS)) {
			if (ajax::$init != "" || $tt) {
				echo " onload='lwAutoInit();'";
			}
			if ($tt) {
				echo " onunload='tt_Disable()'";
			}
			echo ">\n";
		} else {
			if (!is_null(handler::$h->customJS['load'])) {
				echo " onload='" . handler::$h->customJS['load'] . "'";
			}
			if (!is_null(handler::$h->customJS['unload'])) {
				echo " onunload='" . handler::$h->customJS['unload'] ."'";
			}
		}

		// Load the header
		if (is_null(handler::$h->customHeader)) {
			$hdr = "header";
		} else {
			$hdr = handler::$h->customHeader;
		}
		$this->includeFile("{$this->dir}/layout/$thm/$hdr.$lg.inc");
	}


	function footer($pg, $lg) {
		$tt = prefs::get('main/tooltips', 2);
		$col = prefs::get('main/colour', 'purple');
		$thm = prefs::get('beta5/theme', "default");

		if (is_null(handler::$h->customFooter)) {
			$ftr = "footer";
		} else {
			$ftr = handler::$h->customFooter;
		}
		$this->includeFile("{$this->dir}/layout/$thm/$ftr.$lg.inc");

		// Tooltips
		if ($tt && is_null(handler::$h->customJS)) {
			echo "<div id='ttPlaceHolderReserved'></div>";
			$f = getStatic("beta5/js/tt_{$thm}_$col.js");
			if (!is_null($f)) {
				echo "<script language='JavaScript' type='text/javascript' src='$f'></script>";
			}
			$f = getStatic("beta5/js/tooltips.js");
			if (!is_null($f)) {
				echo "<script language='JavaScript' type='text/javascript' src='$f'></script>";
			}
		}

		echo "</body></html>";
	}


	function generateStylesheets($page, $thm, $fs, $col) {
		$css = array("main");
		array_push($css, "thm_$thm");
		array_push($css, "pg_$page");
		array_push($css, "fonts$fs");
		array_push($css, "$col");
		foreach ($css as $fn) {
			$rf = "/css/$fn" . (input::$IE ? "-ie" : "");
			$r = addFileResource("css", "{$this->static}/$rf.css");
			if (input::$IE && !$r) {
				$r = addFileResource("css", "{$this->static}/css/$fn.css");
			}
		}

		$this->cssRes = storeResource("css", 345600);
	}


	function generateJS($pg, $thm, $col, $lg, $tt, $fs) {
		// JavaScript variables (static access URL, color, etc...)
		$jsConf = "var staticurl=\"".config::$main['staticurl']
				. "\";\nvar color=\"$col\";\nvar ttFontSize = '" . ($fs + 9) . "px';\n"
				. "var ttDelay = " . ($tt * 500) . ";\n";

		// AJAX-specified initialization code
		if (ajax::$init != "" || $tt) {
			$jsConf .= "function lwAutoInit()\n{\n";
			if (is_array(ajax::$fHandler)) {
				foreach (ajax::$fHandler as $f) {
					$jsConf .= "\tnew rpc_Function('$f','" . ajax::$method[$f] . "');\n";
				}
			}
			if (is_array(ajax::$fTheme)) {
				foreach (ajax::$fTheme as $f) {
					$jsConf .= "\tnew rpc_Function('$f','" . ajax::$method[$f] . "');\n";
				}
			}
			if (ajax::$init != "") {
				$jsConf .= "\t" . ajax::$init . "\n"; 
			}
			if ($tt) {
				$jsConf .= "\ttt_Init();\n";
			}
			$jsConf .= "}\n";
		}
		addRawResource("js", $jsConf);

		// Common JavaScript functions
		addFileResource("js", "{$this->static}/js/main.js");
		addFileResource("js", "{$this->static}/js/main-$lg.js");

		// AJAX script and initialisation
		if (count(ajax::$fHandler) || count(ajax::$fTheme)) {
			addRawResource("js", "var rpc_pageURI='" . makeLink($pg, input::$game->name, 'rpc') . "';\n");
			addFileResource("js", "{$this->static}/js/rpc.js");
			addFileResource("js", "{$this->static}/js/rpc-$lg.js");
		}

		// Theme-specific JavaScript
		addFileResource("js", "{$this->static}/js/thm_$thm.js");
		addFileResource("js", "{$this->static}/js/thm_$thm-$lg.js");

		// Page-specific JavaScript
		addFileResource("js", "{$this->static}/js/pg_$pg.js");
		addFileResource("js", "{$this->static}/js/pg_$pg-$lg.js");

		// Add JS resource
		$this->jsRes = storeResource("js", 345600);
	}


	function includeFile($file, $args = null) {
		include($file);
	}
}

?>

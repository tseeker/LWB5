<?

$game = config::getDefaultGame();
$man = $game->getLib('main/manual');

// We're in the manual, display manual controls
$manual = $man->call('getStructure', 'en');
$txt = array('Top', 'Up', 'Previous', 'Next');
if (!is_null(handler::$h->page)) {
	$pid = handler::$h->page['id'];
	$navLinks = $man->call('getNavLinks', $pid);
	for ($i=0;$i<4;$i++) {
		$t = is_null($navLinks[$i]) ? "" : "<a href='?p={$navLinks[$i]}'>";
		$t .= $txt[$i] . (is_null($navLinks[$i]) ? "" : "</a>");
		$navLinks[$i] = $t;
	}
} else {
	$pid = null;
	$navLinks = $txt;
}

ob_start();
?>
<div id="msidebox-top">
 <ul id="msb-nav">
  <li id="msb-top"><?= $navLinks[0] ?></li>
  <li id="msb-left"><?= $navLinks[2] ?></li>
  <li id="msb-up"><?= $navLinks[1] ?></li>
  <li id="msb-right"><?= $navLinks[3] ?></li>
 </ul>
 <form action="?" method="post" id="msb-form">
  <div>
   <input type="text" name="ss" value="<?= preg_replace('/"/', '&quot;', handler::$h->searchText) ?>" class="input" id="msb-stext" />
   <input type="submit" value="Search" class="input" id="msb-sbutton" />
  </div>
 </form>
 <span id="msb-title">Contents</span>
</div>
<div id="msb-links">
<?

function dumpManualStructure(&$sections, $depth) {
	$br = false;
	foreach ($sections as $id => $sd) {
		if ($br) {
			echo "<br/>";
		} else {
			$br = true;
		}
		if ($depth > 0) {
			echo str_repeat('&nbsp;', $depth * 2);
		}
		echo "       <a href='?p={$sd['name']}'>" . utf8entities($sd['title']) . "</a>";
		if (count($sd['subs'])) {
			echo "<br/>";
			dumpManualStructure($sd['subs'], $depth + 1);
		}
	}
}

	dumpManualStructure($manual, 0);

?>
</div>
<?
$sbContents = ob_get_contents();
ob_end_clean();

include("side-box.en.inc");

?>

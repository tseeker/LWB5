<?php


function drawContents (&$list) {
	$st = input::$IE ? "margin:0px 0px 0px 20px" : "margin:0px 0px 0px -15px";

	$kl = array_keys($list);
	foreach ($kl as $k) {
		echo "<li><a href='#{$list[$k]['name']}'>{$list[$k]['title']}</a>";
		if (count($list[$k]['subsections'])) {
			echo "<ul style='$st'>";
			drawContents($list[$k]['subsections']);
			echo "</ul>";
		}
		echo "</li>";
	}
}


function displayLinks($text) {
	$l = explode('<mlink ', $text);
	$nText = array_shift($l);
	while (count($l)) {
		$t = preg_replace('/^\s*to=[\'"]/', '', array_shift($l));
		$toName = preg_replace('/^([A-Za-z0-9_\-]+)[\'"]>(.*\n)*.*/', '\1', $t);
		$toName = preg_replace('/\s/', '', $toName);
		$t = preg_replace('/^[A-Za-z0-9_\-]+[\'"]>/', '', $t);

		$secId = handler::$h->lib->call('getSectionId', handler::$h->lang, $toName);
		$link = "";
		if (!is_null($secId)) {
			$pageId = handler::$h->lib->call('getPageId', $secId);
			if (!is_null($pageId)) {
				$lt = handler::$h->lib->call('readSectionRecord', $secId);
				if ($pageId == handler::$h->page['id']) {
					$link = "<a href='#{$lt['name']}'>";
				} else {
					$pg = handler::$h->lib->call('readSectionRecord', $pageId);
					$link = "<a href='?c=0&p={$pg['name']}#{$lt['name']}'>";
				}
			}
		}

		$nText .= $link . preg_replace('/^(.*)<\/mlink>(.*\n)*.*/', '\1', $t) . ($link != '' ? "</a>" : "")
			. preg_replace('/^.*<\/mlink>/', '', $t);
	}

	return	$nText;
}


function drawSecTitle(&$section, $depth) {
	$pgLink = "";
	if ($section['linkto'] != "") {
		$pageId = handler::$h->lib->call('getPageId', $section['linkto']);
		if (!is_null($pageId)) {
			$lt = handler::$h->lib->call('readSectionRecord', $section['linkto']);
			if ($pageId == handler::$h->page['id']) {
				$pgLink = " (<a href='#{$lt['name']}'>view</a>)";
			} else {
				$pg = handler::$h->lib->call('readSectionRecord', $pageId);
				$pgLink = " (<a href='?c=0&p={$pg['name']}#{$lt['name']}'>view</a>)";
			}
		}
	}

	$mDepth = ($depth - 2) * 10;
	echo "<table style='width:95%;margin:0;padding:0'><tr><td style='width:85%'><a name='{$section['name']}'></a><h$depth"
		. " style='padding:0;margin:5px 0px 5px {$mDepth}px'>{$section['title']}$pgLink</h$depth>"
		. "</td><td style='text-align:right'><a href='#" . handler::$h->page['name'] . "'>Top</a></td></tr></table>";
}

function drawSections (&$list, $depth = 2) {
	$kl = array_keys($list);
	foreach ($kl as $k) {
		if ($depth == 2) {
			echo "<hr/>";
		}
		drawSecTitle($list[$k], $depth);
		if ($list[$k]['contents'] != '') {
			echo "<div class='mansection'>" . displayLinks($list[$k]['contents']) . "</div>";
		}
		if (count($list[$k]['subsections'])) {
			drawSections($list[$k]['subsections'], $depth + 1);
		}
	}
}


if (is_null(handler::$h->page)) {
	include('manual_notfound.en.inc');
	return;
}

?>
   <a name="<?=handler::$h->page['name']?>"></a>
   <h1 style="margin:0;padding:0"><?=handler::$h->page['title']?></h1>
<?

if (count(handler::$h->page['subsections'])) {
	$st = input::$IE ? "margin:0px 0px 0px 30px" : "margin:0px 0px 0px -15px";
	echo "<div class='mansection'><b>Contents:</b><ul style='$st'>";
	drawContents(handler::$h->page['subsections']);
	echo "</ul></div>";
	drawSections(handler::$h->page['subsections']);
}

?>

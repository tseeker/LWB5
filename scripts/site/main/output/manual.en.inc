<?php


function drawContents (&$list) {
	$kl = array_keys($list);
	foreach ($kl as $k) {
		echo "<li><a href='#ac-{$list[$k]['name']}'>{$list[$k]['title']}</a>";
		if (count($list[$k]['subsections'])) {
			echo "<ul>";
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
					$link = "<a href='#ac-{$lt['name']}'>";
				} else {
					$pg = handler::$h->lib->call('readSectionRecord', $pageId);
					$link = "<a href='?c=0&amp;p={$pg['name']}#ac-{$lt['name']}'>";
				}
			}
		}

		$nText .= $link . preg_replace('/^(.*)<\/mlink>(.*\n)*.*/', '\1', $t) . ($link != '' ? "</a>" : "")
			. preg_replace('/^.*<\/mlink>/', '', $t);
	}

	return	$nText;
}


function drawTitle(&$section, $depth) {
	$pgLink = "";
	if ($section['linkto'] != "") {
		$pageId = handler::$h->lib->call('getPageId', $section['linkto']);
		if (!is_null($pageId)) {
			$lt = handler::$h->lib->call('readSectionRecord', $section['linkto']);
			if ($pageId == handler::$h->page['id']) {
				$pgLink = "<a class='mlink' href='#ac-{$lt['name']}'>";
			} else {
				$pg = handler::$h->lib->call('readSectionRecord', $pageId);
				$pgLink = "<a class='mlink' href='?c=0&amp;p={$pg['name']}#ac-{$lt['name']}'>";
			}
		}
	}

	$mDepth = ($depth - 2) * 10;
	$tMargin = ($pgLink == "") ? "5px $mDepth 15px 0px" : "5px $mDepth 5px 0px";

?>
<? if ($depth == 2) : ?>
<a class="toplnk" href='#ac-<?= handler::$h->page['name'] ?>'>Top</a>
<? endif; ?>
<h<?= $depth ?> id='ac-<?= $section['name'] ?>'><?= $section['title'] ?></h<?= $depth ?>>
<? if ($pgLink != "") : ?>
<?= $pgLink ?>-&gt; Main article</a>
<? endif;

}

function drawSections (&$list, $depth = 2) {
	$kl = array_keys($list);
	foreach ($kl as $k) {
/*
		if ($depth == 2) {
			echo "<hr style='margin: 30px 0px 0px 0px'/>";
		}
*/
?>
<? if ($depth == 2) : ?>
<div class="mtopsec">
<? else : ?>
<div class="msec">
<? endif; ?>
<? drawTitle($list[$k], $depth);
		if (trim($list[$k]['contents']) != '') {
			echo "<div class='mancontents'>" . displayLinks($list[$k]['contents']) . "</div>";
		}
		if (count($list[$k]['subsections'])) {
			drawSections($list[$k]['subsections'], $depth + 1);
		}
?>
</div>
<?
	}
}


include("manual-box.en.inc");
$this->title = "Manual";
$this->addStylesheet("manual");
$this->addScript("manual");
if (is_null(handler::$h->page)) {
	include('manual_notfound.en.inc');
} else {
?>
<div id="manpage">
 <h1 id="ac-<?=handler::$h->page['name']?>"><?=handler::$h->page['title']?></h1>
<? if (count(handler::$h->page['subsections'])) : ?>
 <div id='clist'>
  <div id='chead'>
   <b>Page contents</b>
   [ <span id='hidecontents'>Hide</span><span id='showcontents'>Show</span> ]
  </div>
  <div id='pcontents'>
   <ul>
<? drawContents(handler::$h->page['subsections']); ?>
   </ul>
  </div>
 </div>
<? drawSections(handler::$h->page['subsections']); ?>
<? endif; ?>
</div>
<?
}
$this->endContents();
?>

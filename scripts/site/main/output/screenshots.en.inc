<?php

include("menu.en.inc");
$this->title = "Screenshots";
$this->addStylesheet("screenshots");
$this->startContents();

if (is_null($__category)) {
?>
<div class="category" style="top: 0px">
 <div class="catname">Screenshots</div>
 <div class="catpics">Select a category</div>
</div>
<?php
	$y = 120;
	foreach ($__list as $id => $data) {
?>
<div class="category" style="top:<?= $y ?>px">
 <div class="catname">
  <a href="<?= makeLink('screenshots', 'main') ?>?c=<?= $id ?>"><?= utf8entities($data['title']) ?></a>
 </div>
 <div class="catpics">
  <?= count($data['pics']) ?> pictures
 </div>
</div>
<?
		$y += 75;
	}
} elseif (is_null($__picture)) {
	$category = $__list[$__category];
?>
<div class="category" style="top: 0px">
 <div class="catname">Screenshots - <?= utf8entities($category['title']) ?></div>
 <div class="catpics"><a href="<?= makeLink('screenshots', 'main') ?>">Back</a></div>
</div>
<?
	$y = 70;
	$x = 0;
	foreach ($category['pics'] as $id => $title) {
?>
<div class="thumbnail" style="top: <?= $y ?>px; left: <?= $x * 33 ?>%">
 <div class="thumbpic"><a href="<?= makeLink('screenshots', 'main') ?>?c=<?= $__category ?>&amp;s=<?= $id ?>"><img src="<?= config::$main['staticurl'] ?>/main/screens/<?= $__category ?>-<?= $id ?>-s.jpg" alt="<?= utf8entities($title) ?>" /></a></div>
 <div class="thumbname"><a href="<?= makeLink('screenshots', 'main') ?>?c=<?= $__category ?>&amp;s=<?= $id ?>"><?= utf8entities($title) ?></a></div>
</div>
<?
		$x = ($x + 1) % 3;
		if ($x == 0) {
			$y += 120;
		}
	}
} else {
	$category = $__list[$__category];
	$title = $category['pics'][$__picture];
?>
<div class="category" style="top: 0px">
 <div class="catname"><?= utf8entities($category['title'] . ' - ' . $title) ?></div>
 <div class="catpics"><a href="<?= makeLink('screenshots', 'main') ?>?c=<?= $__category ?>">Back</a></div>
</div>
<div class="showpic">
 <a href="<?= config::$main['staticurl'] ?>/main/screens/<?= $__category ?>-<?= $__picture ?>.jpg"><img src="<?= config::$main['staticurl'] ?>/main/screens/<?= $__category ?>-<?= $__picture ?>-m.jpg" alt="<?= utf8entities($title) ?>" /></a>
</div>
<?
}

$this->endContents();

?>

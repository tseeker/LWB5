<?php

echo "<h2>Broken links report</h2>";
if (!count($data)) {
	echo "<p>No broken links have been reported.</p>";
	return;
}

echo "<ul>";
foreach ($data as $link) {
	echo "<li>Link <b>" . utf8entities($link['title']) . "</b> to <a href='{$link['url']}' target='_blank'>"
		. utf8entities($link['url']) . "</a> in category " . utf8entities($link['category']['title']) . "<br/>"
		. "Reported by: <b>" . join('</b> - <b>', $link['reporters']) . "</b><br/>"
		. "<a href='?c=lkr&id={$link['id']}&dr=1'>Remove report</a> - "
		. "<a onclick='return confirm(\"Please confirm that you want to remove this link\")' href='?c=lkr&id={$link['id']}&dl=1'>Remove link</a><br/>&nbsp;</li>";
}
echo "</ul>";

?>

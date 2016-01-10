<?php

echo "<h2>Links and categories</h2>";
if (!count($data)) {
	echo "<p>There are no categories. Click <a href='?c=lk&ac=0'>here</a> to add one.</p>";
	return;
}

$hasAccount = (is_array($_SESSION) && !is_null($_SESSION['userid']));
for ($i=0;$i<count($data);$i++) {
	echo "<p style='margin:10px 0px 5px 20px;text-align:justify'><b><u>" . utf8entities($data[$i]['title']) . "</u></b>";
	if (!is_null($data[$i]['description'])) {
		echo "<br/>" . preg_replace('/\n/', '<br/>', utf8entities($data[$i]['description']));
	}
	echo "<br/><a href='?c=lk&ac=1&cid={$data[$i]['id']}'>Add link</a> - <a href='?c=lk&ac=2&cid={$data[$i]['id']}'>Edit</a>";
	echo " - <a onclick='return confirm(\"Are you sure you want to delete this category?\")' href='?c=lk&ac=3&cid={$data[$i]['id']}'>Delete</a>";
	if ($i > 0) {
		echo " - <a href='?c=lk&ac=4&cid={$data[$i]['id']}'>Move up</a>";
	}
	if ($i < count($data) - 1) {
		echo " - <a href='?c=lk&ac=5&cid={$data[$i]['id']}'>Move down</a>";
	}
	echo "</p>";

	echo "<ul style='margin:0px 30px 0px 60px'>";
	if (!count($data[$i]['links'])) {
		echo "<li><i>No links in this category.</i></li>";
	} else {
		foreach ($data[$i]['links'] as $l) {
			echo "<li><a href='{$l['url']}' target='_blank' title=\"" . utf8entities($l['title']) . "\"><u>" . utf8entities($l['title']) . "</u></a>";
			if (!is_null($l['description'])) {
				echo "<br/>" . preg_replace('/\n/', '<br/>', utf8entities($l['description']));
			}
			echo "<br/><a href='?c=lk&ac=10&lid={$l['id']}'>Edit</a> - <a onclick='return confirm(\"Are you sure you want to delete this link?\")' href='?c=lk&ac=11&lid={$l['id']}'>Delete</a>";
			echo "<br/>&nbsp;</li>";
		}
	}
	echo "</ul>";
}
echo "<p style='margin:10px 0px 5px 20px;text-align:justify'><a href='?c=lk&ac=0'>Add category</a></p>";

?>

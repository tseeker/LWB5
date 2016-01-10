<?php

echo "<h2>Submitted links</h2>";
if (!count($data)) {
	echo "<p>No links have been submitted.</p>";
	return;
}

echo "<ul>";
foreach ($data as $link => $entries) {
	echo "<li>Link to <a target='_blank' href='$link'>" . utf8entities($link) . "</a>:<ul>";
	foreach ($entries as $entry) {
		echo "<li>Submitted by <b>{$entry['submitter']}</b> as <b>{$entry['title']}</b><br/>";
		if ($entry['description'] != '') {
			echo "Description: <b>{$entry['description']}</b>";
		} else {
			echo "No description";
		}
		echo "<br/><a href='?c=lks&su=" . urlencode($link) . "&sid={$entry['sid']}'>Accept entry</a></li>";
	}
	echo "<li><a onclick='return confirm(\"Are you sure?\")' href='?c=lks&su=" . urlencode($link) . "'>Delete entry</a></ul></li>";
}
echo "</ul>";

?>

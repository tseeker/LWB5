   <h1>Search results for '<i><?=utf8entities($args['text'])?></i>'</h1>
<?php
if (empty($args['results'])) {
	echo "<p>Sorry, no results were found.</p>\n";
} else {
	$c = count($args['results']);
	echo "<p><b>$c</b> result" . ($c > 1 ? "s were" : " was") . " found:</p>";
	echo "<div class='mansection'><ol>";
	$error = false;
	foreach ($args['results'] as $p) {
		$pData = handler::$h->lib->call('readSectionRecord', $p);
		if (is_null($pData)) {
			$error = true;
			continue;
		}
		echo "<li><a href='?p={$pData['name']}'>{$pData['title']}</a></li>";
	}
	echo "</ol>";
	if ($error) {
		echo "<b>Some results could not be displayed because of a database error.</b>";
	}
	echo "</div>";
}
?>

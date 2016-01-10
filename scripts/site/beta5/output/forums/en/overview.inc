<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu('o', $args); ?>
  </td>
  <td>
   <h1>Forums Overview</h1>
<?

foreach	($args as $cid => $cat)
{
	echo "<table cellspacing='0' cellpadding='0' class='fcat'>\n";
	echo "<tr class='hdr'><th class='hdr'><a href='?cmd=".urlencode("C#$cid")."'" . tooltip('Click here to go to the main page of this forum category') . ">";
	echo utf8entities($cat['title']) . "</a></th><td class='ctype'>(" . ($cat['type'] == "A" ? "alliance" : "general") . ")</td></tr>\n";
	echo "<tr><td colspan='2'>";
	if	($cat['desc'] != "")
		echo "<p>" . preg_replace('/\n/', '<br/>', utf8entities($cat['desc'])) . "</p>\n";
	else
		echo "&nbsp;";
	echo "</td></tr>\n";

	if	(count($cat['forums']))
	{
		echo "<tr><td colspan='2'><table cellspacing='0' cellpadding='0' class='fcf'>\n";
		echo "<tr><td class='fst'>&nbsp;</td><th class='fnm'>Forum name</th>";
		echo "<th class='fnt'>Topics</th><th class='fnp'>Posts</th><th class='flp'>Last post</th></tr>\n";
		for	($i=0;$i<count($cat['forums']);$i++)
		{
			$pic  = config::$main['staticurl'] . "/beta5/pics/" . ($cat['forums'][$i]['unread'] > 0 ? 'un' : '') . 'read.gif';
			echo "<tr><td class='fst' rowspan='2'><img src='$pic' alt='FIXME' /></td>";
			echo "<td class='fnm'><a href='?cmd=".urlencode("F#".$cat['type']."#".$cat['forums'][$i]['id'])."'" . tooltip('Click here to go to the main page of this forum') . ">";
			echo utf8entities($cat['forums'][$i]['title']) . "</a></td>";
			echo "<td class='fnt' rowspan='2'>" . $cat['forums'][$i]['topics'] . "</td>";
			echo "<td class='fnp' rowspan='2'>" . $cat['forums'][$i]['posts'] . "</td>";
			echo "<td class='flp' rowspan='2'>&nbsp;";
			$l = $cat['forums'][$i]['last'];
			if	(is_null($l))
				echo "No Posts";
			else
			{
				echo "Last post by <b>" . utf8entities($l['author']) . "</b><br/>";
				echo preg_replace('/ /', '&nbsp;', gmstrftime("%H:%M:%S on %d/%m/%Y", $l['moment']));
			}
			echo "</td>";
			echo "</tr>\n<tr><td><p>";
			if	($cat['forums'][$i]['description'] != '')
				echo preg_replace('/\n/', '<br/>', utf8entities($cat['forums'][$i]['description']));
			else
				echo "&nbsp;";
			echo "</p></td></tr>\n";
		}
		echo "</table></td></tr>";
	}

	echo "</table>\n";
}

?>
  </td>
 </tr>
</table>

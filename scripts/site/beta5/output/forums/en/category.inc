<?

$cat = $args['all'][$args['id']];

?>
<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu('C#'.$args['id'], $args['all']); ?>
  </td>
  <td>
   <table cellspacing='0' cellpadding='0'><tr>
    <td class="pc50"><h1><?=utf8entities($cat['title'])?></h1></td>
    <td class='maar'><a href="?cmd=<?=urlencode('MC#'.$cat['type']."#".$cat['id'])?>" <?=tooltip('Click here to mark all topics in the forums belonging to this category as read')?>>Mark forums as read</a></td>
   </tr></table>
   <p><?

if	($cat['desc'] != '')
	echo preg_replace('/\n/', '<br/>', utf8entities($cat['desc']));
else if	($cat['type'] == 'A')
	echo "<p>These are the forums of the alliance called <b>".utf8entities($cat['title'])."</b></p>";
else
	echo "&nbsp;";

echo "</p>\n";

if	(count($cat['forums']))
{
	echo "<table cellspacing='0' cellpadding='0' class='fcf'>\n";
	echo "<tr><td class='fst' colspan='2'>&nbsp;</td><th class='fnm'>Forum name</th>";
	echo "<th class='fnt'>Topics</th><th class='fnp'>Posts</th><th class='flpl'>Last post</th></tr>\n";
	for	($i=0;$i<count($cat['forums']);$i++)
	{
		echo "<tr><td class='fst' rowspan='2'>&nbsp;</td>";
		$pic  = config::$main['staticurl'] . "/beta5/pics/" . ($cat['forums'][$i]['unread'] > 0 ? 'un' : '') . 'read.gif';
		echo "<td class='fst' rowspan='2'><img src='$pic' alt='FIXME' /></td>";
		echo "<td class='fnm'><a href='?cmd=".urlencode("F#".$cat['type']."#".$cat['forums'][$i]['id'])."' ".tooltip('Click here to go to this forum topics list page')." >";
		echo utf8entities($cat['forums'][$i]['title']) . "</a></td>";
		echo "<td class='fnt'>" . $cat['forums'][$i]['topics'] . "</td>";
		echo "<td class='fnp'>" . $cat['forums'][$i]['posts'] . "</td>";
		echo "<td class='flpl' rowspan='2'>&nbsp;";
		$l = $cat['forums'][$i]['last'];
		if	(is_null($l))
			echo "No Posts";
		else
		{
			echo "Last post by <b>" . utf8entities($l['author']) . "</b><br/>";
			echo preg_replace('/ /', '&nbsp;', gmstrftime("%H:%M:%S on %d/%m/%Y", $l['moment']));
		}
		echo "</td>";
		echo "</tr>\n<tr><td colspan='3'><p>";
		if	($cat['forums'][$i]['description'] != '')
			echo preg_replace('/\n/', '<br/>', utf8entities($cat['forums'][$i]['description']));
		else
			echo "&nbsp;";
		echo "</p></td></tr>\n";
	}
	echo "</table>";
}
else
	echo "<p>There are no forums in this category.</p>";

?>
  </td>
 </tr>
</table>


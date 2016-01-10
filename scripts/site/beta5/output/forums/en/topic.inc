<?
$f = $args['forum'];
$t = $args['topic'];
$fid = 'F#'.$f['ctype']."#".$f['id'];
list($mPPage,$page,$nPages,$threaded,$reverse) = $args['details'];
?>
<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu($fid, $args['cats']); ?>
  </td>
  <td>
   <h1><?=utf8entities($t['title'])?></h1>
   <form method='post' action='?'>
    <input type='hidden' name='cmd' value='T#<?=$f['ctype'].'#'.$t['id']?>' />
    <table cellspacing="0" cellpadding="0" class="fcmd">
     <tr>
      <td>Display <span <?=tooltip('Use this drop down list to select the number of posts to display on each page')?>><select name='mpp' onChange="submit();"><?php

	for	($i=1;$i<6;$i++)
		echo "<option" . ($i*10==$mPPage?" selected='selected'":"") . ">" . ($i*10) . "</option>";

?></select></span> posts per page</td>
      <td><?php

if	($nPages > 1)
{
	echo "Page <span" .tooltip('Use this drop down list to select the page of posts to display') . "><select name='pg' onChange='submit();'>";
	for	($i=0;$i<$nPages;$i++)
		echo "<option" . ($i==$page?" selected='selected'":"") . ">".($i+1)."</option>";
	echo "</select></span> / " . $nPages;
}
else
	echo "Page 1 / 1";

?></td>
     </tr>
    </table>

<?

$p = $args['posts'];
for	($i=0;$i<count($p);$i++)
{
	$d = min($p[$i]['depth'], 20);
	if	($d)
		echo "<table cellspacing='0' cellpadding='0'><tr><td class='ftd$d'>&nbsp;</td><td>\n";

	echo "<table cellspacing='0' cellpadding='0' class='ftp'>";
	echo "<tr><th>" . utf8entities($p[$i]['title']) . "</th>";
	echo "<td rowspan='2' class='cmd'>";
	echo "<a href='?cmd=".urlencode("R#".$f['ctype']."#".$p[$i]['id'])."&q=0'".tooltip('Click here to go to the compose form and reply to this message'). " >Reply</a> - ";
	echo "<a href='?cmd=".urlencode("R#".$f['ctype']."#".$p[$i]['id'])."&q=1'".tooltip('Click here to go to the compose form and reply to this message by quoting it'). " >Quote</a>\n";
	if	($f['mod'] || $p[$i]['mine'])
	{
		echo "<br/>";
		echo "<a href='?cmd=".urlencode("E#".$f['ctype']."#".$p[$i]['id'])."'".tooltip('Click here to go to the compose form and make changes to this post'). " >Edit</a>";
		if	($p[$i]['id'] != $t['fpid'] || $f['mod'])
		{
			echo " - <a href='?cmd=".urlencode("D#".$f['ctype']."#".$p[$i]['id'])."'".tooltip('Click here to delete this post'). " onClick='return confirm";
			echo ($p[$i]['id'] != $t['fpid']) ? 'DPost' : 'DTopic';
			echo "();'>Delete</a>\n";
		}
	}
	echo "</td></tr>";

	echo "<tr><td>Posted <b>" . gmstrftime("%H:%M:%S on %d/%m/%Y", $p[$i]['moment']);
	echo "</b> by ".(is_null($p[$i]['pid'])?'<b>':('<a href="message?a=c&ct=0&id='.$p[$i]['pid'].'"'.tooltip('Click here to go to the compose form and send a private message to the author of this post'). ' >'));
	echo $p[$i]['author'].(is_null($p[$i]['pid'])?'</b>':'</a>')."</td>";
	echo "</tr>\n";

	echo "<tr><td colspan='2'>" . $p[$i]["contents"] . "</td></tr>\n";

	if	($p[$i]['edited'] != 0)
	{
		echo "<tr><td class='edit' colspan='2'>Edited by ";
		echo (is_null($p[$i]['edit_id']) ? '<b>' : ('<a href="message?a=c&ct=0&id='.$p[$i]['edit_id'].'" '.tooltip('Click here to go to the compose form and send a private message to the last person who edited this post'). '  >')) . utf8entities($p[$i]['edited_by']);
		echo (is_null($p[$i]['edit_id']) ? '</b>' : '</a>'). " at " . gmstrftime("%H:%M:%S on %d/%m/%Y", $p[$i]['edited']) . "</td></tr>";
	}

	echo "</table>\n";
	if	($d)
		echo "</td></tr></table>\n";
}

?>
    <table cellspacing="0" cellpadding="0" class="fcmd">
     <tr>
      <td>Display mode: <span <?=tooltip('Use this drop down list to choose the display mode (threaded or linear)')?>  ><select name='thr' onChange="submit();">
       <option value='0'>Linear</option>
       <option value='1'<?=$threaded?" selected='selected'":""?>>Threaded</option>
      </select></span></td>
      <td>Messages order:<span <?=tooltip('Use this drop down list to choose the messages order (newest or oldest first)')?>  > <select name='ord' onChange="submit();">
       <option value='0'>Oldest first</option>
       <option value='1'<?=$reverse?" selected='selected'":""?>>Newest first</option>
      </select></span></td>
     </tr>
    </table>
   </form>
  </td>
 </tr>
</table>

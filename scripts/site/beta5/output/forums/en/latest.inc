<?
if	($args['cat'] == '')
{
	$mode = 'l';
	$title = 'Latest Messages in All Forums';
}
else
{
	$mode = 'L#' . $args['cat'];
	$title = $args['cats'][$args['cat']]['title'] . ": Latest Messages";
}
list($mPPage,$page) = $args['details'];
?>
<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu($mode, $args['cats']); ?>
  </td>
  <td>
   <h1><?=utf8entities($title)?></h1>
   <form method='post' action='?'>
    <input type='hidden' name='cmd' value='<?=$mode?>' />
    <input type='hidden' name='pg' value='<?=$page?>' />
    <table cellspacing="0" cellpadding="0" class="fcmd">
     <tr>
      <td><?=($page > 0) ? ("<a href='?cmd=".urlencode($mode)."&pg=".($page - 1)."' ".tooltip('Click here to go to the previous page of the latest messages in this forums category').  " >") : ""?>&lt;- Previous page<?=($page > 0) ? "</a>" : ""?></td>
      <td>Display <span <?=tooltip('Use this drop down list to choose the number of messages to diplay on each page')?>  ><select name='mpp' onChange="submit();"><?php

	for	($i=1;$i<6;$i++)
		echo "<option" . ($i*10==$mPPage?" selected='selected'":"") . ">" . ($i*10) . "</option>";

?></select></span> posts per page</td>
      <td><?=(count($args['posts'])==$mPPage+1)?("<a href='?cmd=".urlencode($mode)."&pg=".($page+1)."' ".tooltip('Click here to go to the next page of the latest messages in this forums category'). " >"):""?>Next page -&gt;<?=(count($args['posts'])==$mPPage+1)?"</a>":""?></td>
     </tr>
    </table>
   </form>

<?

$p = $args['posts'];
$nbp = min($mPPage,count($p));
if	($nbp == 0)
	echo "<center><b>No posts were found.</b></center>";
else
{
	for	($i=0;$i<$nbp;$i++)
	{
		echo "<table cellspacing='0' cellpadding='0' class='ftp'>";
		echo "<tr><th class='pfor'>";
		if	($args['cat'] == '')
			echo utf8entities($p[$i]['cname'] . " > ");
		echo utf8entities($p[$i]['fname']) . "</th>";
		echo "<td class='cmd' rowspan='3'><a href='?cmd=".urlencode("F#".$p[$i]['rctype']."#".$p[$i]['fid'])."'".tooltip('Click here to go to the main page of the forum in which this message was posted'). " >View forum</a><br/>";
		echo "<a href='?cmd=".urlencode("T#".$p[$i]['rctype']."#".$p[$i]['tid'])."'".tooltip('Click here to go to go to the page of the topic in which this message was posed'). " >View topic</a><br/>";
		echo "<a href='?cmd=".urlencode("R#".$p[$i]['rctype']."#".$p[$i]['id'])."&q=0'".tooltip('Click here to go to the compose form and reply to this message'). " >Reply</a> - ";
		echo "<a href='?cmd=".urlencode("R#".$p[$i]['rctype']."#".$p[$i]['id'])."&q=1'".tooltip('Click here to go to the compose form and reply to this message by quoting it'). " >Quote</a>\n";
		echo "</td></tr>\n";
		echo "<tr><th>" . utf8entities($p[$i]['title']) . "</th></tr>\n";

		echo "<tr><td>Posted <b>" . gmstrftime("%H:%M:%S on %d/%m/%Y", $p[$i]['moment']);
		echo "</b> by ".(is_null($p[$i]['pid'])?'<b>':('<a href="message?a=c&ct=0&id='.$p[$i]['pid'].'"'.tooltip('Click here to go to the compose form and send a private message to the author of this post'). ' >'));
		echo $p[$i]['author'].(is_null($p[$i]['pid'])?'</b>':'</a>')."</td>";
		echo "</tr>\n";

		echo "<tr><td colspan='2'>" . $p[$i]["contents"] . "</td></tr>\n";

		if	($p[$i]['edited'] != 0)
		{
			echo "<tr><td class='edit' colspan='2'>Edited by ";
			echo (is_null($p[$i]['edit_id']) ? '<b>' : ('<a href="message?a=c&ct=0&id='.$p[$i]['edit_id'].'"'.tooltip('Click here to go to the compose form and send a private message to the player who last edited this post'). ' >')) . utf8entities($p[$i]['edited_by']);
			echo (is_null($p[$i]['edit_id']) ? '</b>' : '</a>'). " at " . gmstrftime("%H:%M:%S on %d/%m/%Y", $p[$i]['edited']) . "</td></tr>";
		}

		echo "</table>\n";
	}
?>
   <form method='post' action='?'>
    <input type='hidden' name='cmd' value='<?=$mode?>' />
    <input type='hidden' name='pg' value='<?=$page?>' />
    <table cellspacing="0" cellpadding="0" class="fcmd">
     <tr>
      <td><?=($page > 0) ? ("<a href='?cmd=".urlencode($mode)."&pg=".($page - 1)."'".tooltip('Click here to go to the previous page of the latest messages in this forums category').  ">") : ""?>&lt;- Previous page<?=($page > 0) ? "</a>" : ""?></td>
      <td>Display <span <?=tooltip('Use this drop down list to choose the number of messages to diplay on each page')?>  ><select name='mpp' onChange="submit();"><?php

	for	($i=1;$i<6;$i++)
		echo "<option" . ($i*10==$mPPage?" selected='selected'":"") . ">" . ($i*10) . "</option>";

?></select></span> posts per page</td>
      <td><?=(count($args['posts'])==$mPPage+1)?("<a href='?cmd=".urlencode($mode)."&pg=".($page+1)."'".tooltip('Click here to go to the next page of the latest messages in this forums category'). " >"):""?>Next page -&gt;<?=(count($args['posts'])==$mPPage+1)?"</a>":""?></td>
     </tr>
    </table>
   </form>
<?
}
?>
  </td>
 </tr>
</table>

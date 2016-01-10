<?
$mPPage = $args['sparm']['perpage'];
$page = $args['sparm']['page'];
?>
<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu('s', $args['cats']); ?>
  </td>
  <td>
   <h1>Search results</h1>
   <form method='post' action='?'>
    <input type='hidden' name='cmd' value='s' />
    <input type='hidden' name='pg' value='<?=$page?>' />
    <table cellspacing="0" cellpadding="0" class="fcmd">
     <tr>
      <td><?=($page > 0) ? ("<a href='?cmd=s&pg=".($page - 1)."' " .tooltip('Click here to go to the previous page of retrieved topics') . " >") : ""?>&lt;- Previous page<?=($page > 0) ? "</a>" : ""?></td>
      <td>Display<span <?=tooltip('Use this drop down list to select the number of topics to display on each page')?>> <select name='mpp' onChange="submit();"><?php

	for	($i=1;$i<6;$i++)
		echo "<option" . ($i*10==$mPPage?" selected='selected'":"") . ">" . ($i*10) . "</option>";

?></select></span> topics per page</td>
      <td><?=(count($args['topics'])==$mPPage+1)?("<a href='?cmd=s&pg=".($page+1)."' " .tooltip('Clickhere to go to the next page of retrieved topics') . " >"):""?>Next page -&gt;<?=(count($args['topics'])==$mPPage+1)?"</a>":""?></td>
     </tr>
    </table>
   </form>

<?

$p = $args['topics'];
$nbp = min($mPPage,count($p));
if	($nbp == 0)
	echo "<center><b>No posts were found.</b></center>";
else
{
	echo '<table cellspacing="0" cellpadding="0" class="ftop">';
	echo "<tr><th class='tpic'>&nbsp;</th>";
	echo "<th class='tnm'>Topic</th><th class='trp'>Replies</th>";
	echo "<th class='tps'>First Post</th><th class='tps'>Last Post</th></tr>\n";
	for	($i=0;$i<count($p);$i++)
	{
		echo "<tr>";
		$pic  = config::$main['staticurl'] . "/beta5/pics/" . ($p[$i]['read'] ? '' : 'un') . 'read';
		if ($p[$i]['sticky']) {
			$pic .= '_sticky';
		}
		$pic .= '.gif';
		$text = ($p[$i]['read'] ? 'Read' : 'Unread') . ($p[$i]['sticky'] ? " sticky" : "") . " topic";
		$id = $p[$i]['rctype'] . "#" . $p[$i]['id'];
		echo "<td class='tpic'><img src='$pic' alt='$text' /></td>";
		echo "<td class='tnm'><a href='?cmd=".urlencode("T#$id")."' " .tooltip('Click here to go to the page of the topic containing the search string') . " >";
		// FIXME: poll icon
		echo utf8entities($p[$i]['title']) . "</a><br/>Forum: ";
		echo "<a href='?cmd=".urlencode("C#".$p[$i]['rctype']."#".$p[$i]['catid']) . "' " .tooltip('Click here to go to main page of the category of forums in which the post containing the search string was posted') . " >" . utf8entities($p[$i]['catname']) . "</a> &gt; ";
		echo "<a href='?cmd=".urlencode("F#".$p[$i]['rctype']."#".$p[$i]['fid']) . "' " .tooltip('Click here to go to main page of the forum in which the post containing the search string was posted') . " >" . utf8entities($p[$i]['fname']) . "</a></td>";
		echo "<td class='trp'>" . ($p[$i]['nitems'] - 1) . "</td>";
		echo "<td class='tps'>" . gmstrftime('%H:%M:%S on %d/%m/%Y', $p[$i]['moment']) . " by ";
		if	($p[$i]['author_id'] != '')
			echo "<a href='message?a=c&ct=0&id=".$p[$i]['author_id']."'  " .tooltip('Click here to go to compose page and send a private message to the author of the first post ') . " >".utf8entities($p[$i]['author'])."</a>";
		else
			echo "<b>".utf8entities($p[$i]['author'])."</b>";
		echo "</td><td class='tps'>" . gmstrftime('%H:%M:%S on %d/%m/%Y', $p[$i]['last_moment']) . " by ";
		if	($p[$i]['last_author_id'] != '')
			echo "<a href='message?a=c&ct=0&id=".$p[$i]['last_author_id']."' " .tooltip('Click here to go to compose page and send a private message to the author of the last post ') . " >".utf8entities($p[$i]['last_author'])."</a>";
		else
			echo "<b>".utf8entities($p[$i]['last_author'])."</b>";
		echo "</td></tr>";
	}
?>
   <form method='post' action='?'>
    <input type='hidden' name='cmd' value='s' />
    <input type='hidden' name='pg' value='<?=$page?>' />
    <table cellspacing="0" cellpadding="0" class="fcmd">
     <tr>
      <td><?=($page > 0) ? ("<a href='?cmd=s&pg=".($page - 1) . "' " .tooltip('Click here to go to the previous page of retrieved topics') . "> ") : ""?>&lt;- Previous page<?=($page > 0) ? "</a>" : ""?></td>
      <td>Display <span <?=tooltip('Use this drop down list to select the number of topics to display on each page')?>><select name='mpp' onChange="submit();"><?php

	for	($i=1;$i<6;$i++)
		echo "<option" . ($i*10==$mPPage?" selected='selected'":"") . ">" . ($i*10) . "</option>";

?></select></span> topics per page</td>
      <td><?=(count($args['topics'])==$mPPage+1)?("<a href='?cmd=s&pg=".($page+1)."' " .tooltip('Clickhere to go to the next page of retrieved topics') . " >"):""?>Next page -&gt;<?=(count($args['topics'])==$mPPage+1)?"</a>":""?></td>
     </tr>
    </table>
   </form>
<?
}
?>
  </td>
 </tr>
</table>

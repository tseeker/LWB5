<?
$f = $args['forum'];
$fid = 'F#'.$f['ctype']."#".$f['id'];
list($tPPage,$page,$nPages) = $args['details'];
$mod = $f['mod'];
?>
<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu($fid, $args['cats']); ?>
  </td>
  <td>
   <table cellspacing='0' cellpadding='0'><tr>
    <td class="pc50"><h1><?=utf8entities($f['title'])?></h1></td>
    <td class='maar'><a href="?cmd=<?=urlencode('MF#'.$f['ctype']."#".$f['id'])?>&pg=<?=$page?>" <?=tooltip('Click here to mark all topics in this forum as read')?>>Mark topics as read</a></td>
   </tr></table>
<?
if	($f['description'] != '')
	echo "<p>" . preg_replace('/\n/', '<br/>', utf8entities($f['description'])) . "</p>\n";
?>
   <form action="?" method="post">
    <input type="hidden" name="cmd" value="<?=$fid?>" />
    <input type="hidden" name="pg" value="<?=$page?>" />

    <table cellspacing="0" cellpadding="0" class="fcmd"><tr>
     <td><?

if	($f['user_post'] || $f['mod'])
	echo "<a href='?cmd=n&f=".urlencode($fid)."' ".tooltip('Click here to go to the new topic form and post a new topic').">New topic</a>";
else
	echo "Only moderators can create topics";

    ?></td>
     <td>
      Display <span <?=tooltip('Use this drop down list to choose the number of topics to display on each page')?>><select name="tpp" onChange="form.submit()"><?

for	($i=1;$i<6;$i++)
{
	echo "<option value='" . ($i * 10) . "'";
	if	($i*10 == $tPPage)
		echo " selected='selected'";
	echo ">" . ($i*10) . "</option>";
}

      ?></select></span> topics per page
     </td>
     <td <?=tooltip('Use this drop down list to choose the page of topics to display')?>>Page <?

if	($nPages <= 1)
	echo "1 / 1";
else
{
	echo "<select name='pg' onChange='form.submit()'>";
	for	($i=0;$i<$nPages;$i++)
		echo "<option value='$i'" . ($page == $i ? " selected='selected'" : "") . ">" . ($i+1) . "</option>";
	echo "</select> / $nPages";
}

     ?></td>
    </tr></table>

    <table cellspacing="0" cellpadding="0" class="ftop">
<?

if	($f['topics'] == 0)
	echo "<tr><td class='fem'>This forum is empty.</tr></td>\n";
else
{
	echo "<tr><th class='tpic'>&nbsp;</th>";
	if	($mod)
		echo "<th class='tpic'>&nbsp;</th>";
	echo "<th class='tnm'>Topic</th><th class='trp'>Replies</th>";
	echo "<th class='tps'>First Post</th><th class='tps'>Last Post</th></tr>\n";

	$topics = $args['topics'];
	for	($i=0;$i<count($topics);$i++)
	{
		echo "<tr>";
		if	($mod)
			echo "<td class='tpic'><input " . tooltip('Use this checkbox to select this topic') . " type='checkbox' id='msel$i' name='msel[]' value='" . $topics[$i]['id'] . "' /></td>";
		$pic  = config::$main['staticurl'] . "/beta5/pics/" . ($topics[$i]['read'] ? '' : 'un') . 'read';
		if	($topics[$i]['sticky'])
			$pic .= '_sticky';
		$pic .= '.gif';
		$text = ($topics[$i]['read'] ? 'Read' : 'Unread') . ($topics[$i]['sticky'] ? " sticky" : "") . " topic";
		echo "<td class='tpic'><img src='$pic' alt='$text' /></td>";
		echo "<td class='tnm'><a href='?cmd=".urlencode("T#".$f['ctype']."#".$topics[$i]['id'])."' " . tooltip('Click here to diplay the posts in this topic') . ">";
		// FIXME: poll icon
		echo utf8entities($topics[$i]['title']) . "</a></td>";
		echo "<td class='trp'>" . $topics[$i]['posts'] . "</td>";
		echo "<td class='tps'>" . gmstrftime('%H:%M:%S on %d/%m/%Y', $topics[$i]['moment']) . " by ";
		if	($topics[$i]['author_id'] != '')
			echo "<a href='message?a=c&ct=0&id=".$topics[$i]['author_id']."' ". tooltip('Click here to send a message to the author of this topic') . " >".utf8entities($topics[$i]['author'])."</a>";
		else
			echo "<b>".utf8entities($topics[$i]['author'])."</b>";
		echo "</td><td class='tps'>" . gmstrftime('%H:%M:%S on %d/%m/%Y', $topics[$i]['last_moment']) . " by ";
		if	($topics[$i]['last_author_id'] != '')
			echo "<a href='message?a=c&ct=0&id=".$topics[$i]['last_author_id']."' ". tooltip('Click here to send a message to author of the last post in this topic') . " >".utf8entities($topics[$i]['last_author'])."</a>";
		else
			echo "<b>".utf8entities($topics[$i]['last_author'])."</b>";
		echo "</td></tr>";
	}
}

?>
    </table>
<?
if	($mod && count($topics))
{
?>
    <table cellspacing="0" cellpadding="0" class="fcmd"><tr>
     <td>
      <input <?=tooltip('Click here to delete the selected topics')?> type="submit" name="dt" value="Delete" onClick="return confirmDelete()" />
      <input <?=tooltip('Click here to go to switch sticky status on the selected topics')?> type="submit" name="st" value="Switch sticky" onClick="return confirmSticky()" />
     </td>
     <td>
<?
	$mf = array();
	foreach	($args['cats'] as $cid => $cat) {
		if	($cat['type'] == 'A' && $f['ctype'] != 'A' || $cat['type'] != 'A' && $f['ctype'] == 'A')
			continue;
		foreach	($cat['forums'] as $cf)
			if	($cf['id'] != $f['id'] && $cf['mod'])
				array_push($mf, $cf);
	}

	if	(!count($mf))
		echo "&nbsp;";
	else
	{
		echo "<input type='submit' " . tooltip('Click here to move the selected topics to the  chosen forum') . "  name='mt' value='Move' onClick='return confirmMove()' /> to forum ";
		echo"<span " . tooltip('use this drop down list to choose the forum to which to move the selected topics') . "  ><select name='mdest' id='mdest'><option value=''>---------</option>";
		foreach	($mf as $cf)
			echo "<option value='" . $cf['id'] . "'>" . utf8entities($cf['title']) . "</option>";
		echo "</select></span>";
	}
?></td>
    </tr></table>
<?
}
?>

   </form>
  </td>
 </tr>
</table>

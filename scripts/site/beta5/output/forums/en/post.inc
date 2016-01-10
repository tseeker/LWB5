<?
$f = $args['forum'];
if	($args['cmd'] == 'n')
	$title = utf8entities($f['title'])." - New Topic";
else if	(substr($args['cmd'],0,2) == 'R#')
	$title = "Reply to '" . utf8entities($args['posts'][0]['title']) . "'";
else if	(substr($args['cmd'],0,2) == 'E#')
	$title = "Editing '" . utf8entities($args['post']['title']) . "'";
?>
<table cellspacing="0" cellpadding="0">
 <tr>
  <td class="mmenu">
   <? drawForumsMenu('F#'.$args['cfid'], $args['cats']); ?>
  </td>
  <td>
   <h2><?=$title?></h2>
   <form action="?#box" method='post'>
    <input type='hidden' name='cmd' value='<?=$args['cmd']?>' />
    <input type='hidden' name='f' value='F#<?=$args['cfid']?>' />
    <input type='hidden' name='ref' value='<?=$args['ref']?>' />
    <input type='hidden' name='uid' value='<?=$args['uid']?>' />

    <a name='box'></a>
    <table cellspacing="0" cellpadding="0" class="fpost">
     <tr><td colspan='3'>&nbsp;</td></tr>
<?

$errs = array('Subject is too short.', 'Subject is too long.', 'Contents are too short.', 'Contents are too long.');
if	($args['err'])
	echo "<tr><td>&nbsp;</td><th class='err'>" . $errs[$args['err']-1] . "</th><td>&nbsp;</td></tr>";

?>
     <tr>
      <th><label for="sub" accesskey="s"><?=$args['cmd'] == 'n' ? "Topic title" : "Subject"?>:</label></th>
      <td colspan="2"><input <?=tooltip('Use this text field to type in or modify the subject of your post')?> type='text' id="sub" name='sub' value="<?=utf8entities($args['sub'], ENT_QUOTES)?>" maxlength="100" size='60' /></td>
     </tr>
     <tr>
      <th><label for="txt" accesskey="t">Text:</label></th>
      <td <?=tooltip('Use this text area to type in or modify your message')?> ><textarea name='txt' id="txt" cols='60' rows='15'><?=utf8entities($args['txt'])?></textarea></td>
      <td id="jszone">&nbsp;</td>
     </tr>
     <tr>
      <th>Options:</th>
      <td colspan="2">
       <input <?=tooltip('Use this checkbox to enable / disable graphical smileys')?> type='checkbox' name='sm' id="esm" value='1'<?=$args['sm']?" checked='checked'":""?> /> <label for="esm" accesskey="e">Enable graphical smileys</label>
       <input <?=tooltip('Use this checkbox to enable / disable forum tags')?> type='checkbox' name='fc' id="efc" value='1'<?=$args['fc']?" checked='checked'":""?> /> <label for="efc" accesskey="f">Enable forum tags</label>
      </td>
     </tr>
<?

if	($args['sst'])
{
?>
     <tr>
      <th>Sticky topic:</th>
      <td colspan="2">
       <input <?=tooltip('Use this checkbox to change the sticky status of the post')?> type='checkbox' name='st' id="est" value='1'<?=$args['st']?" checked='checked'":""?> /> <label for="est" accesskey="y">Enable sticky topic</label>
      </td>
     </tr>
<?
}

?>
     <tr><td colspan='3'>&nbsp;</td></tr>
     <tr><td>&nbsp;</td><td colspan='2' class='butt'>
      <input <?=tooltip('Click here to post your message')?> type='submit' name='e' value='Submit' accesskey="g" />
      <input <?=tooltip('Click here to preview your message as it will be displayed in the forums')?> type='submit' name='p' value='Preview' accesskey="p" />
      <input <?=tooltip('Click here to cancel your changes and go back to the previous page')?> type='submit' name='c' value='Cancel' accesskey="c" />
     </td></tr>
<?php

if	($args['prev'] != "")
{
	echo "  <tr><td rowspan='3'>&nbsp;</td><td>&nbsp;</td><td rowspan='3'>&nbsp;</td></tr>\n";
	echo "  <tr><th class='hdprev'>Preview - " . utf8entities($args['sub']) . "</th></tr>\n";
	echo "  <tr><td class='prev'>" . $args['prev'] . "</td></tr>\n";
}

if	(is_array($args['posts']))
{
	echo "  <tr><td rowspan='3'>&nbsp;</td><td>&nbsp;</td><td rowspan='3'>&nbsp;</td></tr>\n";
	echo "  <tr><th class='hdprev'>Replying to ...</th></tr>\n";
	echo "  <tr><td>";
	$p = $args['posts'];
	for	($i=0;$i<count($p);$i++)
	{
		echo "<table cellspacing='0' cellpadding='0' class='ftp'>";
		echo "<tr><th>" . utf8entities($p[$i]['title']) . "</td></tr>\n";
		echo "<tr><td>Posted <b>" . gmstrftime("%H:%M:%S on %d/%m/%Y", $p[$i]['moment']);
		echo "</b> by <b>".$p[$i]['author']."</b></td>";
		echo "</td></tr>\n";
		echo "<tr><td>" . $p[$i]["html"] . "</td></tr>\n";
		echo "</table>\n";
	}
	echo "</td></tr>\n";
}
else if	(is_array($args['post']))
{
	echo "  <tr><td rowspan='3'>&nbsp;</td><td>&nbsp;</td><td rowspan='3'>&nbsp;</td></tr>\n";
	echo "  <tr><th class='hdprev'>Original post</th></tr>\n";
	echo "  <tr><td>";
	$p = $args['post'];
	echo "<table cellspacing='0' cellpadding='0' class='ftp'>";
	echo "<tr><th>" . utf8entities($p['title']) . "</td></tr>\n";
	echo "<tr><td>Posted <b>" . gmstrftime("%H:%M:%S on %d/%m/%Y", $p['moment']);
	echo "</b> by <b>".$p['author']."</b></td>";
	echo "</td></tr>\n";
	echo "<tr><td>" . $p["html"] . "</td></tr>\n";
	echo "</table>\n";
	echo "</td></tr>\n";
}

?>
    </table>
   </form>
  </td>
 </tr>
</table>

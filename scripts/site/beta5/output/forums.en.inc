<?

function	drawForumsMenu($mode,$fList)
{
?>
<table cellspacing="0" cellpadding="0">
 <tr><td colspan="3">&nbsp;</td></tr>
 <tr><th colspan="3">Forums</th></tr>
 <tr><td colspan="3" style="text-align:right;padding:0px 15px 0px 0px"><a style="color:white;text-decoration:underline" href="manual?p=bugs">Help</a></td></tr>
 <tr>
  <td class="mmspc" rowspan="2">&nbsp;</td>
  <td colspan="2"><a href='?cmd=o' <?=tooltip('Click here to display the forums overview')?>><?=($mode=='o')?"<i>":""?>Overview<?=($mode=='o')?"</i>":""?></a></td>
 </tr>
 <tr><td colspan="2"><a href='?cmd=l' <?=tooltip('Click here to display the latest messages in all forums')?>><?=($mode=='l')?"<i>":""?>Latest messages<?=($mode=='l')?"</i>":""?></a></td></tr>
 <tr><td colspan="3">&nbsp;</td></tr>
<?

	foreach	($fList as $cid => $cat)
	{
		$sel1 = ($mode == "C#$cid");
		$selL = ($mode == "L#$cid");
		$sel = $sel1 || $selL;
		if	(!$sel && substr($mode,0,4) == "F#".$cat['type']."#")
		{
			for	($i=0;!$sel&&$i<count($cat['forums']);$i++)
				$sel = (substr($mode,4) == $cat['forums'][$i]['id']);
		}

		$au = 0;
		if	(!$sel)
			for	($i=0;$i<count($cat['forums']);$i++)
				$au += $cat['forums'][$i]['unread'];

		$cb = $au ? '<b>' : ''; $ceb = $au ? " ($au)</b>" : '';

		echo '<tr><td class="mmspc" rowspan="'.(2+($sel?count($cat['forums']):0)).'">&nbsp;</td>';
		echo "<td colspan='2'><a href='?cmd=".urlencode("C#$cid")."' ".tooltip('Click here to display this forums category main page').">$cb".($sel1?"<i>":"").utf8entities($cat['title']);
		echo ($sel1?"</i>":"")."$ceb</a></td></tr>\n";

		if	($sel)
			foreach	($cat['forums'] as $f)
			{
				$au = $f['unread'];
				$cb = $au ? '<b>' : ''; $ceb = $au ? " ($au)</b>" : '';
				$fid = "F#".$cat['type']."#".$f['id'];
				$selF = ($mode == $fid);
				echo "<tr><td class='mmspc'>&nbsp;</td>";
				echo " <td><a href='?cmd=".urlencode($fid)."' ".tooltip('Click here to display the contents of this forum').">$cb".($selF?"<i>":"").utf8entities($f['title']).($selF?"</i>":"")."$ceb</a></td>";
				echo "</tr>\n";
			}

		echo '<tr><td class="mmspc">&nbsp;</td>';
		echo "<td><a href='?cmd=".urlencode("L#$cid")."' ".tooltip('Click here to display the latest messages this forums category').">".($selL?"<i>":"").'Latest messages'.($selL?"</i>":"")."</a></td></tr>\n";
		echo "<tr><td colspan='3'>&nbsp;</td>\n";
	}

?>
 <tr>
  <td class="mmspc" rowspan="2">&nbsp;</td>
  <td colspan="2"><a href='?cmd=s' <?=tooltip('Click here to go to the forums search page')?>><?=($mode=='s')?"<i>":""?>Search forums<?=($mode=='s')?"</i>":""?></a></td>
 </tr>
 <tr><td colspan="3">&nbsp;</td>
 <tr><th colspan="3"><a href="message" <?=tooltip('click here to go to the messaging system page')?>>Messages</a></th></tr>
</table>
<?
}

$sp = $args['sp'];
$args = $args['d'];
include("forums/en/$sp.inc");

?>

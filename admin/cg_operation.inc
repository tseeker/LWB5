<html>
 <head>
  <title>Legacy Worlds Beta 5 > Administration > Create game</title>
 </head>
 <body style="background-color: #ffdfdf">
  <table style="margin: 50px 0; padding: 0px;width:100%" cellspacing="0" cellpadding="0">
   <tr>
    <td width="33%">&nbsp;</td>
    <td width="34%"><table style="border: 4px solid red; background-color:white; padding: 5px;width: 100%">
     <tr>
      <td colspan="3" style="padding: 2px 0px;font-size:150%;color:red;text-align:center">LegacyWorlds Beta 5</td>
     </tr>
     <tr>
      <td colspan="3" style="padding: 2px 0px;font-size:150%;color:red;text-align:center">Game creation in progress</td>
     </tr>
     <tr><td colspan="3" style="height:5px;font-size:1pt">&nbsp;</td></tr>
     <tr><td colspan="3" style="padding: 2px 0px;color:#ff3f3f;text-align:center"><?=$op['text']?></td></tr>
     <tr>
      <td style="width:10%">&nbsp;</td>
      <td><table style="border: 1px solid black; padding: 1px; margin: 0px; width: 100%; height: 16px"><tr>
<?php

if ($op['pc'] == 0) {
	print "<td style='font-size:1pt'>&nbsp;</td>";
} elseif ($op['pc'] == 100) {
	print "<td style='font-size:1pt;background-color:#7f0000'>&nbsp;</td>";
} else {
	print "<td style='font-size:1pt;background-color:#7f0000;width:" . $op['pc'] . "%'>&nbsp;</td>"
		. "<td style='font-size:1pt'>&nbsp;</td>";
}

?>
      </tr></table></td>
      <td style="width:10%">&nbsp;</td>
     </tr>
     <tr><td colspan="3" style="padding: 2px 0px;color:#ff3f3f;text-align:center"><?=$op['pc']?>% complete</td></tr>
     <tr><td colspan="3">&nbsp;</td></tr>
    </table></td>
    <td width="33%">&nbsp;</td>
  </table>
<script language="JavaScript">
window.setTimeout('window.location="<?=$op['to']?>"', <?=$op['delay']*1000?>);
</script>
 </body>
</html>

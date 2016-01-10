<h2>Send admin spam</h2>
<p>
 Type the spam in the form below. You can use forum tags inside the spam.<br/>
 Use the "Preview" button to check on your spam before sending it.<br/>
 If you need to send the spam to all players within all active games, check the "Send in all games" box.
</p>
<form action="?" method='post'>
 <input type='hidden' name='c' value='s' />

 <a name='box'></a>
 <table cellspacing="0" cellpadding="0" style="width: 75%">
  <tr><td colspan='3'>&nbsp;</td></tr>
<?

$errs = array('Subject is too short.', 'Subject is too long.', 'Contents are too short.', 'Contents are too long.');
if ($data['err']) {
	echo "  <tr><td>&nbsp;</td><th style='text-align:left;color:red'>" . $errs[$data['err']-1]
		. "</th><td>&nbsp;</td></tr>";
}

?>
  <tr>
   <th style="width:10%;text-align:right"><label for="sub" accesskey="s">Subject:</label></th>
   <td colspan="2"><input type='text' id="sub" name='sub' style="width:90%" value="<?=utf8entities($data['sub'], ENT_QUOTES)?>" maxlength="100" size='60' /></td>
  </tr>
  <tr>
   <th style="vertical-align:top;width:10%;text-align:right"><label for="txt" accesskey="t">Text:</label></th>
   <td colspan="2"><textarea name='txt' id="txt" style="width:90%" rows='15'><?=utf8entities($data['txt'])?></textarea></td>
  </tr>
  <tr>
   <th style="width:10%;text-align:right">Options:</th>
   <td colspan="2">
    <input type='checkbox' name='ag' id="eag" value='1'<?=$data['ag']?" checked='checked'":""?> /> <label for="eag" accesskey="a">Send in all games</label>
   </td>
  </tr>
  <tr><td colspan='3'>&nbsp;</td></tr>
  <tr>
   <td>&nbsp;</td>
   <td colspan='2' class='butt'>
    <input type='submit' name='e' value='Spam, spam, spam' accesskey="g" />
    <input type='submit' name='p' value='Preview' accesskey="p" />
    <input type='submit' name='cc' value='Cancel' accesskey="c" />
   </td>
  </tr>
<?php

if ($data['prev'] != "") {
	echo " <tr><td rowspan='3'>&nbsp;</td><td>&nbsp;</td><td rowspan='3'>&nbsp;</td></tr>\n";
	echo " <tr><th style='text-align:left'>Preview - " . utf8entities($data['sub']) . "</th></tr>\n";
	echo " <tr><td style='border: 1px solid #3f3f3f;padding: 4px'>" . $data['prev'] . "</td></tr>\n";
}

?>
 </table>
</form>

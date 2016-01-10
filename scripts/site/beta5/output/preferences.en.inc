<?php

function	makeOpt($a,$n,$v)
{
	$t = "<option value='$v'";
	if	($a[$n] == $v)
		$t .= " selected='selected'";
	$t .= ">";
	return $t;
}

?>
<form method='post' action='?'>
 <table cellspacing="0" cellpadding="0">
  <tr>
   <td class="pc5" rowspan="7">&nbsp;</td>
   <td class="pc45">
    <table class="prefs">
     <tr><td class="psec" colspan="2"><h1>Account</h1></td></tr>
<?php
if	(!is_null($args['err1']))
	echo "<tr><td class='err' colspan='2'>The address you entered is invalid, please correct it.</td></tr>\n";
?>
     <tr>
      <th class="div2">E-mail address:</th>
      <td class="div2"><input type='text' name='mail' <?=tooltip('Use this text field to type in your email address')?>  class='txt' value="<?=is_null($args['err1'])?$args['mail']:$args['err1']?>" size="35" maxlength="128" /></td>
     </tr>
     <tr><td colspan='2'
<?php
if	(!is_null($args['err2']))
{
	echo " class='err'>";
	switch ($args['err2']) :
	case 1: echo "A database access error has occured"; break;
	case 2: echo "The current password is incorrect"; break;
	case 3: echo "The new password and its confirmation are different"; break;
	case 4: echo "The new password is too short (minimum 4 characters)"; break;
	case 5: echo "The new password is too long (maximum 64 characters)"; break;
	case 6: echo "The new password must be different from your user name"; break;
	endswitch;
}
else
	echo " class='note'>Please leave these fields empty if you do not intend to change your password.";
?></td></tr>
     <tr>
      <th>Current password:</th>
     <td><input type='password' class='txt' name='opass' <?=tooltip('Use this text field to type in your current password')?>/></td>
     </tr>
     <tr>
      <th>New password:</th>
      <td><input type='password' class='txt' name='npass' <?=tooltip('Use this text field to type in your new password')?>/></td>
     </tr>
     <tr>
      <th>Confirm new password:</th>
      <td><input type='password' class='txt' name='cpass' <?=tooltip('Use this text field to confirm your new password')?>/></td>
     </tr>
    </table>
   </td>
   <td class="pc45">
    <table class="prefs">
     <tr><td class="psec" colspan="2">
      <table style="width:100%;padding:0;margin:0"><tr>
       <td style="vertical-align:top"><h1>Display</h1></td>
       <td style="text-align:right;vertical-align:top"><a href="manual?p=preferences">Help</a></td>
      </tr></table>
     </td></tr>
     <tr>
      <th class="div2">Language:</th>
      <td class="div2" <?=tooltip('Use this drop down list to choose the language to use')?>><select name='lang' >
       <?=makeOpt($args,'lang','en')?>English</option>
      </select></td>
     </tr><tr>
      <th>Font size:</th>
      <td <?=tooltip('Use this drop down list to select the font size')?>><select name='fs'>
       <?=makeOpt($args,'fs',0)?>tiny</option>
       <?=makeOpt($args,'fs',1)?>small</option>
       <?=makeOpt($args,'fs',2)?>normal</option>
       <?=makeOpt($args,'fs',3)?>big</option>
       <?=makeOpt($args,'fs',4)?>huge</option>
      </select></td>
     </tr><tr>
      <th>Tooltips:</th>
      <td <?=tooltip('Use this drop down list to set up tooltips delays')?>><select name='tt'>
       <?=makeOpt($args,'tt',0)?>Disabled</option>
       <?=makeOpt($args,'tt',1)?>0.5 second</option>
       <?=makeOpt($args,'tt',2)?>1 second</option>
       <?=makeOpt($args,'tt',3)?>1.5 second</option>
       <?=makeOpt($args,'tt',4)?>2 seconds</option>
       <?=makeOpt($args,'tt',5)?>2.5 seconds</option>
       <?=makeOpt($args,'tt',6)?>3 seconds</option>
      </select></td>
     </tr><tr>
      <th>Theme:</th>
      <td <?=tooltip('Use this drop down list to choose your interface theme')?>><select name='thm'>
       <?=makeOpt($args,'thm','default')?>LegacyWorlds Beta 5</option>
       <?=makeOpt($args,'thm','invert')?>Beta 5 Reversed</option>
       <?=makeOpt($args,'thm','classic')?>LegacyWorlds Classic</option>
       <?=makeOpt($args,'thm','cripes')?>That Other Theme</option>
      </select></td>
     </tr><tr>
      <th>Colour scheme:</th>
      <td <?=tooltip('Use this drop down list to choose the colour scheme to use')?>><select name='col'>
       <?=makeOpt($args,'col','red')?>red</option>
       <?=makeOpt($args,'col','green')?>green</option>
       <?=makeOpt($args,'col','blue')?>blue</option>
       <?=makeOpt($args,'col','grey')?>grey</option>
       <?=makeOpt($args,'col','purple')?>purple</option>
       <?=makeOpt($args,'col','yellow')?>yellow</option>
      </select></td>
     </tr>
    </table>
   </td>
   <td class="pc5" rowspan="7">&nbsp;</td>
  </tr>
  <tr><td colspan="2"><hr/></td></tr>
  <tr><td colspan="2">
   <table class="prefs">
    <tr><td class="psec" colspan='4'><h1>Forums</h1></td></tr>
    <tr>
     <th class="div4">Topics/page:</th>
     <td class="div4" <?=tooltip('Use this drop down list to choose the number oftopics to display on each forum page')?>><select name='tpp'><?php
for	($i=1;$i<6;$i++)
	echo "<option" . ($i*10==$args['tpp']?" selected='selected'":"") . ">" . ($i*10) . "</option>";
?></select></td>
     <th class="div4">Messages/page:</th>
     <td class="div4" <?=tooltip('Use this drop down list to choose the number ofmessages to display on each topic page')?>><select name='mpp'><?php
for	($i=1;$i<6;$i++)
	echo "<option" . ($i*10==$args['mpp']?" selected='selected'":"") . ">" . ($i*10) . "</option>";
?></select></td>
    </tr>
    <tr>
     <th>Graphical smileys:</th>
     <td <?=tooltip('Use this drop down list to enable / disable graphical smileys in the forums')?>><select name='gsm' >
      <option value='0'>Disabled</option>
      <option value='1'<?=$args['gsm']?" selected='selected'":""?>>Enabled</option>
     </td>
     <th>Forum tags:</th>
     <td <?=tooltip('Use this drop down list to enable / disable forum tags - those are necessary to handle text modifiers')?>><select name='gft'>
      <option value='0'>Disabled</option>
      <option value='1'<?=$args['gft']?" selected='selected'":""?>>Enabled</option>
     </td>
    </tr>
    <tr>
     <th>Display mode:</th>
     <td <?=tooltip('Use this drop down list to choose between linear and threaded displays')?>><select name='fdm'>
      <option value='0'>Linear</option>
      <option value='1'<?=$args['fdm']?" selected='selected'":""?>>Threaded</option>
     </td>
     <th>Messages order:</th>
     <td <?=tooltip('Use this drop down list to choose messager ordering criteria')?>><select name='fmo'>
      <option value='0'>Oldest first</option>
      <option value='1'<?=$args['fmo']?" selected='selected'":""?>>Newest first</option>
     </td>
    </tr>
    <tr>
     <th>Signature:</th>
     <td colspan='3' <?=tooltip('Use this text area to type in your signature for your posts in the forums ')?>><textarea name='fsig'><?=utf8entities($args['fsig'])?></textarea></td>
    </tr>
   </table>
  </td></tr>
  <tr><td colspan="2"><hr/></td></tr>
  <tr><td colspan="2" class='buttons'>
   <input type='submit' value='Update preferences' <?=tooltip('Click here to update your preferences')?>/>
   <input type='reset' value='Restore previous values' <?=tooltip('Click here to cancel your changes and restore the previous values')?> />
  </td></tr>
<?
if ($args['lok']) {
?>
  <tr><td colspan="2"><hr/></td></tr>
  <tr><td colspan="2">
   <h1>Leave Game</h1>
   <p>
<?

	if	(is_null($args['quit']))
	{
?>
	Click the button bellow to leave this game. Please note that it will not close your account; it will simply quit the current game. You will have 24 hours after you click the button to cancel your action; after that, your planets will be made neutral, your fleets will be destroyed, your private messages erased, etc...<br/><br/>
	You will still be able to access the other games you're playing, and you'll be able to start playing any other game from the Account page.<br/><br/><input type='submit'<?=tooltip('Click here to leave this Legacy Worlds Game')?> name='quit' value='Leave <?=$args['name']?>' />
<?
	} else {
		echo "You have decided to leave {$args['name']}. Your player information will be destroyed in ";
		$tl = 86400 + $args['quit']  - time();
		$h = ($tl - $tl % 3600) / 3600;
		if	($h > 0)
			echo "<b>$h</b> hour" . ($h>1 ? 's' : '');
		$tl -= $h * 3600;
		$m = ($tl - $tl % 60) / 60;
		if	($m > 0)
			echo ($h != 0 ? ' and ' : '') . "<b>$m</b> minute" . ($m>1 ? 's' : '');
		$tl -= $m * 60;
	?>
	unless you decide to cancel this action by clicking the button below.<br/><br/><input type='submit' name='quit' <?=tooltip('Click here to cancel your request to leave this game')?> value='Do NOT leave <?=$args['name']?>' />
<?
	}
}
?>
   </p>
  </td></tr>
 </table>
</form>

<?
include("menu.en.inc");
$this->title = "Preferences";
$this->startContents();
?>
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
<form method='post' action='?'><div>
 <h3 style="text-align: center"><?=$_SESSION['login']?>'s general preferences</h3>
 <p>&nbsp;</p>
 <table style="width: 100%">
<?php
if (!is_null($args['err1']))
	echo "<tr><td colspan='4' style='text-align:center'>The address you entered is invalid, please correct it.</td></tr>\n";
?>
  <tr>
   <td>E-mail address:</td>
   <td><input type='text' name='mail' value="<?=is_null($args['err1'])?$args['mail']:$args['err1']?>" class="input"/></td>
   <td>Language:</td>
   <td><select name='lang' class="input">
    <?=makeOpt($args,'lang','en')?>English</option>
   </select></td>
  </tr>
  <tr>
   <td>Colour scheme:</td>
   <td><select name='col' class="input">
    <?=makeOpt($args,'col','red')?>red</option>
    <?=makeOpt($args,'col','green')?>green</option>
    <?=makeOpt($args,'col','blue')?>blue</option>
    <?=makeOpt($args,'col','grey')?>grey</option>
    <?=makeOpt($args,'col','purple')?>purple</option>
    <?=makeOpt($args,'col','yellow')?>yellow</option>
   </select></td>
   <td>Font size:</td>
   <td><select name='fs' class="input">
    <?=makeOpt($args,'fs',0)?>tiny</option>
    <?=makeOpt($args,'fs',1)?>small</option>
    <?=makeOpt($args,'fs',2)?>normal</option>
    <?=makeOpt($args,'fs',3)?>big</option>
    <?=makeOpt($args,'fs',4)?>huge</option>
   </select></td>
  </tr>

  <tr><td colspan='4'>&nbsp;</td></tr>
  <tr><th colspan='4'>Forums</th></tr>
  <tr><td colspan='4'>
  <tr>
   <td>Topics/page:</td>
   <td><select name='tpp' class="input"><?php
for	($i=1;$i<6;$i++)
	echo "<option" . ($i*10==$args['tpp']?" selected='selected'":"") . ">" . ($i*10) . "</option>";
?></td>
   <td>Messages/page:</td>
   <td><select name='mpp' class="input"><?php
for	($i=1;$i<6;$i++)
	echo "<option" . ($i*10==$args['mpp']?" selected='selected'":"") . ">" . ($i*10) . "</option>";
?></td>
  </tr>
  <tr>
   <td>Graphical smileys:</td>
   <td><select name='gsm' class="input">
    <option value='0'>Disabled</option>
    <option value='1'<?=$args['gsm']?" selected='selected'":""?>>Enabled</option>
   </td>
   <td>Forum tags:</td>
   <td><select name='gft' class="input">
    <option value='0'>Disabled</option>
    <option value='1'<?=$args['gft']?" selected='selected'":""?>>Enabled</option>
   </td>
  </tr>
  <tr>
   <td>Display mode:</td>
   <td><select name='fdm' class="input">
    <option value='0'>Linear</option>
    <option value='1'<?=$args['fdm']?" selected='selected'":""?>>Threaded</option>
   </td>
   <td>Messages order:</td>
   <td><select name='fmo' class="input">
    <option value='0'>Oldest first</option>
    <option value='1'<?=$args['fmo']?" selected='selected'":""?>>Newest first</option>
   </td>
  </tr>
  <tr>
   <td>Signature:</td>
   <td colspan='3'><textarea name='fsig' cols='60' class="input" rows="2"><?=utf8entities($args['fsig'])?></textarea></td>
  </tr>

  <tr><td colspan='4'>&nbsp;</td></tr>
  <tr><th colspan='4'>Password modification</th></tr>
  <tr><td colspan='4'>
   Please leave theses fields empty if you do not intend to change your password.
  </td></tr>
<?php
if	(!is_null($args['err2']))
{
	echo "<tr><td colspan='4'><center>";
	switch ($args['err2']) :
	case 1: echo "A database access error has occured"; break;
	case 2: echo "The current password is incorrect"; break;
	case 3: echo "The new password and its confirmation are different"; break;
	case 4: echo "The new password is too short (minimum 4 characters)"; break;
	case 5: echo "The new password is too long (maximum 64 characters)"; break;
	case 6: echo "The new password must be different from your user name"; break;
	endswitch;
	echo ".</center></td></tr>\n";
}
?>
  <tr><td colspan='4'>&nbsp;</td></tr>
  <tr>
   <td colspan='2'>Current password:</td>
   <td colspan='2'><input type='password' name='opass' class="input" /></td>
  </tr>
  <tr>
   <td colspan='2'>New password:</td>
   <td colspan='2'><input type='password' name='npass' class="input" /></td>
  </tr>
  <tr>
   <td colspan='2'>Confirm new password:</td>
   <td colspan='2'><input type='password' name='cpass' class="input" /></td>
  </tr>

  <tr><td colspan='4'>&nbsp;</td></tr>
  <tr><td colspan='4' style="text-align:center"><input type='submit' value='Update my preferences' class="input" /></td></tr>
 </table>
</div></form>
<? $this->endContents(); ?>

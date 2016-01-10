<?
include("menu.en.inc");
$this->title = "Password recovery";
$this->startContents();
?>
<?php

if ($args === false || is_array($args) && $args['error'] < 2) {
	if ($args === false) {
		$args = array();
	}
?>
<form action="?" method="post"><div>
 <h1>Forgot your password?</h1>
 <p>
  This page allows you to have the server change your password to a new, random password. In order to do that, you will first have to enter your username as well as the email address you used when you created your account. You will then receive a confirmation code.
 </p>
 <p>&nbsp;</p>
 <table style="width:100%">
  <tr>
   <th style="width:20%;max-width:250px;text-align:left">Username:</th>
   <td><input type="text" name="u" value="<?=preg_replace('/"/', '&quot;', $args['name'])?>" size="17" maxlength="16" class="input" /></td>
  </tr>
  <tr>
   <th style="text-align:left">E-mail address:</th>
   <td><input type="text" name="m" value="<?=preg_replace('/"/', '&quot;', $args['mail'])?>" style="width:80%" class="input" /></td>
  </tr>
<?php
	if ($args['error'] == 1) {
?>
  <tr><td>&nbsp;</td><td><b>Invalid or unknown username or mail address.</b></td></tr>
<?php
		sleep(2);
	}
?>
  <tr><td colspan="2">&nbsp;</td></tr>
  <tr>
   <td>&nbsp;</td>
   <td><input type="submit" value="Send me the confirmation code" class="input" /></td>
  </tr>
 </table>
</div></form>
<?
} elseif ($args === true) {
?>
<h1>Your password has been changed!</h1>
<p>
 You will receive an email containing your new password in a few minutes.<br/>
 Thank you!
</p>
<?php
} elseif ($args['error'] == 2) {
?>
<h1>Unable to send the confirmation mail</h1>
<p>
 The server was unable to send the email containing the confirmation code.<br/>
 It's possible the server is too loaded at the moment; please try again later.<br/>
 Sorry for the inconvenience.
</p>
<?php
} elseif ($args['error'] == 4 || $args['error'] == 3) {
?>
<h1>Confirmation of the request</h1>
<p>
 An email has been sent to the address associated with your account; it contains a confirmation code.<br/>
 Please copy this confirmation code in the box below in order to confirm that you want your password to be reset to a random value which will be mailed to you.
</p>
<p>&nbsp;</p>
<form action="?" method="post"><div>
 <input type="hidden" name="u" value="<?=preg_replace('/"/', '&quot;', $args['name'])?>" class="input" />
 <input type="hidden" name="m" value="<?=preg_replace('/"/', '&quot;', $args['mail'])?>" class="input" />
 <table style="width:100%">
  <tr>
   <th style="width:20%;max-width:250px;text-align:left">Confirmation code:</th>
   <td><input type="text" name="c" value="<?=preg_replace('/"/', '&quot;', $args['code'])?>" size="17" maxlength="16" class="input" /></td>
  </tr>
<?php
	if ($args['error'] == 3) {
?>
  <tr><td>&nbsp;</td><td><b>Invalid confirmation code.</b></td></tr>
<?php
		sleep(2);
	}
?>
  <tr><td colspan="2">&nbsp;</td></tr>
  <tr>
   <td>&nbsp;</td>
   <td><input type="submit" value="Reset my password" class="input" /></td>
  </tr>
 </table>
</div></form>
<?php
} elseif ($args['error'] == 5) {
?>
<h1>Unable to send the new password</h1>
<p>
 The server was unable to send the email containing the new password; your password was therefore kept unmodified.<br/>
 It's possible the server is too loaded at the moment; please try again later.<br/>
 Sorry for the inconvenience.
</p>
<?php
} else {
	$rVal = "LP" . rand(1000000,9999999);
	l::error("main/lostpass: error $rVal ($args)");
?>
<h1>Something unexpected happened ...</h1>
<p>
 Apparently, this script smoked too much Space Weed and just had a fleeting moment of randomness. Of course, what we mean is the this is a bug. Please contact the Legacy Worlds <a href="mailto:staff@legacyworlds.com">staff</a> regarding this error and give them the following value: <?=$rv?>.
</p>
<?php
}

?>
<? $this->endContents(); ?>

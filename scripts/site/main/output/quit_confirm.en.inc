<?
include("menu.en.inc");
$this->addStylesheet("account");
$this->title = "Close account";
$this->startContents();
?>
<div id="tabs">&nbsp;</div>
<div class="acctab" id="quit">
 <h1>You are about to close your account</h1>
 <form action="?#acc-quit" method="post"><div>
  <input type="hidden" name="rqc" value="1" />
  <p id="header">
   If you go ahead with this action, a countdown ranging from 24h to 48h will be set on your account, and it will be
   closed at the end of the countdown.<br/>
   An email will be sent to confirm the account's deletion at this moment.<br/>
   When your account is closed, you will automatically be removed from all of the games you are playing; you will
   lose your rankings as well.<br/>
   The account itself will however not be deleted, and you can choose to open it again at any time.
  </p>
  <p>
   In order to close your account, you must provide the following details to make sure it really is you:
  </p>
 
  <table style="margin:0px 0px 0px 20px;width:80%">
   <tr>
    <th style="width:20%;text-align:left">Password:</th>
    <td><input type="password" name="q_pass" value="" size="30" class="input" /></td>
   </tr>
 <?php
 
if ($args['ePass']) {
?>
   <tr>
    <td>&nbsp;</th>
    <td style="color:white;background-color:red">The password you provided is incorrect.</td>
   </tr>
 <?php
 }
 
 ?>
  </table>
 
  <p>
   In addition, you may provide us with a reason why you're quitting the game, as it could allow us to make it better. But that is of course optional.<br/>
  </p>
 
  <table style="margin:0px 0px 0px 20px;width:80%">
   <tr>
    <th style="text-align:left;vertical-align:top;width:20%">Why are you leaving?</th>
    <td><textarea name="q_reason" style="width:90%" cols="auto" rows="5" class="input"><?=preg_replace(array('/</','/>/','/&/'), array('&lt;','&gt;','/&amp;/'), $args['reason'])?></textarea></td>
   </tr>
  </table>
 
  <p>
   <input type="submit" value="I really want to close my account" class="input" />
   <input type="submit" name="cancel" value="No, wait, I was just kidding" class="input" />
  </p>
 </div></form>
</div>
<? $this->endContents(); ?>

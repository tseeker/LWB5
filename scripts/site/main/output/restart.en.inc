<?
include("menu.en.inc");
$this->addStylesheet('text');
$this->title = "Account re-activation";
$this->startContents();
?>
<form action='?' method='post'><div>
 <h1>Welcome back!</h1>
 <p>
  Since you had left the game, you will need to confirm that you want to
  re-activate this account.
 </p>
 <p>
  Your email address is <b><?=$args['email']?></b>; a confirmation code
  will be sent to that address after you click the button below.
 </p>
 <p>
  Once you have received the email containing the confirmation code, you
  will need to log in again and type in the code.
 </p>
 <p>
  <input type='submit' name="ac_restart" value='Re-enable my account' class="input" />
 </p>
 <p>
  <u>Note:</u> if your email address has changed and you still want to
  keep this account, please send an email to the
  <a href="mailto:staff@legacyworlds.com">staff</a>; you must indicate your
  user name and password in the email.
 </p>
</div></form>
<? $this->endContents(); ?>

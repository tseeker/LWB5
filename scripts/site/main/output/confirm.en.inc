<?
include("menu.en.inc");
$this->title = "Account confirmation";
$this->startContents();
?>
<form action='?' method='post'><div>
 <h1>Account confirmation</h1>
 <p>
  Please type in the confirmation code that was sent to the email address you specified.
 </p>
 <p>&nbsp;</p>
 <?=$args['error']?"<p><b>Invalid confirmation code</b></p>":""?>
 <p>
  Confirmation code: <input type='text' name='authcode' class="input" />
 </p>
 <p>
  <input type='submit' value='Validate my account' class="input" />
 </p>
</div></form>
<? $this->endContents(); ?>

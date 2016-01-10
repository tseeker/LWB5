<?
include("menu.en.inc");
$this->title = "Account creation";
$this->addStylesheet('text');
$this->startContents();
if ($args['success']) {
?>
 <h1>Your Legacy Worlds account has been created!</h1>
 <p>
  An e-mail has been sent to you with your account's details as well as
  the validation procedure that will confirm your account's creation.
 </p>
<?php
} else {
?>
 <h1>An error has occured!</h1>
 <p>
  Something went wrong in the process of creating your account. This should never have
  happened, please contact our <a href='mailto:webmaster@legacyworlds.com'>staff</a> to
  report this.
 </p>
<?php
}
$this->endContents();
?>

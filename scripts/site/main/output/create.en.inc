<?php

function htmlq($v) {
	return	preg_replace('/"/', '&quot;', $v);
}

include("menu.en.inc");
$this->title = "Create an account";
$this->addStylesheet("create");
$this->startContents();

?>
<form action ="?" method="post"><div>
 <h1>Legacy Worlds account creation</h1>
 <div class="fsection" id="name-lang">
<?php

$eAccount = array(
	"Valid characters: letters, numbers, spaces, and _@.+'/",
	"Username is too long (maximum 15 characters).",
	"Username contains invalid characters.<br/>Use letters, numbers, spaces and _.@-+'/ only).",
	"Spaces are not allowed at the beginning or at the end of the username.",
	"Multiple spaces are not allowed.",
	"Username is too short (minimum 2 characters).",
	"Username must contain at least one letter.",
	"This username is not available."
);
if ($args['err1']) {
	$aClass = " error";
} else {
	$aClass = "";
}
echo "<div class='sechd$aClass'>{$eAccount[(int) $args["err1"]]}</div>";

?>
  <div class="formline">
   <div class="fcol1 pad">
    <span>Username:</span>
    <input type='text' name='username' maxlength='15' value="<?=htmlq($args['username'])?>" class="input" />
   </div>
   <div class="fcol2">
    <span>Language:</span>
    <select name='lang' class="input">
     <option value='en' selected='selected'>English</option>
    </select>
   </div>
  </div>
 </div>
<?php
$eMail = array(
	"The e-mail address <i>MUST</i> exist.",
	"E-mail fields do not match, please verify.",
	"Invalid address.",
	"You already have a Legacy Worlds account."
);
if ($__err2) {
	$aClass = " error";
} else {
	$aClass = "";
}

?>
 <div class="fsection" id="email">
  <div class='sechd<?= $aClass ?>'><?= $eMail[(int) $__err2] ?></div>
  <div class='formline'>
   <div class="fcol1">
    <span>E-mail:</span>
    <input type='text' name='email' value="<?=htmlq($args['mail'])?>" class="input" />
   </div>
   <div class="fcol2">
    <span>Confirm e-mail:</span>
    <input type='text' name='email2' value="<?=htmlq($args['mail2'])?>" class="input" />
   </div>
  </div>
 </div>
<?php

$ePass = array(
	"4 to 64 characters ; must be different from your username",
	"The password and its confirmation are different.",
	"The password is too short (minimum 4 characters).",
	"The password is too long (maximum 64 characters).",
	"The password must be different from your user name."
);
if ($__err3) {
	$aClass = " error";
} else {
	$aClass = "";
}

?>
 <div class="fsection" id="passwd">
  <div class='sechd<?= $aClass ?>'><?= $ePass[(int) $__err3] ?></div>
  <div class='formline'>
   <div class="fcol1">
    <span>Password:</span>
    <input type='password' name='password' maxlength='64' class="input" />
   </div>
   <div class="fcol2">
    <span>Confirm password:</span>
    <input type='password' name='password2' maxlength='64' class="input" />
   </div>
  </div>
 </div>
<?
$ePlanet = array(
	"Valid characters: letters, numbers, spaces, and _@.+'/",
	"The planet name is too long.",
	"The planet name contains invalid characters.<br/>Use letters, numbers, spaces and _.@-+'/ only).",
	"The planet name must not contain sequences of spaces.",
	"The planet name is too short.",
	"The planet name must contain at least one letter.",
	"This planet name is unavailable.",
	"The planet name must not start or end with spaces."
);
if ($__err4) {
	$aClass = " error";
} else {
	$aClass = "";
}
?>
 <div class="fsection" id="planet">
  <div class='sechd<?= $aClass ?>'><?= $ePlanet[(int) $__err4] ?></div>
  <div class='formline'>
   <div class="fcol1">
    <span>Planet name:</span>
    <input type='text' name='planet' maxlength='15' value="<?=htmlq($args['planetname'])?>" class="input" />
   </div>
  </div>
 </div>
 <div class="fsection" id="confirm">
  <div class="formline"><input type='submit' name='create' value='Create account' class="input" /></div>
 </div>
</div></form>
<? $this->endContents(); ?>

<form action="?" method="POST">
 <input type="hidden" name="c" value="a" />
 <h2>Select an account</h2>
 <p>
  Name of the account to manage:
  <input type="text" name="an" value="<?=utf8entities($data['account'])?>" size="17" maxlength="16" />
  <input name="sa" value="Select account" type="submit" />
<?php

if ($data['notfound']) {
	print "<br/><span style='color:red; font-weight: bold'>Account not found.</span>";
} elseif ($data['admin']) {
?>
  <br/><span style='color:red; font-weight: bold'>Administrative account, no changes allowed.</span>
<?php
} elseif ($data['account'] != '') {
?>
 </p>
 <h2>Manage account</h2>
 <p>
  Account status: <b><?=$data['status']?></b>
 </p>
 <p>
  Account email address:
  <input type="text" name="ma" value="<?=utf8entities($data['email'])?>" size="32" maxlength="128" />
  <input type="submit" name="mma" value="Change mail address" />
<?
switch ($data['mailerr']) :
	case -1: print "<br/><span style='color:green; font-weight: bold'>Address changed</span>"; break;
	case 1: print "<br/><span style='color:red; font-weight: bold'>Invalid address</span>"; break;
	case 2: print "<br/><span style='color:red; font-weight: bold'>Address already used by account "
			. utf8entities($data['oacc']) . "</span>"; break;
	case 3: print "<br/><span style='color:red; font-weight: bold'>Database error</span>"; break;
endswitch;
?>
 </p>
 <p>
  Account password:
  <input type="text" name="pw" value="<?=utf8entities($data['password'])?>" size="32" maxlength="64" />
  <input type="submit" name="mpw" value="Change password" />
<?
switch ($data['passerr']) :
	case -1: print "<br/><span style='color:green; font-weight: bold'>Password updated</span>"; break;
	case 1: print "<br/><span style='color:red; font-weight: bold'>Password too short (min 4 characters)</span>"; break;
	case 2: print "<br/><span style='color:red; font-weight: bold'>Database error</span>"; break;
endswitch;
}

?>
 </p>
<?php

if (!is_null($data['conf_code'])) {
?>
 <p>
  Confirmation code: <i><?=$data['conf_code']?></i>
 </p>
<?php
}
?>
</form>

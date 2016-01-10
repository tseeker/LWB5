<?
include("menu.en.inc");
$this->title = "Cancel account deletion";
$this->addStylesheet("account");
$this->startContents();
?>
<div id="tabs">&nbsp;</div>
<div class="acctab">
 <h1>Do you want to keep your account?</h1>
 <form action="?#acc-quit" method="post"><div>
  <input type="hidden" name="crqc" value="1" />
  <p>
   Please choose whether you want to cancel the countdown for your account's deletion or keep it going.
  </p>
 
  <input type="submit" value="Yes, cancel the countdown" class="input" />
  <input type="submit" name="cancel" value="No, get on with it" class="input" />
 </div></form>
</div>
<? $this->endContents(); ?>

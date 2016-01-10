<?
include("menu.en.inc");
$this->title = "Cancel vacation mode";
$this->addStylesheet("account");
$this->startContents();
?>
<div id="tabs">&nbsp;</div>
<div class="acctab">
 <h1>You are about to cancel your request for vacation mode</h1>
 <p>
  Your account will no longer enter vacation mode at the indicated time and re-entering vacation mode after that will take another 24 to 30 hours.
 </p>
 <p>
  Are you sure you want to do that?
 </p>
 <form method="post" action="?#acc-vacation"><div>
  <input type="hidden" name="cvmsc" value="1" />
  <input type="submit" value="Yes, cancel vacation countdown" class="input" />
  <input type="submit" name="cancel" value="No, continue with the countdown" class="input" />
 </div></form>
</div>
<? $this->endContents(); ?>

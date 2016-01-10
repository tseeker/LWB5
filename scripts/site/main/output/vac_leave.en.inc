<?
include("menu.en.inc");
$this->title = "Leave vacation mode";
$this->addStylesheet("account");
$this->startContents();
?>
<div id="tabs">&nbsp;</div>
<div class="acctab">
 <h1>You are about to leave vacation mode</h1>
 <p>
  You will be able to play the game again.<br/>
  However, you will not be able to enter vacation mode for the next seven days.
 </p>
 <p>
  Are you sure you want to do that?
 </p>
 <form method="post" action="?#acc-vacation"><div>
  <input type="hidden" name="evmc" value="1" />
  <input type="submit" value="Yes, exit vacation mode" class="input" />
  <input type="submit" name="cancel" value="No, stay in vacation mode" class="input" />
 </div></form>
</div>
<? $this->endContents(); ?>

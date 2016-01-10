<?
$this->addStylesheet("sidebox");
$this->addScript("sidebox");
?>
<div id="sbox">
 <ul id="slbox">
  <li class="slcol" id="slleft">
   <span>Manual</span>
   <div id="slcontent">
    <?= $sbContents ?>
   </div>
  </li>
  <li class="slcol" id="slright">
   <span>Menu</span>
   <div>
    <? include("menu.en.inc"); ?>
   </div>
  </li>
 </ul>
</div>
<div id="cframe">

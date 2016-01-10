<?php
if (is_array($args)) {
?>
<form action="?" method="post" onSubmit="return false;"><table cellspacing='0' cellpadding='0'>
 <tr>
  <td id='mkppsel'>&nbsp;</td>
  <td style="width:5%;vertical-align:top"><a href="manual?p=marketplace">Help</a></td>
 </tr>
 <tr><td id='mkppage' colspan="2">
  &nbsp;
 </td></tr>
</table></form>
<div id='mkpfpage' style="display: none; visibility: hidden;"><?=$args['page']?></div>
<div id='mkpinit' style="display: none; visibility: hidden;"><?=$args['pdata']?></div>
<?php
} else {
	echo "<center><br/>Your account has been created too recently for you to use this feature.<br/>You must wait for $args more day".($args>1?"s":"").".</center>";
}
?>

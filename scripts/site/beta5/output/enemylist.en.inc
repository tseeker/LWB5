<form action="?" onSubmit="return false"><table cellspacing="0" cellpadding="0">
 <tr>
  <td class="pc5">&nbsp;</td>
  <td class="pc45">
   <h1>Player Enemy List</h1>
   <p>
    New enemy:
    <input type="text" <?=tooltip('Use this textfield to type in the name of the enemy player to add to your list')?> name="nepl" id="nepl" value="" size="16" maxlength="15" />
    <input type="button" <?=tooltip('Click here to add this player to your enemy list')?> name="addpl" value="Add" onClick="addPlayer();return false" />
   </p>
   <div id="epldiv"></div>
  </td>
  <td class="pc45">
   <h1>Alliance Enemy List</h1>
   <p>
    New enemy:
    <input type="text" <?=tooltip('Use this textfield to type in the tag of the enemy alliance to add to your list')?> name="neal" id="neal" value="" size="6" maxlength="5" />
    <input type="button" <?=tooltip('Click here to add this alliance to your enemy list')?>  name="addal" value="Add" onClick="addAlliance();return false" />
   </p>
   <div id="ealdiv"></div>
  </td>
  <td class="pc5" style="vertical-align:top"><a href="manual?p=enemies">Help</a></td>
 </tr>
</table></form>
<div id='elinit' style="display: none; visibility: hidden;"><?=$args?></div>

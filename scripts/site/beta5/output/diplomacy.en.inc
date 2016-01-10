  <table>
   <tr>
    <td class='div2'>
     <h1>Alliance</h1>
      <div id="alliance"></div>
    </td>
    <td class='pc45'>
     <h1>Scientific Assistance</h1>
      <p id="rsass">&nbsp;</p>
     <h1>Allies &amp; Enemies</h1>
      <p>
       <span id="allies">&nbsp;</span><br/>
       <a href="allies"<?=tooltip('Click here to go to the trusted allies management page')?>>Manage allies</a> - <a href="enemylist" <?=tooltip('Click here to go to the enemies management page')?>>Manage enemies</a>
      </p>
     <h1>Private Messages</h1>
      <p>
       <a href="message?a=f&f=I" <?=tooltip('Click here to go to your inbox in the messaging system')?>>Messages</a>: <b id="pm">&nbsp;</b> (<b id="pmn">&nbsp;</b> new)<br/>
       <a href="message?a=f&f=T" <?=tooltip('Click here to go to the internal transmissions folder of the messaging system')?>>Internal Transmissions</a>: <b id="it">&nbsp;</b> (<b id="itn">&nbsp;</b> new)<br/>
       <a href="message?a=c" <?=tooltip('Click here to go to the new message form and prepare a new message to be sent')?>>Compose</a> a message
      </p>
    </td>
    <td style="vertical-align:top"><a href="manual?p=diplomacy_page">Help</a></td>
   </tr>
  </table>
<div id='dinit' style="display: none; visibility: hidden;"><?=$args?></div>

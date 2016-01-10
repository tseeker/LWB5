<table><tr>
 <td class='div2'>
  <h1>Private Messages</h1>
    <p><a href='message?a=c' <?=tooltip('Click here to go to the form to compose a new private message')?>>Compose Message</a></p>
  <h2>Default Folders</h2>
    <p>
     <a href='message?a=f&f=I' <?=tooltip('Click here to go to your inbox folder')?>>Inbox</a>: <span id="msg0"> </span><br/>
     <a href='message?a=f&f=T' <?=tooltip('Click here to go to your internal transmissions folder')?>>Internal Transmissions</a>: <span id="msg1"> </span><br/>
     <a href='message?a=f&f=O' <?=tooltip('Click here to go to your outbox folder')?>>Outbox</a>: <span id="msg2"> </span>
    </p>
   <h2>Custom Folders</h2>
    <p>
     <span id='cflist'></span><br/>
     <a href='message?a=mf' <?=tooltip('Click here to go to the custom folders management page')?>>Manage Custom Folders</a>
    </p>
 </td><td style="width:45%">
  <h1>Forums</h1>
   <h2>General forums</h2>
    <p id="gforums"></p>
   <div id="aforums"></div>
 </td><td>
  <a href="manual?p=communications_page">Help</a>
 </td>
</tr></table>
<div style="display:none;visibility:hidden"><form action="?" onsubmit="return false"><textarea id="init-data"><?=$args?></textarea></form></div>

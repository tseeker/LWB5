<?php

set_magic_quotes_runtime(false);

session_start();

?>
<html>
 <head>
  <title>Legacy Worlds Beta 5 > Administration > Create game</title>
 </head>
 <body style="background-color: #dfdfff">
  <table style="margin: 50px 0; padding: 0px;width:100%" cellspacing="0" cellpadding="0">
   <tr>
    <td width="33%">&nbsp;</td>
    <td width="34%"><table style="border: 4px solid blue; background-color:white; padding: 5px;width: 100%">
     <tr>
      <td colspan="3" style="padding: 2px 0px;font-size:150%;color:blue;text-align:center">LegacyWorlds Beta 5</td>
     </tr>
     <tr>
      <td colspan="3" style="padding: 2px 0px;font-size:150%;color:red;text-align:center"><?=$_SESSION['lw_new_game']['name']?></td>
     </tr>
     <tr><td colspan="3" style="height:5px;font-size:1pt">&nbsp;</td></tr>
     <tr><td colspan="3" style="padding: 2px 0px;color:#3f3fff;text-align:center">Game creation complete!</td></tr>
     <tr>
      <td style="width:10%">&nbsp;</td>
      <td><table style="border: 1px solid black; padding: 1px; margin: 0px; width: 100%; height: 16px"><tr>
       <td style='font-size:1pt;background-color:#00007f'>&nbsp;</td>
      </tr></table></td>
      <td style="width:10%">&nbsp;</td>
     </tr>
     <tr><td colspan="3" style="padding: 2px 0px;color:#ff3f3f;text-align:center">100% complete</td></tr>
     <tr><td colspan="3">&nbsp;</td></tr>
    </table></td>
    <td width="33%">&nbsp;</td>
    <tr><td colspan="3">&nbsp;</td></tr>
    <tr>
     <td>&nbsp;</td>
     <td style="text-align:center">
      The new game has been created; however, it is not visible from the interface yet, you will have to
      <a href="game_status.php">make it visible</a><br/>
      <br/>
      The server is still in <a href="maintenance.php">maintenance mode</a>, you will have to disable it manually.<br/>
      <br/>
      <a href="index.html">Main admin page</a>
     </td>
     <td>&nbsp;</td>
    </tr>
  </table>
 </body>
</html>
<?php

$_SESSION['lw_new_game']['started'] = false;

?>

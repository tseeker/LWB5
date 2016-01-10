<html>

<head>
  <title>Legacy: Player Rankings</title>
  <link rel='stylesheet' type='text/css' href='red.css' title='Wibble'>
</head>

<body>
<body bgcolor="#333333" text="#COCOCO" link="#CC33FF" vlink="#CC33FF" background="images/background.jpg">
<br>
<?php
  $db=mysql_connect("localhost","chris1","")
        or die("could not connect:".mysql_error);

   mysql_select_db("Legacy",$db);
$GetPlayerList = mysql_query("SELECT `playerName`, `rank` FROM `player` WHERE `admin` != 'Y' AND NOT multi ORDER BY `rank` DESC");
$Rank = '0';

echo "<h3 align='center'>Legacy Player Rankings</h3><table align='center'>";
echo "<th>Rank</th><th>Player</th><th>Points</th>";
while ($PlayerList = mysql_fetch_array($GetPlayerList)){
	$Name = $PlayerList['playerName'];
	$Rank ++;
    $Points = $PlayerList['rank'];
    echo "<tr><td>$Rank </td><td><B><em>$Name</em></B></td><td>$Points</td>";

}
echo "</table>";


?>

</body>

</html>

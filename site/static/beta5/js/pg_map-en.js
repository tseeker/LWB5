var	uncharted = '(uncharted)';
var	dirs = ['Up', 'Down', 'Left', 'Right'];
var	hdNames = ['Coordinates', 'Planet', 'Tag', 'Opacity'];
var	lcHeader = ['Protected', null, 'Target'];
var	yesNo = ['Yes', 'No'];

function	makeMapTooltips()
{
	maptt = new Array();
	if	(ttDelay == 0)
	{
		for	(i=0;i<31;i++)
			maptt[i] = "";
		return;
	}
	maptt[0] = tt_Dynamic("Use this drop down list to change the information displayed on the map");
	maptt[1] = tt_Dynamic("Use this radio button to center the map on the typed in coordinates");
	maptt[2] = tt_Dynamic("Use this text field to enter the horizontal coordinate you want the map to be centered on.");
	maptt[3] = tt_Dynamic("use this text field to enter the vertical coordinate you want the map to be centered on.");
	maptt[4] = tt_Dynamic("Use this drop down list to select the size of the grid in which the map is displayed.");
	maptt[5] = tt_Dynamic("Use this radio button to center the map on chosen own planet");
	maptt[6] = tt_Dynamic("Use this drop down list to select the planet you own to use to center the map");
	maptt[7] = tt_Dynamic("Click here to update the display according to the provided parameters");
	maptt[8] = tt_Dynamic("Use this radio button to center the map on chosen planet");
	maptt[9] = tt_Dynamic("Use this text field to enter the name of a planet to search for; if it exists the map will be centered on the system in which it is located.");
	maptt[10] = tt_Dynamic("Click this button to move the display towards positive vertical coordinates.");
	maptt[11] = tt_Dynamic("Click this button to move the display towards negative horizontal coordinates.");
	maptt[12] = tt_Dynamic("Click this button to move the display towards positive horizontal coordinates.");
	maptt[13] = tt_Dynamic("Click this button to move the display towards negative vertical coordinates.");
	maptt[20] = tt_Dynamic("Click here to order the list according to this field and switch between descending and ascending order");
	maptt[30] = tt_Dynamic("Click here to go to the individual page of this planet");
}


function	drawMapControls()
{
	var	str  = '<form action="?" onSubmit="return false"><table class="mapctrl">';
	var	i;

	str += '<tr><th>Display:</th><td><select name="mtype" ' + maptt[0] + 'onChange="setMode(options[selectedIndex].value)">';
	var	modes = ['planet map', 'alliance map', 'listing'];
	for	(i=0;i<modes.length;i++)
		str += '<option value="'+i+'"'+(map.mode == i ? ' selected="selected"' : '')+'>'+modes[i]+'</option>';
	str += '</td><th>Centre on</th><td onClick="centreMode=0;document.getElementById(\'cm0\').checked=true">';
	str += '<input type="radio" ' + maptt[1] + ' name="cm" id="cm0" value="0" onClick="centreMode=0" /> ';
	str += 'coordinates (<input type="text" ' + maptt[2] + 'name="cx" id="cx" value="'+map.cx+'" size="4" />,<input type="text" ' + maptt[3] + ' name="cy" id="cy"';
	str += ' value="'+map.cy+'" size="4" />)</td><th colspan="4" id="mcttl">Map colors</th></tr>';

	str += '<tr><th>Grid size:</th><td><select ' + maptt[4] + ' name="gsize" onChange="setSize(options[selectedIndex].value)">';
	for	(i=1;i<=7;i++)
		str += '<option value="'+i+'"'+(map.size==i?' selected="selected"':'')+'>'+i+'x'+i+'</option>';
	str += '</select></td><th>or</th><td onClick="centreMode=1;document.getElementById(\'cm1\').checked=true">';
	str += '<input type="radio" ' + maptt[5] + ' name="cm" id="cm1" value="1" onClick="centreMode=1" '+(centreOnOwn!=0?' checked="checked"':'')+'/> ';
	str += 'own planet <span id="plpl"><select name="plpl" ' + maptt[6] + ' onChange="centreOnOwn=options[selectedIndex].value">';
	str += '<option value="0">-----</option>';
	for	(i=0;i<plPlanets.length;i++)
	{
		str += '<option value="'+plPlanets[i].id+'"'+(plPlanets[i].id==centreOnOwn?' selected="selected"':'')+'>';
		str += plPlanets[i].name.replace(/&/g,'&amp;').replace(/>/g,'&gt;').replace(/</g,'&lt;') + '</option>';
	}
	str += '<td class="mcolors" id="neb1">Nebula 1</td><td class="mcolors" id="neb2">Nebula 2</td>';
	str += '<td class="mcolors" id="neb3">Nebula 3</td><td class="mcolors" id="neb4">Nebula 4</td></tr>';

	str += '<tr><td>&nbsp;</td><td><input ' + maptt[7] + ' type="button" onClick="manualUpdate();return false" name="mupd" value="Update Display" /></td>';
	str += '<th>or</th><td onClick="centreMode=2;document.getElementById(\'cm2\').checked=true">';
	str += '<input type="radio" ' + maptt[8] + ' name="cm" id="cm2" value="2" onClick="centreMode=2" '+(centreOnOwn==0?' checked="checked"':'')+'/> ';
	str += 'planet <input type="text" ' + maptt[9] + ' name="cp" id="cp" value="'+centreOn.replace(/"/g, '&quot;')+'" size="16" maxlength="15" /></td>';
	str += '</select></span></td><td class="mcolors" id="mcown">Own</td><td class="mcolors" id="mcally">Allied</td>';
	str += '<td class="mcolors" id="mcother">Other</td>'
	switch (gameType) {
		case '0': str += '<td class="mcolors" style="background-color:#00274f">Protected</td>'; break;
		case '1': str += '<td>&nbsp;</td>'; break;
		case '2': str += '<td class="mcolors" style="background-color:#00274f">Target</td>'; break;
	}
	str += '</tr>';

	str += '</table></form>';
	document.getElementById('jsctrls').innerHTML = str;
}

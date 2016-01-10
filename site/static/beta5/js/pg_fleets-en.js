var	actionText = ['Please select at least one fleet.', 'Rename', 'Change orders', 'Switch status', 'Split', 'Merge', 'Sell', 'Disband', 'View trajectory', 'Clear selection', 'Cancel sale', 'Sale details'];
var	statusText = ['Attack', 'Defense', 'Avail.', 'Unavail.', 'On sale', 'Sold', ' (bundled)'];
var	sectionTitles = ['Own planets', 'Allied planets', 'Other planets', 'Moving fleets', 'Fleets standing by'];
var	noFleetsFound = 'No fleets were found.';
var	foSelHeader = '<th class="fown">Owner</th><th class="fname">Name</th><th class="fhs">Haul</th><th class="fshp">Ships (G/F/C/B)</th><th class="fpwr">Power</th><th class="ftraj">Trajectory</th>';
var	foAvaHeader = '<th class="fown">Owner</th><th class="fname">Name</th><th class="fhs">Haul</th><th class="fshp">Ships (G/F/C/B)</th><th class="fpwr">Power</th><th class="fco">Current Orders</th>';
var	curOrdersTxt = ['Defending ', 'Attacking ', '', '', 'Moving to ', 'Moving to ', ' for defense', ' for attack', 'Standing by in Hyperspace to defend ', 'Standing by in Hyperspace to attack ', '', ''];
var	noSelection = 'no destination selected';
var	smapCoordTxt = "System at coordinates ";
var	dirTxt = ['Up', 'Left', 'Right', 'Down'];
var	unchartedTxt = "(uncharted)";
var	orbitTypes = ['Planet', 'Planetary remains', 'Class 1 nebula', 'Class 2 nebula', 'Class 3 nebula', 'Class 4 nebula'];
var	salesText = ['Target player:', 'Price:', 'Minimum bid:'];


function	drawMainLayout()
{
	var	i, str = '<form action="?" onSubmit="return false"><table class="fctrl" cellspacing="0" cellpadding="0">';

	// Locations
	str += '<tr><td>Locations:</td><td><select name="lLoc" id="lLoc" onChange="changeFilters(\'listLocations\', this.options[this.selectedIndex].value)">';
	var	locs = ['All planets', 'Own planets', 'Allied planets', 'Other planets'];
	for	(i=0;i<locs.length;i++)
		str += '<option value="'+i+'"'+(listLocations == i ? ' selected="selected"' : '')+'>'+locs[i]+'</option>';
	str += '</select></td>';

	// Modes
	str += '<td>Fleet mode:</td><td><select name="lMode" id="lMode" onChange="changeFilters(\'listMode\', this.options[this.selectedIndex].value)">';
	var	fmd = ['Any', 'Defending', 'Attacking'];
	for	(i=0;i<fmd.length;i++)
		str += '<option value="'+i+'"'+(listMode == i ? ' selected="selected"' : '')+'>'+fmd[i]+'</option>';
	str += '</select></td></tr>';

	// Current status
	str += '<tr><td>Fleet status:</td><td><select name="fMode" id="fMode" onChange="changeFilters(\'fDispMode\', this.options[this.selectedIndex].value)">';
	var	fst = ['Any', 'Idle', 'Unavailable', 'Moving', 'H.S. Stand-by', 'On sale', 'Sold'];
	for	(i=0;i<fst.length;i++)
		str += '<option value="'+i+'"'+(fDispMode == i ? ' selected="selected"' : '')+'>'+fst[i]+'</option>';
	str += '</select></td>';

	// Allies mode
	str += '<td>Fleet owner:</td><td><select name="aMode" id="aMode" onChange="changeFilters(\'alliesMode\', this.options[this.selectedIndex].value)">';
	var	alm = ['Any', 'Myself', 'Trusted Allies', 'Others'];
	for	(i=0;i<alm.length;i++)
		str += '<option value="'+i+'"'+(alliesMode == i ? ' selected="selected"' : '')+'>'+alm[i]+'</option>';
	str += '</select></td></tr></table>';

	// Search engine
	str += '<table class="fsearch"><tr><td>';
	str += 'Search for <select name="sType" id="sType" onChange="changeFilters(\'sType\', this.options[this.selectedIndex].value)">';
	var	stp = ['fleet', 'location', 'player'];
	for	(i=0;i<stp.length;i++)
		str += '<option value="'+i+'"'+(sType == i ? ' selected="selected"' : '')+'>'+stp[i]+'</option>';
	var	c = "changeFilters('sText', this.value)";
	str += '</select> <input type="text" name="sText" id="sText" value="" onKeyUp="'+c+'" onClick="'+c+'" />';
	str += '</td></tr></table>';
	
	// Main display
	str += '<hr/><div id="fmain">&nbsp;</div>';

	// Commands
	str += '<div class="factions" id="faframe"><table class="factions" cellpadding="0" cellspacing="0"><tr><th>Actions:</th>';
	str += '<td id="factions">&nbsp;</td><td style="width:5%"><a href="manual?p=fleets_page">Help</a></tr></table></div>';

	str += '</form>';
	document.getElementById('fpage').innerHTML = str;
	document.getElementById('sText').value = sText;
}


function drawMovingHeader(hasSel) {
	var str;
	str  = '<table class="mfltl"><tr>';
	if (hasSel) {
		str += '<th class="fsel" id="f-move-sel" rowspan="2">&nbsp;</th>';
	}
	str += '<th class="fown" rowspan="2">Owner</th><th class="fname" rowspan="2">Name</th>';
	str += '<th class="fhs">Haul</th><th class="fshp">GA Ships</th>';
	str += '<th class="fshp">Fighters</th><th class="fshp">Cruisers</th><th class="fshp">Battle Cruisers</th>';
	str += '<th class="fpwr">Power</th><th class="fstat">Status</th></tr>';
	str += '<tr><th class="floc" colspan="2">Current Location</th><th class="fdest" colspan="2">Destination</th>';
	str += '<th class="feta">E.T.A.</th><th class="fstd" colspan="2">H.S. Standby</th></tr></table>';
	return	str;
}


function drawWaitingHeader(hasSel) {
	var str;
	str  = '<table class="wfltl"><tr>';
	if (hasSel) {
		str += '<th class="fsel" id="f-hssb-sel" rowspan="2">&nbsp;</th>';
	}
	str += '<th class="fown" rowspan="2">Owner</th><th class="fname" rowspan="2">Name</th>';
	str += '<th class="fhs">Haul</th><th class="fshp">GA Ships</th>';
	str += '<th class="fshp">Fighters</th><th class="fshp">Cruisers</th><th class="fshp">Battle Cruisers</th>';
	str += '<th class="fpwr">Power</th><th class="fstat">Status</th></tr>';
	str += '<tr><th class="floc" colspan="2">Location</th><th class="ftime" colspan="2">Time Spent</th>';
	str += '<th class="ftime" colspan="2">Time Left</th><th class="floss">Loss Prob.</th></tr></table>';
	return	str;
}


function drawPlanetBox(planet) {
	var	str = '<table class="planet"><tr class="phdr">';
	var	so = (planet.fleetLocation.details && planet.fleetLocation.details.owner != '' && planet.fleetLocation.details.owner != myself.id);
	var	apow = 0, dpow = 0;

	str += "<td class='pimg'><img src='" + staticurl + '/beta5/pics/';
	if (planet.fleetLocation.opacity == 0) {
		str += 'pl/s/'+planet.id;
	} else if (planet.fleetLocation.opacity == 1) {
		str += 'prem_s';
	} else {
		str += 'nebula' + (planet.fleetLocation.opacity-1);
	}
	str += ".png' class='pimg' alt='[P]' /></td><td class='name'><a href='planet?id=";
	str += planet.id + "'>" + planet.fleetLocation.name + '</a>';
	str += " (<b>" + planet.fleetLocation.x + ',' + planet.fleetLocation.y + '</b>,' + planet.fleetLocation.orbit + ')';
	if (planet.fleetLocation.details && planet.fleetLocation.details.tag != '') {
		str += ' [<b>' + planet.fleetLocation.details.tag + '</b>]';
	}
	str += '</td>';
	if (so) {
		str += '<td class="pinf"><a href="message?a=c&ct=0&id='+planet.fleetLocation.details.owner+'">';
		str += players.get(planet.fleetLocation.details.owner) + '</a></td>';
	}
	str += "<td class='pinf'";
	if (planet.fleetLocation.details) {
		with(planet.fleetLocation.details) {
			str += '>Population: <b>' + formatNumber(pop) + 'M</b></td><td class="pinf">';
			if (turrets != 0) {
				str += '<b>' + formatNumber(turrets) + '</b> turret' + (turrets > 1 ? 's' : '');
				str += '; power: <b>' + formatNumber(tPower) + '</b>';
				dpow += parseInt(tPower, 10);
			} else {
				str += 'No turrets';
			}
		}
	} else {
		str += ' colspan="2">' + (planet.fleetLocation.opacity == 1 ? 'Planetary remains' : ('Class ' + (planet.fleetLocation.opacity - 1) + ' nebula'));
	}
	str += '</td></tr><tr><td colspan="'+(so?'5':'4')+'" class="';

	if (planet.fleets.length) {
		planet.fleetLocation.selectableFleets = planet.fleetLocation.selectedFleets = 0;
		var	j, f, om = -1;

		str += 'flt"><table class="fltl"><tr><th class="fsel" id="fsel-p-' + planet.id + '">&nbsp;</th>';
		str += '<th class="fown">Owner</th><th class="fname">Name</th><th class="fhs">Haul</th><th class="fshp">GA Ships</th>';
		str += '<th class="fshp">Fighters</th><th class="fshp">Cruisers</th><th class="fshp">Battle Cruisers</th>';
		str += '<th class="fpwr">Power</th><th class="fstat">Status</th></tr>';
		planet.fleets.sort(sortPlanetFleets);
		for (j=0;j<planet.fleets.length;j++) {
			f = fleets.get(planet.fleets[j]);
			if (f.owner == myself.id) {
				om = f.mode;
			}
		}
		if (om == -1) {
			for (j=0;j<planet.fleets.length;j++) {
				f = fleets.get(planet.fleets[j]);
				if	(allies.containsKey(f.owner))
					om = f.mode;
			}
		}
		if (om == -1) {
			om = 0;
		}
		for (j=0;j<planet.fleets.length;j++) {
			f = fleets.get(planet.fleets[j]);
			if (f.mode == 0) {
				dpow += parseInt(f.power,10);
			} else {
				apow += parseInt(f.power,10);
			}
			str += drawFleetLine(f, f.owner == myself.id ? 'own' : (f.mode == om ? 'ally': 'enemy'),true);
		}
		str += '</table>';

		if (planet.fleetLocation.details) {
			var	act = new Array(), ang = new Array();
			f = allies.keys();
			if (!planet.fleetLocation.details.vacation) {
				for (j=0;j<f.length;j++) {
					var	a = allies.get(f[j]), og;
					if (!a.gasAt.containsKey(planet.id)) {
						continue;
					}
					og = a.gasAt.get(planet.id);
					if (og * a.gaPop >= planet.fleetLocation.details.pop) {
						act.push(a);
					} else if (og > 0) {
						var ep = parseInt(planet.fleetLocation.details.pop, 10) - og * a.gaPop;
						ep = Math.ceil(ep / a.gaPop);
						ang.push([a,ep.toString()]);
					}
				}
			}
			if (act.length || ang.length || apow != 0 || planet.fleetLocation.details.vacation
					|| planet.fleetLocation.details.protection) {
				str += '</td></tr><tr><td colspan="'+(so?'5':'4')+'" class="takers">';
				if (act.length) {
					for (j=0;j<act.length;j++) {
						if (j != 0) {
							str += (j == act.length - 1) ? ' or ' : ', ';
						}
						str += act[j].getName();
					}
					str += ' could take the planet.';
					if (ang.length || apow != 0) {
						str += '<br/>';
					}
				}
				if (ang.length) {
					for (j=0;j<ang.length;j++) {
						str += ang[j][0].getName() + ' would need <b>' + formatNumber(ang[j][1]) + '</b> more GA Ship'
							+ (ang[j][1] > 1 ? 's' : '') + ' to take this planet.';
						if (j != ang.length - 1) {
							str += '<br/>';
						}
					}
					if (apow != 0) {
						str += '<br/>';
					}
				}
				if (apow != 0) {
					str += 'Battle status: <b class="own">' + formatNumber((om == 0 ? dpow : apow).toString())
						+ '</b> vs. <b class="enemy">' + formatNumber((om == 1 ? dpow : apow).toString()) + '</b>';
					if (planet.fleetLocation.details.vacation || planet.fleetLocation.details.protection) {
						str += '<br/>';
					}
				}
				if (planet.fleetLocation.details.protection) {
					str += '<b class="enemy">Planet is under protection!</b>';
				} else if (planet.fleetLocation.details.vacation) {
					str += '<b class="enemy">Planet is on vacation!</b>';
				}
			}
		}
		str += '</table>';
	} else {
		str += 'noflt">No fleets found';
	}

	str += '</td></tr></table>';
	return	str;
}


function	getNewFleetName(p)
{
	return	prompt("Please type the new name for th"+(p?"ese fleets":"is fleet")+".", "");
}

function alertFleetName(e) {
	var	str = 'Error: ';
	switch	(e)
	{
	case	0:
		str += 'please specify a name';
		break;
	case	1:
		str += 'this name is too long';
		break;
	case	2:
		str += 'this name is too short';
		break;
	case	3:
		str += 'a fleet was not found';
		break;
	case 200:
		str += 'you can\'t rename a fleet while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}

	alert(str + '.');
}


function	getMergedFleetName()
{
	return	prompt("Please type the name of the merged fleet. Leave blank to keep\nits former name.\n", '');
}

function alertFleetMerge(e) {
	var	str = 'Error: ';
	switch	(e)
	{
	case	0:
		str += 'this name is too long';
		break;
	case	1:
		str += 'this name is too short';
		break;
	case	2:
		str += 'a fleet was not found';
		break;
	case 200:
		str += 'you can\'t merge fleets while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}

	alert(str + '.');
}

function alertFleetSwitch(e) {
	var	str = 'Error: ';
	switch (e) {
	case	0:
		str += 'you no longer have control over this fleet';
		break;
	case	1:
		str += 'you can\'t switch to attack mode on a planet you own';
		break;
	case	2:
		str += 'you (or your alliance) are in the planet owner\'s enemy list';
		break;
	    case 3:
		str += 'the Peacekeepers will not let you get away with that so easily';
		break;
	case 200:
		str += 'you can\'t switch a fleet\'s status while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}

	alert(str + '.');
}

function alertFleetDisband(e) {
	var	str = 'Error: ';
	switch (e) {
	case	0:
		str += 'you don\'t have control over this fleet';
		break;
	case	1:
		str += 'the selected fleet(s) no longer exist';
		break;
	case	2:
		str += 'not enough cash to cancel the fleet\'s sale';
		break;
	case 200:
		str += 'you can\'t disband fleets while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}

	alert(str + '.');
}

function alertFleetSplit(e) {
	var	str = 'Error: ';
	switch (e) {
	case 0:
		str += 'you no longer have control over this fleet';
		break;
	case 1:
		str += 'you are trying to cheat, aren\'t you? If not, contact us';
		break;
	case 2: case 5:
		str += 'the fleet you tried to split is no longer available';
		break;
	case 3: case 4: case 7:
		str += 'the fleet\'s size has changed in the meantime';
		break;
	case 10:
		str += 'the new fleets\' name is too short';
		break;
	case 11:
		str += 'the new fleets\' name is too long';
		break;
	case 200:
		str += 'you can\'t split a fleet while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}

	alert(str + '.');
}

function alertFleetOrders(e) {
	var	str = 'Error: ';
	switch (e) {
	case 0:
		str += 'you no longer have control over these fleets';
		break;
	case 1:
		str += 'the destination planet wasn\'t found';
		break;
	case 2:
		str += 'an internal error occured.\nPlease report this to the staff as "setOrders/2"';
		break;
	case 3:
		str += 'the fleets you selected are for sale';
		break;
	case 4:
		str += 'the fleets you selected are no longer available';
		break;
	case 200:
		str += 'you can\'t change fleet orders while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}

	alert(str + '.');
}

function alertServerError() {
	alert('Error: an error has occured on the server.\nPlease contact the staff.');
}

function alertViewSale() {
	alert('Error: this sale is no longer available');
}

function	drawSplitPage()
{
	var	i, str = '<form action="?" onSubmit="return false"><h2>';
	var	f = splitParam[0];

	switch	(f.orders.oType)
	{
	case	0:
		str += 'Split fleet orbitting ' + f.orders.loc.name + '</h2><br/>';
		str += '<table class="planet"><tr><td class="flt"><table class="fltl"><tr>';
		str += '<th class="fown">Owner</th><th class="fname">Name</th><th class="fhs">Haul</th><th class="fshp">GA Ships</th>';
		str += '<th class="fshp">Fighters</th><th class="fshp">Cruisers</th><th class="fshp">Battle Cruisers</th>';
		str += '<th class="fpwr">Power</th><th class="fstat">Status</th></tr>';
		str += drawFleetLine(f, (f.owner == myself.id ? 'own' : 'ally'), false);
		str += '</table></td></tr></table>';
		break;
	case	1:
		str += 'Split moving fleet</h2><br/><table class="planet"><tr><td class="flt">' + drawMovingHeader(false) + '</td></tr>';
		str += drawMovingFleetLine(f, false);
		str += '</table>';
		break;
	case	2:
		str += 'Split waiting fleet</h2><br/><table class="planet"><tr><td class="flt">' + drawWaitingHeader() + '</td></tr>';
		str += drawWaitingFleetLine(f, false);
		str += '</table>';
		break;
	}

	str += '<h2>New fleet(s)</h2>';
	str += '<table class="scmd"><tr><th>Split type:</th><td><label><input type="radio" name="stype" value="0" id="stype0" checked="checked" onClick="setSplitType(0)" />';
	str += 'Manual</label></td></tr><tr><th>&nbsp;</th><td><label><input type="radio" name="stype" value="1" onClick="setSplitType(1)" /> Automatic</label></td></tr>';
	str += '<tr><th>Amount of fleets:</th><td><select name="nfleets" onChange="setSplitCount(parseInt(this.options[this.selectedIndex].value, 10))">';
	for	(i=1;i<11;i++)
		str += '<option value="'+i+'">'+(i+1)+'</option>';
	str += '</select></td></tr><tr><th>New name:</th><td><input type="text" value="" name="nfn" id="nfn" onChange="splitParam[9]=this.value" /></td></tr>';
	str += '<tr><td colspan="2">&nbsp;</td></tr>';
	if	(f.ships[0] > 0)
	{
		var c = 'setSplitShips(0,parseInt(this.value, 10))';
		c = "onKeyUp='"+c+"' onClick='"+c+"'";
		str += '<tr><th>GA Ships:</th><td><input type="text" value="0" id="sgas" name="sgas" size="10"'+c+' /> / ' + formatNumber(f.ships[0]) + '</td></tr>';
	}
	if	(f.ships[1] > 0)
	{
		var c = 'setSplitShips(1,parseInt(this.value, 10))';
		c = " onKeyUp='"+c+"' onClick='"+c+"'";
		str += '<tr><th>Fighters:</th><td><input type="text" value="0" id="sfgt" name="sfgt" size="10"'+c+' /> / ' + formatNumber(f.ships[1]) + '</td></tr>';
	}
	if	(f.ships[2] > 0)
	{
		var c = 'setSplitShips(2,parseInt(this.value, 10))';
		c = " onKeyUp='"+c+"' onClick='"+c+"'";
		str += '<tr><th>Cruisers:</th><td><input type="text" value="0" id="scru" name="scru" size="10"'+c+' /> / ' + formatNumber(f.ships[2]) + '</td></tr>';
	}
	if	(f.ships[3] > 0)
	{
		var c = 'setSplitShips(3,parseInt(this.value, 10))';
		c = " onKeyUp='"+c+"' onClick='"+c+"'";
		str += '<tr><th>Battle Cruisers:</th><td><input type="text" value="0" id="sbcr" name="sbcr" size="10"'+c+' /> / ' + formatNumber(f.ships[3]) + '</td></tr>';
	}
	if	(f.ships[2] > 0 || f.ships[3] > 0)
		str += '<tr><th>Haul used:</th><td id="hused">0</td></tr><tr><th>Haul available:</th><td id="havail">0</td></tr>';

	str += '<tr><td colspan="2">&nbsp;</td></tr><tr><td colspan="2" class="sbut">';
	str += '<input type="submit" onClick="doFleetSplit(); return false" value="Split fleet" id="bsplit" disabled="disabled" />';
	str += '<input type="submit" onClick="cancelSplit(); return false" value="Cancel" /></td></tr>';
	str += '</table>';

	str += '</form>';
	document.getElementById('fpage').innerHTML = str;
}


function	confirmDisband(p)
{
	return	confirm('Are you sure you want to disband ' + (p ? 'these fleets' : 'this fleet') + '?');
}


function	drawOrdersPage()
{
	var	str = '<form action="?" onSubmit="return false">'
			+ '<table><tr><td id="chord"><h1>Change Orders</h1><td id="chordc">&nbsp;</td></tr></table>'
			+ '<h2>New orders</h2><p><span id="moveto">&nbsp;</span><br/><span id="standby">&nbsp;</span><br/>'
			+ '<span id="odmode">&nbsp;</span></p><h2>Selected fleets</h2><div id="oself">&nbsp;</div>'
			+ '<h2>Available fleets</h2><div id="oavaf">&nbsp;</div></form>';
	document.getElementById('fpage').innerHTML = str;
}

function	getMoveToLine()
{
	if	(moveTo == -1)
		return "No destination selected - <a href='#' onClick='setOrdersDestination();return false'>Set destination</a>";

	return "Move to <b>" + moveToLoc.name + "</b> (<b>" + moveToLoc.x + ',' + moveToLoc.y + '</b>,' + moveToLoc.orbit
		+ ") - <a href='#' onClick='setOrdersDestination();return false'>Change destination</a> - "
		+ "<a href='#' onClick='removeOrdersDestination();return false'>Remove destination</a>";
}

function	getStandByLine()
{
	if	(!selCanHS)
	{
		waitTime = -1;
		return	"Selection is not capable of Hyperspace travel";
	}
	if	(waitTime == -1)
		return	"No Hyperspace stand-by orders - <a href='#' onClick='setOrdersTime();return false'>Set delay</a>";
	return	"Selection is scheduled to stand by in Hyperspace for <b>" + waitTime + "</b>h "
		+ " - <a href='#' onClick='setOrdersTime();return false'>Change delay</a>"
		+ " - <a href='#' onClick='cancelOrdersDelay();return false'>Remove delay</a>";
}

function	getModeLine()
{
	var	fd = false;
	if	(orderFleets.length)
	{
		var	i;
		for	(i=0;i<orderFleets.length;i++)
		{
			var	f = fleets.get(orderFleets[i]);
			if	(	moveTo != -1 && moveToLoc.details && f.owner == moveToLoc.details.owner
				||	moveTo == -1 && (f.orders.oType == 1 && f.orders.cur.details && f.orders.cur.details.owner == f.owner)
				||	moveTo == -1 && (f.orders.oType != 1 && f.orders.loc.details && f.orders.loc.details.owner == f.owner)
				)
				return	"Destination is owned by one of the fleets' owners - forcing to defense.";
		}
	}
	return	"Fleet mode: <select onclick='orderMode = parseInt(this.options[this.selectedIndex].value, 10)' "
		+ "name='flmode' id='flmode'><option value='0'>Defense</option>"
		+ "<option value='1' " + (orderMode ? " selected='selected'" : "") + ">Attack</option></select>";
}

function	getOrderLinks()
{
	var	str = "";
	if	(orderFleets.length > 0)
		str += '<a href="#" onClick="setNewOrders();return false">Confirm</a> - ';
	str += '<a href="#" onClick="cancelOrders();return false">Cancel</a>';
	return	str;
}

function	getFleetDelay(current)
{
	return	prompt("Please enter the duration of the fleet's Hyperspace stand by,\nin hours.", current);
}

function	fleetDelayError(e)
{
	var	str = "Error: ";
	switch	(e)
	{
	case	0:
		str += "please type a valid number";
		break;
	case	1:
		str += "the delay must be at least 1 hour";
		break;
	case	2:
		str += "the delay must be at most 48 hours";
		break;
	}
	alert(str);
}


function	drawDestinationSelection()
{
	var	str = '<form onSubmit="return false" action="?">';
	str += '<table class="fseldest"><tr><td><h1>Set Destination</h1>';
	str += '<p>Current selection: <span id="cursel">&nbsp;</span></p><p>';
	str += 'Please use the map on the right to select a new destination.</p>';
	str += '<p class="sdbut"><input type="button" name="sdok" value="Confirm" onClick="confirmSetDestination()" id="sdok" disabled="disabled" />';
	str += ' <input type="button" name="sdcancel" value="Cancel" onClick="cancelSetDestination()" /></p>';
	str += '</td><td id="sdmap">&nbsp;</td></tr></table></form>';
	document.getElementById('fpage').innerHTML = str;
}


function drawMapControls() {
	var	str;

	// Centre map on coords...
	str = '<p>&nbsp;</p><table id="mapctr"><tr>';
	str += '<td><input type="checkbox" name="mrem" id="mcrem" ' + (mapRemember ? 'checked="checked" ':'')
		+ ' onclick="mapRemember = this.checked" /></td><td><label for="mcrem">Remember map location'
		+ '</label></td></tr>';
	str += '<td onClick="document.getElementById(\'mct0\').checked=true">';
	str += '<input type="radio" name="mct" id="mct0" ' + (mapCType==0?'checked="checked" ':'') + ' /></td>';
	str += '<td onClick="document.getElementById(\'mct0\').checked=true"><label for="mct0">Centre on coordinates</label></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="document.getElementById(\'mct0\').checked=true">';
	str += '(<input type="text" name="cx" id="cx" value="'+dMapX+'" size="4" />,<input type="text" name="cy" id="cy"';
	str += ' value="'+dMapY+'" size="4" />)</td></tr>';

	// Centre map on own planet...
	str += '<td onClick="document.getElementById(\'mct1\').checked=true">';
	str += '<input type="radio" name="mct" id="mct1" ' + (mapCType==1?'checked="checked" ':'') + ' /></td>';
	str += '<td onClick="document.getElementById(\'mct1\').checked=true"><label for="mct1">Centre on own/allied planet</label></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="document.getElementById(\'mct1\').checked=true">';
	str += '<select name="mcpo" style="width:100%" id="mcpo"><option value="0">-----</option>';
	var i, l = locations.keys(), mapList = [];
	for (i = 0; i < l.length; i ++) {
		var p = locations.get(l[i]);
		if (!p.details || !p.details.owner) {
			continue;
		}
		mapList.push(l[i]);
	}
	mapList.sort(sortMapPList);
	for (i in mapList) {
		var p = locations.get(mapList[i]);
		str += '<option value="' + p.id + '"'
			+ (mapCType == 1 && p.id == mapParm ? ' selected="selected"' : '') + '>' + p.name
			+ (allies.get(p.details.owner).isMe ? '' : (' (' + players.get(p.details.owner) + ')'))
			+ '</option>';
	}
	str += '</select></td></tr>';

	// Centre map on planet...
	str += '<td onClick="document.getElementById(\'mct2\').checked=true">';
	str += '<input type="radio" name="mct" id="mct2" ' + (mapCType==2?'checked="checked" ':'') + ' /></td>';
	str += '<td onClick="document.getElementById(\'mct2\').checked=true"><label for="mct2">Centre on planet</label></td></tr>';
	str += '<tr><td>&nbsp;</td><td onClick="document.getElementById(\'mct2\').checked=true">';
	str += '<input type="text" style="width:100%" name="pn" id="mcpn" value="' + (mapCType==2?mapParm:'') + '" /></td></tr>';

	str += '</table><p class="mapbt"><input type="button" name="mcup" value="Move" onClick="updateGalacticMap()" /></p>';
	return	str;
}

function	alertMap()
{
	alert('Error: this planet was not found on the map');
}

function	getTrajectoryText(id, t)
{
	if	(!t)
		return	"(loading)";

	var	f = fleets.get(id);
	if	(t.length == 0 && f.orders.oType != 1)
		return	"N/A";

	var	r, i, str = '<select name="tr' + id + '">';
	if	(f.orders.oType == 1)
	{
		r = f.orders.reroute + 10;
		if	(r > 61)
			r = 61;
		str += '<option>Rerouting: ' + r + ' minutes</option>';
	}
	else
		r = 0;
	for	(i=0;i<t.length;i++)
	{
		var	w = t[i];
		str += '<option';
		if	(i == t.length - 1)
			str += ' selected="selected">Arriving at';
		else
			str += '>Leaving';
		var e = parseInt(w.eta,10) + r;
		str += ' ' + w.name + ' ' + w.coords + ': ' + e + ' minutes</option>';
	}
	str += '</select>';

	return	str;
}


function	drawFleetTrajectory()
{
	var	str, f = fleetTrajectory.fleet, o = allies.get(f.owner);
	var	used = f.ships[0] * o.gSpace + f.ships[1] * o.fSpace;
	var	haul = f.ships[2] * o.cHaul + f.ships[3] * o.bHaul;
	var	m = parseInt(f.mode, 10);

	str = '<table><tr><td id="chord"><h1>View Fleet Trajectory ...</h1></td><td id="chordc">'
		+ '<a href="#" onClick="prepareUpdate();x_getFleetsList(parseMainData);return false">'
		+ 'Close</a></td></tr></table><h2>Selected Fleet</h2><table class="oavaf"><tr>' + foAvaHeader
		+ '</tr><tr class="' + (f.owner == myself.id ? 'flown' : 'flally') + '"><td class="fown">'
		+ players.get(f.owner) + '</td><td class="fname">' + f.name + '</td><td class="fhs">';
	if	(haul == 0)
		str += 'N/A';
	else
	{
		var hp = Math.round(used * 100 / haul);
		str += (hp > 200) ? '&gt;200%' : (hp + '%');
	}
	var	op = locations.get(fleetTrajectory.from);
	str += '</td><td class="fshp">' + formatNumber(f.ships[0]) + ' / ' + formatNumber(f.ships[1]) + ' / '
		+ formatNumber(f.ships[2]) + ' / ' + formatNumber(f.ships[3]) + '</td><td class="fpwr">'
		+ formatNumber(f.power) + '</td><td class="fco">' + curOrdersTxt[m+4] + f.orders.to.name
		+ curOrdersTxt[m+6] + '</td></tr></table><h2>Trajectory</h2><p>Fleet moving in '
		+ (fleetTrajectory.hyperspace ? 'Hyperspace' : 'normal space') + '; original point of origin: '
		+ '<a href="planet?id=' + op.id + '">' + op.name + '</a> (<b>' + op.x + ',' + op.y + '</b>,'
		+ op.orbit + ')</p><table class="ftraj"><tr>'
		+ '<th class="tname">Location</th><th class="ttype">Type</th><th class="tstat">'
		+ 'Status</th></tr>';

	var	hc = false;
	if	(fleetTrajectory.changed > 0)
	{
		str += '<tr class="tcur"><td class="tname"><a href="planet?id=' + fleetTrajectory.waypoints[0].id
			+ '">' + fleetTrajectory.waypoints[0].name + '</a> (<b>' + fleetTrajectory.waypoints[0].system
			+ '</b>,' + fleetTrajectory.waypoints[0].orbit + ')</td><td class="ttype"><span class="ot'
			+ fleetTrajectory.waypoints[0].opacity + '">'+ orbitTypes[fleetTrajectory.waypoints[0].opacity]
			+ '</td><td class="tstat">Rerouting (' + fleetTrajectory.changed + ' minute'
			+ (fleetTrajectory.changed > 1 ? 's' : '') + ')</td></tr>';
		hc = true;
	}

	var	i;
	for	(i=0;i<fleetTrajectory.waypoints.length;i++)
	{
		var	wp = fleetTrajectory.waypoints[i];
		str += '<tr';
		if	(!hc && wp.eta < fleetTrajectory.left)
		{
			str += ' class="tcur"';
			hc = true;
		}
		str += '><td class="tname"><a href="planet?id=' + wp.id + '">' + wp.name + '</a> (<b>' + wp.system
			+ '</b>,' + wp.orbit + ')</td><td class="ttype"><span class="ot' + wp.opacity + '">'
			+ orbitTypes[wp.opacity] + '</td><td class="tstat">';
		if	(!hc)
		{
			var	n = wp.eta - fleetTrajectory.left;
			if	(n > 0)
				str += 'Left orbit ' + n + ' minute' + (n>1?'s':'') + ' ago';
			else
				str += 'Just left orbit';
		}
		else if (i < fleetTrajectory.waypoints.length - 1)
		{
			var	n = fleetTrajectory.left + fleetTrajectory.changed - wp.eta;
			str += 'Leaving orbit in ' + n + ' minute' + (n>1?'s':'');
		}
		else
		{
			var	n = fleetTrajectory.left + fleetTrajectory.changed - wp.eta;
			str += 'Reaching destination in ' + n + ' minute' + (n>1?'s':'');
		}
		str += '</td></tr>';
	}

	if	(fleetTrajectory.wait != 0)
	{
		var	wp = fleetTrajectory.waypoints[fleetTrajectory.waypoints.length - 1];
		str += '<tr><td class="tname"><a href="planet?id=' + wp.id + '">' + wp.name + '</a> (<b>' + wp.system
			+ '</b>,' + wp.orbit + ')</td><td class="ttype"><span class="ot' + wp.opacity + '">'
			+ orbitTypes[wp.opacity] + '</td><td class="tstat">Standing by in Hyperspace for '
			+ fleetTrajectory.wait + ' hour' + (fleetTrajectory.wait>1?'s':'') + '</td></tr>';
	}

	str += '</table>';
	document.getElementById('fpage').innerHTML = str;
}


function	drawSellLayout()
{
	var	str;

	str = '<form onSubmit="return false" action="?"><table><tr><td id="chord">'
		+ '<h1>Sell Fleets ...</h1></td><td id="chordc">&nbsp;</td></tr></table>'
		+ '<div id="sflist">&nbsp;</div></form>';

	document.getElementById('fpage').innerHTML = str;
}

function	FleetSale_drawForm()
{with(this){
	var	str;

	str = '<p>&nbsp;</p><h2>Orbitting ' + fleetLocation.name + ' (' + fleetLocation.x + ',' + fleetLocation.y + ',' + fleetLocation.orbit
		+ ')</h2><h3>Selected Fleets</h3><table class="fslist"><tr><th class="fname">Fleet Name</th>'
		+ '<th class="fshp">G.A. Ships</th><th class="fshp">Fighters</th><th class="fshp">Cruisers'
		+ '</th><th class="fshp">Battle Cruisers</th><th class="fpwr">Power</th></tr>';

	var	tot = [0,0,0,0,0];
	for	(var i=0;i<fleets.length;i++)
	{
		str += '<tr><td class="fname">' + fleets[i].name + '</td>';
		for	(var j=0;j<4;j++)
		{
			str += '<td class="fshp">' + formatNumber(fleets[i].ships[j]) + '</td>';
			tot[j] += parseInt(fleets[i].ships[j], 10);
		}
		str += '<td class="fpwr">' + formatNumber(fleets[i].power) + '</td></tr>';
		tot[4] += parseInt(fleets[i].power, 10);
	}
	str += '<tr><th class="ftot">Total</th>';
	for	(i=0;i<5;i++)
		str += '<td class="f' + (i==4 ? 'pwr' : 'shp') + '">' + formatNumber(tot[i].toString()) + '</td>';

	var	smc = ['','','',''];
	smc[mode] = ' checked="checked"';

	str += '</tr></table><h3>Sale details</h3><table cellspacing="0" cellpadding="0"><tr>'
		+ '<td class="div20" rowspan="4">&nbsp;</td><td class="pc45"><input type="radio" name="s'
		+ id + 'mode" id="s' + id + 'm0" value="0" onClick="sales.get(' + id + ').setMode(0);'
		+ 'return true"'+smc[0]+' /> <label for="s' + id + 'm0">Gift</label></td><td class="pc15"><label for="s'
		+ id + 'exp">Offer expires:</label></td><td class="pc30"><select name="s' + id + 'exp" id="s'
		+ id + 'exp" onChange="sales.get(' + id + ').setExpire(this.options[this.selectedIndex].value)">';

	var	expTime = [0, 6, 12, 24, 48, 72, 96, 120],
		expLabel = ['Never', '6 hours', '12 hours', '1 day', '2 days', '3 days', '4 days', '5 days'];
	for	(i=0;i<expTime.length;i++)
		str += '<option value="'+expTime[i]+'">'+expLabel[i]+'</option>';

	str += '</select></td><td class="div20" rowspan="4">&nbsp;</td></tr><tr><td class="pc45"><input type="radio" '
		+ 'name="s'+ id + 'mode" id="s' + id + 'm1" value="1" onClick="sales.get(' + id + ').setMode(1);'
		+ 'return true"'+smc[1]+' /> <label for="s' + id + 'm1">Direct Sale</label></td><td id="s' + id
		+ 'tarl">&nbsp;</td><td id="s' + id + 'tar">&nbsp;</td></tr><tr><td class="pc45"><input type="radio" '
		+ 'name="s'+ id + 'mode" id="s' + id + 'm2" value="2" onClick="sales.get(' + id + ').setMode(2);'
		+ 'return true"'+smc[2]+' /> <label for="s' + id + 'm2">Public Sale</label></td><td id="s' + id
		+ 'pril">&nbsp;</td><td id="s' + id + 'pri">&nbsp;</td></tr><tr><td class="pc45"><input type="radio" '
		+ 'name="s'+ id + 'mode" id="s' + id + 'm3" value="3" onClick="sales.get(' + id + ').setMode(3);'
		+ 'return true"'+smc[3]+' /> <label for="s' + id + 'm3">Auction Sale</label></td><td colspan="2">&nbsp;</td>'
		+ '</tr></table>';

	return	str;
}}


function	drawSellLinks(valid)
{
	var	str;
	if	(valid)
		str = '<a href="#" onClick="confirmSale();return false">Confirm</a> - ';
	else
		str = '';
	str += '<a href="#" onClick="cancelSale();return false">Cancel</a>';
	document.getElementById('chordc').innerHTML = str;
}


function saleAlert(ec, id) {
	var	str;
	if	(ec == -1)
		str = "Go away, kiddie.";
	else if	(ec == 4)
		str = 'Error: some fleets were no longer available for sale; they have been\nremoved from the '
			+ 'list. Please verify the updated fleet sale form.';
	else if	(ec == 5) {
		str = 'Error: none of the previously selected fleets are available for sale.';
	} else if (ec == 200) {
		str = 'Error: you are not allowed to sell fleets while in vacation mode.';
	} else if (ec == 201) {
		str = 'Error: you are not allowed to sell fleets while under Peacekeeper protection.';
	} else {
		str = 'An error occured while handling the sale at ' + locations.get(id).name + ':\n';
		switch	(ec)
		{
		case	0:
			str += 'The target player could not be found.';
			break;
		case	1:
			str += 'The target player is in a different protection level.';
			break;
		case	2:
			str += 'The price is invalid.';
			break;
		case	3:
			str += 'Auction sales require an expiration date.';
			break;
		case	6:
			str += 'You are trying to sell or give a fleet to yourself.';
			break;
		case	7:
			str += 'The target player has not been in the game long enough to\naccept the offer.';
			break;
		}
	}
	alert(str);
}


function	confirmCancelSale(pl)
{
	return	confirm('Are you sure you want to cancel the sale of th' + (pl?'ese fleets':'is fleet') + '?');
}


function	drawSaleDetails(l, name)
{
	var	str, f = fleets.get(l[1]), o = allies.get(f.owner);
	var	used = f.ships[0] * o.gSpace + f.ships[1] * o.fSpace;
	var	haul = f.ships[2] * o.cHaul + f.ships[3] * o.bHaul;
	var	m = parseInt(f.mode, 10);
	
	str = '<table><tr><td id="chord"><h1>Fleet Sale Details</h1></td><td id="chordc">'
		+ '<a href="#" onClick="prepareUpdate();x_getFleetsList(parseMainData);return false">'
		+ 'Close</a></td></tr></table><h2>Fleet</h2><table class="oavaf"><tr>' + foAvaHeader
		+ '</tr><tr class="flown"><td class="fown">' + o.getName()
		+ '</td><td class="fname">' + f.name + '</td><td class="fhs">'
	if	(haul == 0)
		str += 'N/A';
	else
	{
		var hp = Math.round(used * 100 / haul);
		str += (hp > 200) ? '&gt;200%' : (hp + '%');
	}
	str += '</td><td class="fshp">' + formatNumber(f.ships[0]) + ' / ' + formatNumber(f.ships[1]) + ' / '
		+ formatNumber(f.ships[2]) + ' / ' + formatNumber(f.ships[3]) + '</td><td class="fpwr">'
		+ formatNumber(f.power) + '</td><td class="fco">'
		+ (f.orders.oType == 3 ? 'On sale at ' : 'Waiting for transfer at ') + f.orders.loc.name
		+ '</td></tr></table><h2>Sale details</h2><table class="sdet"><tr><th>Sale type:</th><td>';

	var	st;
	switch	(l[6])
	{
	case	'0':
		if	(l[7] == '0')
		{
			str += 'Gift';
			st = 0;
		}
		else
		{
			str += 'Direct offer';
			st = 1;
		}
		str += '</td></tr><tr><th>To player:</th><td><a href="message?a=c&ct=0&id=' + l[4] + '">'
			+ name + '</a></td></tr>';
		if	(st == 1)
			str += '<tr><th>Price:</th><td>&euro;' + formatNumber(l[7]) + '</td></tr>';
		str += '<tr><th>Expires:</th><td>' + (l[2] == "" ? 'Never' : formatDate(l[2])) + '</td></tr>';
		break;
	case	'1':
		str += 'Public offer</td></tr><tr><th>Price:</th><td>' + formatNumber(l[7]) + '</td></tr>'
			+ '<tr><th>Expires:</th><td>' + (l[2] == "" ? 'Never' : formatDate(l[2])) + '</td></tr>';
		st = 2;
		break;
	case	'2':
		str += 'Auction sale</td></tr><tr><th>Minimum bid:</th><td>' + formatNumber(l[7]) + '</td></tr>'
			+ '<tr><th>Expires:</th><td>' + formatDate(l[2]) + '</td></tr>';
		if	(l[8] != '')
		{
			str += '<tr><th>Latest bid at:</th><td>' + formatDate(l[10]) + '</td></tr><tr><th>'
				+ 'Bid:</th><td>&euro;' + formatNumber(l[8]) + '</td></tr>';
			if	(l[3] == "")
				str += '<tr><th>Bid by:</th><td><a href="message?a=c&ct=0&id=' + l[9] + '">'
					+ name + '</a></td></tr>';
		}
		st = 3;
		break;
	}
	if	(l[3] != "")
	{
		var	tx = parseInt(l[5],10) + 1;
		str += '<tr><th>Finalized:</th><td>' + formatDate(l[3]) + '</td></tr>';
		if	(st > 1)
			str += '<tr><th>Sold to:</th><td><a href="message?a=c&ct=0&id=' + l[4] + '">'
				+ name + '</a></td></tr>';
		str += '<tr><th>Transfer time:</th><td>' + tx + ' hour' + (tx > 1 ? 's' : '') + '</td></tr>';
	}
	str += '</table>';

	document.getElementById('fpage').innerHTML = str;
}

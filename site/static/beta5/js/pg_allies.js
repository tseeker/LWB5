var	lsAllies = new Array(), idAllies = new Array();
var	lsRAllies = new Array(), idRAllies = new Array();
var	lsBans = new Array(), idBans = new Array();

var	tratt;

function	TrustedAlly(id,name)
{
	this.id = id;
	this.name = name;
	this.selected = false;
}

function	ReverseAlly(id,level,name)
{
	this.id = id;
	this.name = name;
	this.level = parseInt(level,10);
	this.selected = false;
}

function	BannedPlayer(id,name)
{
	this.id = id;
	this.name = name;
	this.selected = false;
}

function	parseData(data)
{
	var	list = new Array();
	var	byId = new Array();
	var	l = data.split('\n');
	var	na, nr, nb;
	var	i, a, t, id;

	a = l.shift().split('#');
	na = parseInt(a[0], 10);
	nr = parseInt(a[1], 10);
	nb = parseInt(a[2], 10);

	for	(i=0;i<na;i++)
	{
		a = l.shift().split('#');
		id = a.shift();
		if	(idAllies[id])
			t = idAllies[id];
		else
			t = new TrustedAlly(id, a.join('#'));
		byId[id] = t;
		list.push(t);
	}
	lsAllies = list; idAllies = byId;

	list = new Array(); byId = new Array();
	for	(i=0;i<nr;i++)
	{
		a = l.shift().split('#');
		id = a.shift();
		if	(idRAllies[id])
			t = idRAllies[id];
		else
			t = new ReverseAlly(id, a.shift(), a.join('#'));
		byId[id] = t;
		list.push(t);
	}
	lsRAllies = list; idRAllies = byId;

	list = new Array(); byId = new Array();
	for	(i=0;i<nb;i++)
	{
		a = l.shift().split('#');
		id = a.shift();
		if	(idBans[id])
			t = idBans[id];
		else
			t = new BannedPlayer(id, a.join('#'));
		byId[id] = t;
		list.push(t);
	}
	lsBans = list; idBans = byId;
}


function	initPage()
{
	var	data = document.getElementById('alinit').innerHTML;
	if	(data.split('\n').length == 1 && data.split('#').length > 3)
		x_getTrusted(listReceived);
	else
		listReceived(data);
}

function	listReceived(data)
{
	parseData(data);
	displayPage();
	update = setTimeout('x_getTrusted(listReceived)', 600000);
}


function	displayPage()
{
	var	str = "";

	var	e = document.getElementById('newally');
	if	(lsAllies.length < 5)
		str += drawAddAlly(e ? e.value : "");
	str += drawAllyList();
	document.getElementById('allies').innerHTML = str;
	document.getElementById('rallies').innerHTML = drawReverseList();
	document.getElementById('bans').innerHTML = drawBlackList();
	updateButtons(); updateRButtons(); updateBLButtons();
}


function handleCommand(data) {
	if (data.indexOf('ERR#') == 0) {
		var ei = parseInt(data.replace(/ERR#/, ''), 10);
		commandAlert(ei);
		update = setTimeout('x_getTrusted(listReceived)', 600000);
	} else {
		var e = document.getElementById('newally');
		if (e) {
			e.value = '';
		}
		listReceived(data);
	}
}



function	drawAllyList()
{
	var	i, str = '<h1>' + listTitle + '</h1>';
	if	(lsAllies.length == 0)
		return	str + '<p>' + emptyList + '</p>';

	str += '<table cellspacing="0" cellpadding="0" class="lsallies">';
	for	(i=0;i<lsAllies.length;i++)
	{
		str += '<tr>';
		if	(i==0)
			str += '<td class="spacer" rowspan="' + (lsAllies.length+2) + '">&nbsp;</td>';
		str += '<td class="al'+i+'"><input type="checkbox" ' + tratt[20] + ' name="ally" id="al'+i+'" value="'+i+'"';
		str += ' onClick="lsAllies['+i+'].selected=!lsAllies['+i+'].selected;updateButtons()" ';
		if	(lsAllies[i].selected)
			str += ' checked="checked"';
		str += '/> <label for="al'+i+'">' + lsAllies[i].name + '</label></td></tr>';
	}
	str += '<tr><td>&nbsp;</td></tr><tr><td id="buttons">&nbsp;</td></tr></table>';
	return	str;
}

function	updateButtons()
{
	if	(!document.getElementById('buttons'))
		return;
	
	var	i, cr = false, cmu = true, cmd = true;
	for	(i=0;i<lsAllies.length;i++)
	{
		cr = cr || lsAllies[i].selected;
		cmu = cmu && (!lsAllies[i].selected || i != 0);
		cmd = cmd && (!lsAllies[i].selected || i != lsAllies.length - 1);
	}
	cmu = cmu && cr;
	cmd = cmd && cr;
	document.getElementById('buttons').innerHTML = drawButtons(cr,cmu,cmd);
}

function	moveUp()
{
	clearTimeout(update);

	var	i, sl = new Array();
	for	(i=0;i<lsAllies.length;i++)
		if	(lsAllies[i].selected)
			sl.push(lsAllies[i].id);
	x_moveAllies(sl.join('#'), 1, handleCommand);
}

function	moveDown()
{
	clearTimeout(update);

	var	i, sl = new Array();
	for	(i=0;i<lsAllies.length;i++)
		if	(lsAllies[i].selected)
			sl.push(lsAllies[i].id);
	x_moveAllies(sl.join('#'), 0, handleCommand);
}

function	removeAllies()
{
	clearTimeout(update);

	var	i, sl = new Array();
	for	(i=0;i<lsAllies.length;i++)
		if	(lsAllies[i].selected)
			sl.push(lsAllies[i].id);
	x_removeAllies(sl.join('#'), handleCommand);
}

function	addAlly()
{
	var	n = document.getElementById('newally').value;
	if	(!addAllyConfirm(n))
		return;
	clearTimeout(update);
	x_addAlly(n, handleCommand);
}



function	drawReverseList()
{
	if	(lsRAllies.length == 0)
		return	"<p>" + notTrusted + "</p>";

	var	str = '<table cellspacing="0" cellpadding="0" class="list" id="ralist"><tr>';
	str += '<td class="spacer" rowspan="' + (3+lsRAllies.length) + '">&nbsp;</td><td class="rasel">&nbsp;</td>';
	str += '<th class="raname">'+raHeaders[0]+'</th><th class="ralevel">' + raHeaders[1] + '</th></tr>';

	var	i;
	lsRAllies.sort(new Function('a','b','return a.name.toLowerCase()>b.name.toLowerCase()?1:-1'));
	for	(i=0;i<lsRAllies.length;i++)
	{
		str += '<tr><td class="rasel"><input type="checkbox" ' + tratt[21] + ' name="ally" id="ral'+i+'" value="'+i+'"'
			+ ' onClick="lsRAllies['+i+'].selected=!lsRAllies['+i+'].selected;updateRButtons()" ';
		if	(lsRAllies[i].selected)
			str += ' checked="checked"';
		str += '/></td><td class="raname"><a href="message?a=c&ct=0&id=' + lsRAllies[i].id + '">' + lsRAllies[i].name + '</a></td>'
			+ '<td class="ralevel">' + (lsRAllies[i].level + 1) + '</td></tr>';
	}
	str += '<tr><td colspan="3">&nbsp;</td></tr><tr><td colspan="3" id="rbuttons">&nbsp;</td></tr></table>';

	return	str;
}

function	updateRButtons()
{
	if	(!document.getElementById('rbuttons'))
		return;
	
	var	i;
	for	(i=0;i<lsRAllies.length;i++)
		if	(lsRAllies[i].selected)
			break;
	document.getElementById('rbuttons').innerHTML = (i==lsRAllies.length) ? "&nbsp;" : drawRButtons();
}

function	removeRAllies()
{
	clearTimeout(update);

	var	i, sl = new Array();
	for	(i=0;i<lsRAllies.length;i++)
		if	(lsRAllies[i].selected)
			sl.push(lsRAllies[i].id);

	if	(!confirmRemoveRAllies(sl))
	{
		update = setTimeout('x_getTrusted(listReceived)', 600000);
		return;
	}

	x_removeRAllies(sl.join('#'), handleCommand);
}

function	removeBanRAllies()
{
	clearTimeout(update);

	var	i, sl = new Array();
	for	(i=0;i<lsRAllies.length;i++)
		if	(lsRAllies[i].selected)
			sl.push(lsRAllies[i].id);

	if	(!confirmBanRAllies(sl))
	{
		update = setTimeout('x_getTrusted(listReceived)', 600000);
		return;
	}

	x_removeBanRAllies(sl.join('#'), handleCommand);
}



function	drawBlackList()
{
	var	i, str;
	str = '<h2>' + addBanText + '</h2><p>' + addBanLabel + ' <input type="text" name="addban" id="addban" value="" '
		+ 'size="16" maxlength="15" /> <input type="button" name="baddban" value="' + addBanButton
		+ '" onClick="addBan();return false" /></p><h2>' + blackListTitle + '</h2>';
	if	(lsBans.length == 0)
		return	str + '<p>' + emptyBlackList + '</p>';

	str += '<table cellspacing="0" cellpadding="0" class="lsbans">';
	for	(i=0;i<lsBans.length;i++)
	{
		str += '<tr>';
		if	(i==0)
			str += '<td class="spacer" rowspan="' + (lsBans.length+1) + '">&nbsp;</td>';
		str += '<td class="rasel"><input type="checkbox" ' + tratt[30] + ' name="banned" id="bl'+i+'" value="'+i+'"'
			+ ' onClick="lsBans['+i+'].selected=!lsBans['+i+'].selected;updateBLButtons()" ';
		if	(lsBans[i].selected)
			str += ' checked="checked"';
		str += '/></td><td><a href="message?a=c&ct=0&id=' + lsBans[i].id + '">' + lsBans[i].name + '</a></td></tr>';
	}
	str += '<tr><td>&nbsp;</td></tr><tr><td>&nbsp;</td><td id="blbuttons" colspan="2">&nbsp;</td></tr></table>';
	return	str;
}

function	updateBLButtons()
{
	if	(!document.getElementById('blbuttons'))
		return;
	
	var	i;
	for	(i=0;i<lsBans.length;i++)
		if	(lsBans[i].selected)
			break;
	document.getElementById('blbuttons').innerHTML = (i==lsBans.length) ? "&nbsp;" : drawBLButtons();
}

function	removeBans()
{
	clearTimeout(update);

	var	i, sl = new Array();
	for	(i=0;i<lsBans.length;i++)
		if	(lsBans[i].selected)
			sl.push(lsBans[i].id);
	x_removeBans(sl.join('#'), handleCommand);
}

function	addBan()
{
	clearTimeout(update);

	var	pname = document.getElementById('addban').value;

	var	i = 0, pws = true;
	while	(i < pname.length)
	{
		if	(pname.charAt(i) == " ")
		{
			if	(pws)
			{
				var	s1 = (i>0) ? pname.substr(0, i) : "";
				var	s2 = (i<pname.length-1) ? pname.substr(i+1) : "";
				pname = s1 + s2;
			}
			else
				i++;
			pws = true;
		}
		else
		{
			pws = false;
			i++;
		}
	}
	if	(pws)
		pname = pname.substr(0, pname.length - 1);

	if	(pname == "")
	{
		commandAlert(11);
		update = setTimeout('x_getTrusted(listReceived)', 600000);
		return;
	}

	var	found = null;
	for	(i=0;i<lsRAllies.length;i++)
	{
		if	(lsRAllies[i].name.toLowerCase() == pname.toLowerCase())
		{
			found = lsRAllies[i];
			break;
		}
	}
	if	(found && !confirmBanRAllies([found.id]))
	{
		update = setTimeout('x_getTrusted(listReceived)', 600000);
		return;
	}

	found = null;
	for	(i=0;i<lsBans.length;i++)
	{
		if	(lsBans[i].name.toLowerCase() == pname.toLowerCase())
		{
			found = lsBans[i];
			break;
		}
	}
	if	(found)
	{
		commandAlert(12);
		update = setTimeout('x_getTrusted(listReceived)', 600000);
		return;
	}

	x_addBan(pname, handleCommand);
}

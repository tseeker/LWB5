var	enPlayers = new Array();
var	enAlliances = new Array();
var	lsPlayers = new Array();
var	lsAlliances = new Array();
var	update;

var	enltt;

function	Enemy(id,name)
{
	this.id = id;
	this.name = name;
	this.selected = false;
}


function parseData(data) {
	if (data == "ERR#200") {
		alertRemoveVacation();
		return;
	}

	var	l = data.split('\n');
	var	pl, al, t;

	pl = enPlayers; al = enAlliances;
	enPlayers = new Array(); lsPlayers = new Array();
	enAlliances = new Array(); lsAlliances = new Array();

	while	(l.length > 0) {
		var	a = l.shift().split('#');
		var	tp = a.shift(), id = a.shift();
		if	(tp == 0)
		{
			if	(pl[id])
				t = pl[id];
			else
				t = new Enemy(id, a.join('#'));
			enPlayers[id] = t;
			lsPlayers.push(t);
		}
		else
		{
			if	(al[id])
				t = al[id];
			else
				t = new Enemy(id, a.join('#'));
			enAlliances[id] = t;
			lsAlliances.push(t);
		}
	}
}


function	initList()
{
	var	data = document.getElementById('elinit').innerHTML;
	if	(data != "")
	{
		if	(data.split('#').length > 3 && data.indexOf('\n') == -1)
		{
			// IE really sucks.
			x_getEnemies(updatePage);
			return;
		}
		parseData(data);
	}
	displayPage();
}


function updatePage(data) {
	if (data == "") {
		enPlayers = new Array(); lsPlayers = new Array();
		enAlliances = new Array(); lsAlliances = new Array();
	} else {
		parseData(data);
	}
	displayPage();
}


function	displayPage()
{
	drawList('eal', lsAlliances, "lsAlliances");
	drawList('epl', lsPlayers, "lsPlayers");
	update = setTimeout('x_getEnemies(updatePage)', 60000);
}


function	drawList(lid, lst, lsn)
{
	if	(lst.length == 0)
	{
		document.getElementById(lid + 'div').innerHTML = '<p>' + emptyListText + '</p>';
		return;
	}
	lst.sort(new Function('a','b','return (a.name.toLowerCase()>b.name.toLowerCase())?1:-1'));

	var	i, str = '<table cellspacing="0" cellpadding="0" class="enlist">';
	var	nr = (lst.length % 2) + (lst.length - (lst.length % 2)) / 2;
	for	(i=0;i<lst.length;i++)
	{
		if	(i%2 == 0)
		{
			str += '<tr>';
			if	(i==0)
				str += '<td rowspan="' + (nr+1) + '" class="spacer">&nbsp;</td>';
		}
		str += '<td><input type="checkbox" ' + enltt[0] + ' name="'+lid+'sel" id="'+lid+'s'+i+'" value="'+i+'" onClick="';
		str += lsn+'['+i+'].selected=!'+lsn+'['+i+'].selected;updateButton(\''+lid+'\','+lsn+',\''+lsn+'\')"';
		if	(lst[i].selected)
			str += ' checked="checked"';
		str += '/> <label for="'+lid+'s'+i+'">'+lst[i].name+'</label></td>';
		if	(i%2 == 1)
			str += '</tr>';
	}
	if	(i%2 == 1)
		str += '<td>&nbsp;</td></tr>';
	str += '<tr><td colspan="2" class="lsbutton" id="del'+lid+'">&nbsp;</td></tr></table>';
	document.getElementById(lid + 'div').innerHTML = str;
	updateButton(lid, lst, lsn);
}

function	updateButton(lid, lst, lsn)
{
	var	i;
	for	(i=0;i<lst.length&&!lst[i].selected;i++) ;
	document.getElementById('del'+lid).innerHTML=(i==lst.length) ? '&nbsp;'
		: ('<input type="button" ' + enltt[10] + ' name="bd'+lid+'" value="'+removeText+'" onClick="listRemove('+lsn+',\''+lsn+'\');return false" />');
}


function	listRemove(lst, lsn)
{
	var	i, lids = new Array();
	for	(i=0;i<lst.length;i++)
		if	(lst[i].selected)
			lids.push(lst[i].id);
	clearTimeout(update);
	x_removeEnemies(lsn == 'lsPlayers' ? 0 : 1, lids.join('#'), updatePage);
}


function	addPlayer()
{
	clearTimeout(update);
	x_addEnemy(0, document.getElementById('nepl').value, addDataReceived);
}

function	addAlliance()
{
	clearTimeout(update);
	x_addEnemy(1, document.getElementById('neal').value, addDataReceived);
}

function addDataReceived(data) {
	if (data.indexOf('ERR#') == 0) {
		var ei = parseInt(data.replace(/ERR#/, ''), 10);
		addAlert(ei);
		update = setTimeout('x_getEnemies(updatePage)', 60000);
	} else {
		document.getElementById('neal').value = '';
		document.getElementById('nepl').value = '';
		updatePage(data);
	}
}

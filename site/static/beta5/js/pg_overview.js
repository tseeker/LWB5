var	dFolders, cFolders;
var	genForums, aForums, allianceId;
var	plOverview, flOverview, moOverview, nResearch;
var	unOverview,stDiff,ticks,tUpdate,rankings;
var	complete, protection, updateTimer;

var	ovett;

function	emptyCB(data) { }


function	Tick(id,first,interval,last,name)
{
	this.id		= id;
	this.first	= parseInt(first, 10);
	this.interval	= parseInt(interval, 10);
	this.stopTime	= (last != "") ? parseInt(last, 10) : -1;
	this.name	= name;

	this.started	= true;
	this.stDays	= 0;
	this.previous	= 0;
	this.next	= 0;
	this.last	= 0;
	this.remaining	= 0;

	this.compute	= Tick_compute;
	this.draw	= Tick_draw;
}

function	Tick_compute(st)
{
	this.started = (st >= this.first);
	if	(!this.started)
	{
		var	tl = this.first - st;
		this.stDays = Math.floor((tl - (tl % 86400)) / 86400);
		this.next = this.first;
		this.remaining = this.next - (st + this.stDays * 86400);
		this.last = -1;
		return;
	}
	else
		this.stDays = 0;

	var	f = this.first % 86400;
	var	n = (st % 86400);
	var	tm = n + ((n<f) ? 86400 : 0) - f;
	var	nx = this.interval - tm % this.interval;

	if	(this.stopTime > -1)
	{
		var	s = (this.stopTime - this.first);
		var	m = s % this.interval;
		this.last = this.first + s - m
	}
	else
		this.last = -1;

	if	(this.last != -1 && nx + st > this.last)
	{
		this.next = -1;
		this.previous = this.last;
	}
	else
	{
		this.next = nx + st;
		this.previous = this.next - this.interval;
		this.remaining = nx;
	}
}

function	Tick_draw()
{
	var	str = makeNextText(this.name);
	if	(this.next != -1)
	{
		if	(!this.started)
			str += '<b>' + this.stDays + '</b> ' + getDaysText(this.stDays > 1) + ', ';
		str += '<b>'

		var	rh = (this.remaining - (this.remaining % 3600)) / 3600,
			rm = (this.remaining - rh*3600 - (this.remaining % 60)) / 60,
			rs = this.remaining - (rh*60+rm)*60;
		var	s = rh.toString();
		if	(s.length == 1)
			s = '0' + s;
		str += s + ':';
		s = rm.toString();
		if	(s.length == 1)
			s = '0' + s;
		str += s + ':';
		s = rs.toString();
		if	(s.length == 1)
			s = '0' + s;
		str += s;
	}
	else
		str += '<b>N/A';
	str += '</b>';
	return	str;
}


function	Folder(id, tMsg, nMsg, name)
{
	this.id		= id;
	this.tMsg	= tMsg;
	this.nMsg	= nMsg;
	this.name	= name;
}

function	Category(id, type, name)
{
	this.id		= id;
	this.type	= type;
	this.name	= name;
	this.forums	= new Array();
}

function	Forum(id, nTopics, nUnread, name)
{
	this.id		= id;
	this.nTopics	= nTopics;
	this.nUnread	= nUnread;
	this.name	= name;
}



function initPage() {
	overviewReceived(document.getElementById('init-data').value);
}


function	updatePage()
{
	clearTimeout(tUpdate);
	x_getOverview(overviewReceived);
}


function	parseComms(l)
{
	var	i, a = l.shift().split('#');
	var	nCustom = parseInt(a[0],10), nGenCats = parseInt(a[1],10), nAForums = parseInt(a[2],10);
	allianceId = a[3];

	// Default folders
	dFolders = new Array();
	for	(i=0;i<3;i++)
	{
		a = l.shift().split('#');
		dFolders.push(new Folder('', a[0], a[1], ''));
	}

	// Custom folders
	cFolders = new Array();
	for	(i=0;i<nCustom;i++)
	{
		a = l.shift().split('#');
		cFolders.push(new Folder(a.shift(), a.shift(), a.shift(), a.join('#')));
	}

	// General categories & forums
	genForums = new Array();
	for	(i=0;i<nGenCats;i++)
	{
		a = l.shift().split('#');

		var	j,c,id,tp,nForums;
		id = a.shift(); tp = a.shift();
		nForums = parseInt(a.shift(), 10);
		c = new Category(id, tp, a.join('#'));

		for	(j=0;j<nForums;j++)
		{
			a = l.shift().split('#');
			c.forums.push(new Forum(a.shift(),a.shift(),a.shift(),a.join('#')));
		}

		genForums.push(c);
	}

	// Alliance forums
	aForums = new Array();
	for	(i=0;i<nAForums;i++)
	{
		a = l.shift().split('#');
		aForums.push(new Forum(a.shift(),a.shift(),a.shift(),a.join('#')));
	}
}


function	parseTicks(l)
{
	var	a = l.shift();
	var	now = Math.round((new Date().getTime()) / 1000);
	stDiff = now - parseInt(a,10);
	ticks = new Array();
	while	(l.length > 0)
	{
		a = l.shift().split('#');
		var	t = new Tick(a.shift(), a.shift(), a.shift(), a.shift(), a.join('#'));
		ticks.push(t);
	}
}


function overviewReceived(data) {
	var l = data.split('\n');

	complete = (l.shift() == 1);
	protection = parseInt(l.shift(), 10);
	parseComms(l);
	plOverview = l.shift().split('#');
	flOverview = l.shift().split('#');
	moOverview = l.shift().split('#');
	moOverview[2] = (parseInt(moOverview[0],10) - parseInt(moOverview[1],10)).toString();
	nResearch = l.shift();
	unOverview = l.shift().split('#');
	rankings = l.shift().split('#');
	parseTicks(l);

	drawOverviewPage();
	updateTimer = setTimeout('updatePage()', 60000);
}


function	drawOverviewPage()
{
	if	(complete)
		drawCompleteOverview();
	else
		drawShortOverview();
	drawTicks();
}

function	drawTicks()
{
	var	now = Math.round((new Date().getTime()) / 1000) - stDiff;
	var	i, str = '';
	for	(i=0;i<ticks.length;i++)
	{
		ticks[i].compute(now);
		str += ticks[i].draw() + "<br/>";
	}
	document.getElementById('ticks').innerHTML = str;
	tUpdate = setTimeout('drawTicks()', 1000);
}

function	switchMode()
{
	complete = !complete;
	clearTimeout(tUpdate);
	drawOverviewPage();
	x_switchOvMode(emptyCB);
}

function breakProtection() {
	if (!confirmBreakProtection()) {
		return;
	}
	clearTimeout(updateTimer);
	clearTimeout(tUpdate);
	x_breakProtection(overviewReceived);
}

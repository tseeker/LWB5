var	ticks;
var	tList;
var	updateTimer;
var	stDifference;
var	nUpdates;


function	Tick(id, first, interval, last)
{
	this.id		= id;
	this.first	= parseInt(first, 10);
	this.interval	= parseInt(interval, 10);
	this.stopTime	= (last != "") ? parseInt(last, 10) : -1;

	this.started	= true;
	this.stDays	= 0;
	this.previous	= 0;
	this.next	= 0;
	this.last	= 0;
	this.remaining	= 0;

	this.compute	= Tick_compute;
	this.draw	= Tick_draw;
	this.setEnd	= Tick_setEnd;
}

function	Tick_setEnd(last)
{
	this.stopTime	= (last != "") ? parseInt(last, 10) : -1;
}

function	Tick_compute(st)
{
	var	f = this.first % 86400;
	var	n = (st % 86400);
	var	tm = n + ((n<f) ? 86400 : 0) - f;
	var	nx = this.interval - tm % this.interval;


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
	var	str = '<p>' + tText[0]+ ': ';
	if	(this.started && this.previous >= this.first)
		str += '<b>' + formatDate(this.previous) + '</b>';
	else
		str += '<b>N/A</b>';
	str += '<br/>';
	if	(this.next != -1)
	{
		str += tText[1] + ': <b>' + formatDate(this.next + this.stDays * 86400) + '</b> (' + tText[3] + ': ';
		if	(!this.started)
			str += '<b>' + this.stDays + '</b> ' + getDaysText(this.stDays > 1) + ', ';

		str += '<b>';
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
		str += s + '</b>)';
		if	(this.last != -1)
		{
			str += '<br/>' + tText[2] + ': <b>' + formatDate(this.last) + '</b>';
		}
	}
	else
		str += '<b>' + tText[4] + '</b>';
	str += '</p>';

	document.getElementById('tick' + this.id).innerHTML = str;
}


function	readInitString()
{
	var	l = document.getElementById('tickinit').innerHTML.split('#');
	var	now = Math.round((new Date().getTime()) / 1000);
	var	st = parseInt(l.shift(), 10);
	stDifference = now - st;

	ticks = new Array();
	tList = new Array();
	while	(l.length > 0)
	{
		var	t = new Tick(l.shift(), l.shift(), l.shift(), l.shift());
		ticks[t.id] = t;
		tList.push(t);
		t.compute(st);
		t.draw();
	}

	nUpdates = 0;
	setTimeout('updateTicks()', 1000);
}

function	updateTicks()
{
	var	i;
	var	now = Math.round((new Date().getTime()) / 1000) - stDifference;
	var	as = true;
	for	(i=0;i<tList.length;i++)
	{
		tList[i].compute(now);
		tList[i].draw();
		as = as && (tList[i].last != -1) && (tList[i].last < now);
	}

	if	(as)
		return;

	nUpdates ++;
	if	(nUpdates < 300)
		setTimeout('updateTicks()', 1000);
	else
		x_updateTicks(updateReceived);
}

function	updateReceived(data)
{
	var	l = data.split('#');
	var	now = Math.round((new Date().getTime()) / 1000);
	var	st = parseInt(l.shift(), 10);
	stDifference = now - st;

	while	(l.length > 0)
	{
		var	id = l.shift();
		ticks[id].setEnd(l.shift());
	}

	nUpdates = 0;
	setTimeout('updateTicks()', 1000);
}

var	diptt;

function	initPage()
{
	var	data = document.getElementById('dinit').innerHTML;
	if	(data.indexOf('\n') != -1)
		drawDiplomacyPage(data);
	else	// IE sucks.
		x_getInformation(drawDiplomacyPage);
}

function	drawDiplomacyPage(data)
{
	var	l = data.split('\n');

	// Alliance status
	var	a = l.shift().split('#');
	if	(a[0] == 0)
		drawNoAlliance();
	else if	(a[0] == 1)
		drawPending(a[1],a[2],a[3],a[4],a[5],l.shift(),l.shift(),l.shift());
	else
	{
		var	i, fl = new Array();
		var	tag = l.shift(), name = l.shift();
		var	leader = (a[6] == 1) ? "" : l.shift();
		var	rank = (a[6] == 1) ? "-" : l.shift();
		for	(i=0;i<a[7];i++)
			fl.push(l.shift());
		drawAlliance(a[1],a[2],a[3],a[4],a[5],a[6],tag,name,leader,rank,fl);
	}

	// Allies and enemies
	a = l.shift().split('#');
	var	nenp = parseInt(a[0], 10);
	var	nena = parseInt(a[1], 10);
	var	nall = parseInt(a[2], 10);
	var	nrall = parseInt(a[3], 10);
	if	(nenp+nena+nall+nrall == 0)
		drawNoRelations();
	else
		drawRelations(nenp, nena, nall, nrall);

	// Messages
	a = l.shift().split('#');
	document.getElementById('pm').innerHTML = formatNumber(a[0]);
	document.getElementById('pmn').innerHTML = formatNumber(a[1]);
	document.getElementById('it').innerHTML = formatNumber(a[2]);
	document.getElementById('itn').innerHTML = formatNumber(a[3]);

	// Scientific assistance
	a = l.shift().split('#');
	if	(a[0] == 0)
		drawNoAssistance();
	else
	{
		a.shift();
		drawAssistance(a.shift(), a.join('#'));
	}

	setTimeout('x_getInformation(drawDiplomacyPage)', 120000);
}

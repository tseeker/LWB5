function        makeDiplomacyTooltips()
{
        diptt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<5;i++)
			diptt[i] = "";
		return;
	}
	diptt[0] = tt_Dynamic("Click here to go to the alliance page and create or join one");
	diptt[1] = tt_Dynamic("Click here to go to the alliance page and manage your joining request");
	diptt[2] = tt_Dynamic("Click here to go to alliance page and access your alliance specific information"); 
	diptt[3] = tt_Dynamic("Click here to go to the main page of this specific alliance forum");
	diptt[4] = tt_Dynamic("Click here to go to the scientific assistance page and manage scientific diplomatic exchange");
}												

function	drawNoRelations()
{
	document.getElementById('allies').innerHTML = 'You do not have enemies nor allies.';
}

function	drawRelations(nenp, nena, nall, nrall)
{
	var	str = "";

	if	(nall == 0)
		str += "You have no allies.";
	else
		str += "You have <b>" + nall + "</b> trusted all" + (nall > 1 ? "ies" : "y") + ".";
	str += '<br/>';

	if	(nrall == 0)
		str += "Noone trusts you.";
	else
		str += "<b>" + nrall + "</b> player"+(nrall>1?"s trust":" trusts")+" you.";
	str += '<br/>';

	if	(nenp + nena == 0)
		str += "You have no enemies.";
	else if	(nenp == 0)
		str += "Enemies: <b>" + nena + "</b> alliance" + (nena>1?"s":"");
	else if	(nena == 0)
		str += "Enemies: <b>" + nenp + "</b> player" + (nenp>1?"s":"");
	else
		str += "Enemies: <b>" + nenp + "</b> player" + (nenp>1?"s":"") + " and <b>" + nena + "</b> alliance" + (nena>1?"s":"")
	str += "<br/>";

	document.getElementById('allies').innerHTML = str;
}

function	drawNoAlliance()
{
	document.getElementById('alliance').innerHTML = '<p>You are not a member of any alliance.<br/><a href="alliance" ' + diptt[0] + ' >Alliance page</a></p>';
}

function	drawPending(npl,ax,ay,rk,pts,tag,name,ln)
{
	var	str = '<p>You are requesting to join this alliance:<br/>';
	str += 'Alliance tag: <b>['+tag+']</b><br/>Alliance name: <b>' + name + '</b><br/>';
	str += 'Leader: <b>' + ln + '</b><br/>';

	if	(npl == 0)
		str += 'No controlled planets.';
	else if	(npl == 1)
		str += '<b>1</b> planet at coordinates (<b>'+ax+','+ay+'</b>)';
	else
		str += '<b>'+npl+'</b> planets at average coordinates (<b>'+ax+','+ay+'</b>)';
	str += '<br/>';

	if	(rk != "")
		str += 'The alliance is ranked <b>#'+rk+'</b> with <b>' + formatNumber(pts) + '</b> points.<br/>';

	str += '<br/><a href="alliance" ' + diptt[1] + '>Alliance page</a></p>';

	document.getElementById('alliance').innerHTML = str;
}

function	drawAlliance(npl,ax,ay,rk,pts,il,tag,name,ln,prk,fl)
{
	var	str = '<h2>Overview</h2>';

	str += '<p>Alliance tag: <b>['+tag+']</b><br/>Alliance name: <b>' + name + '</b><br/>';
	if	(il != 1)
		str += 'Leader: <b>' + ln + '</b><br/>';

	if	(npl == 0)
		str += 'No controlled planets.';
	else if	(npl == 1)
		str += '<b>1</b> planet at coordinates (<b>'+ax+','+ay+'</b>)';
	else
		str += '<b>'+npl+'</b> planets at average coordinates (<b>'+ax+','+ay+'</b>)';
	str += '<br/>';

	if	(rk != "")
		str += 'The alliance is ranked <b>#'+rk+'</b> with <b>' + formatNumber(pts) + '</b> points.<br/>';

	str += 'You are ';
	if	(il == 1)
		str += 'the leader';
	else
		str += 'a member';
	str += ' of this alliance';
	if	(il == 0 && prk != "-")
		str += ' with the rank of <b>' + prk + '</b>';

	str += '.<br/><a href="alliance" ' + diptt[2] + ' >Alliance page</a></p>';

	if	(fl.length > 0)
	{
		str += '<h2>Forums</h2><p>';
		var	i;
		for	(i=0;i<fl.length;i++)
		{
			var	a = fl[i].split('#');
			var	id = a.shift(), ntot = a.shift(), nunr = a.shift();
			str += '<a href="forums?cmd=F%23A%23'+id+'" ' + diptt[3] + '>' + a.join('#') + '</a>: ';
			if	(ntot>0)
			{
				str += '<b>' + ntot + '</b> topic' + (ntot>1?"s":"");
				if	(nunr > 0)
					str += ', <b>' + nunr + '</b> unread';
			}
			else
				str += 'empty forum';
			str += '<br/>';
		}
		str += '</p>';
	}

	document.getElementById('alliance').innerHTML = str;
}


function	drawNoAssistance()
{
	document.getElementById('rsass').innerHTML = 'You cannot yet give or receive scientific assistance.';
}

function	drawAssistance(pend,sto)
{
	var	str = 'You have ';
	if	(pend == 0)
		str += 'no pending assistance offers.';
	else if	(pend == 1)
		str += '<b>1</b> pending offer.';
	else
		str += '<b>'+pend+'</b> pending offers.';
	str += '<br/>';

	if	(sto == "")
		str += 'You haven\'t sent any offer in the last 24 hours.';
	else
		str += 'You have sent an offer to <b>'+sto+'</b> in the last 24 hours.';
	str += '<br/><a href="research?p=d" ' + diptt[4] + ' >Scientific Assistance page</a>';
	document.getElementById('rsass').innerHTML = str;
}

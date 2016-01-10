function        makeOverviewTooltips()
{
        ovett = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<23;i++)
			ovett[i] = "";
		return;
	}
	ovett[0] = tt_Dynamic("Click here to switch to Complete Overview mode");
	ovett[1] = tt_Dynamic("Click here to go to your inbox and read your new external message(s)");
	ovett[2] = tt_Dynamic("Click here to go to your internal transmissin folder and read your new internal message(s)");
	ovett[3] = tt_Dynamic("Click here to go to the compose page and prepare a new message to be sent");
	ovett[4] = tt_Dynamic("Click here to go to this general forum main page");
	ovett[5] = tt_Dynamic("Click here to go to your alliance forum main page");
	ovett[10] = tt_Dynamic("Click here to switch to Short Overview mode");
	ovett[11] = tt_Dynamic("Click here to go directly to this folder");
	ovett[12] = tt_Dynamic("Click here to go directly to the planets overview page");
	ovett[13] = tt_Dynamic("Click here to go directly to the fleets overview page");
	ovett[14] = tt_Dynamic("Click here to go directly to the research management page");
	ovett[15] = tt_Dynamic("Click here to go directly to the money page");
	ovett[16] = tt_Dynamic("Click here to go to the main page for this forum category");
	ovett[17] = tt_Dynamic("Click here to go to this forum main page");
	ovett[18] = tt_Dynamic("Click here to go to you alliance forums main page");
	ovett[19] = tt_Dynamic("Click here to go to the maps page");
	ovett[20] = tt_Dynamic("Click here to go to the universe overview page");
	ovett[21] = tt_Dynamic("Click here to go to the ticks page");
	ovett[22] = tt_Dynamic("Click here to go to the rankings page");
}


function	makeNextText(name)
{
	return	"Next " + name + ": ";
}

function	makeTopicsText(tot, n)
{
	if	(tot == 0)
		return	"empty forum";
	var	str = '<b>' + formatNumber(tot) + '</b> topic' + (tot > 1 ? 's' : '');
	if	(n == 0)
		return	str;
	str += ' (<b>' + formatNumber(n) + '</b> unread)';
	return	str;
}

function drawShortOverview() {
	var	str = '<table><tr><td class="pc50"><h1>Empire</h1></td><td class="pc30"><h1>Universe</h1></td><td class="flink"><a href="#" '
		+ ovett[0] + ' onClick="switchMode();return false">Complete overview</a> - <a href="manual?p=overview_page">Help</a></td></tr>';

	str += '<tr><td><h2>Messages</h2><p>You have ';
	if	(dFolders[0].nMsg > 0 && dFolders[1].nMsg > 0)
	{
		str += '<a href="message?a=f&f=I" ' + ovett[1] + ' >'+formatNumber(dFolders[0].nMsg)+' external</a>';
		str += ' and <a href="message?a=f&f=T" ' + ovett[2] + ' >'+formatNumber(dFolders[1].nMsg)+' internal</a> messages';
	}
	else if	(dFolders[0].nMsg > 0)
	{
		str += '<a href="message?a=f&f=I" ' + ovett[1] + ' >'+formatNumber(dFolders[0].nMsg)+' external</a> message';
		if	(dFolders[0].nMsg > 1)
			str += 's';
	}
	else if	(dFolders[1].nMsg > 0)
	{
		str += '<a href="message?a=f&f=T" ' + ovett[2] + ' >'+formatNumber(dFolders[1].nMsg)+' internal</a> message';
		if	(dFolders[1].nMsg > 1)
			str += 's';
	}
	else
		str += 'no new messages';
	str += '.<br/><a href="message?a=c" ' + ovett[3] + ' >Compose</a> a message.</p>';

	str += '<h2>Planets</h2><p>'
		+ (protection == 0 ? '' : (
			'<b>Under protection</b> - <b>' + protection + '</b> day'
			+ (protection > 1 ? 's' : '') + ' left (<a href="#" onclick="breakProtection(); return false">'
			+ 'Break protection</a>)<br/>'))
		+ 'Planets owned: <b>' + plOverview[0] + '</b><br/>'
		+ 'Total population: <b>' + formatNumber(plOverview[2]) + '</b><br/>'
		+ 'Total factories: <b>' + formatNumber(plOverview[4]) + '</b>'
		+ '</p>'
	str += '<h2>Fleets</h2><p>Total fleet power: <b>'+formatNumber(flOverview[0])+'</b></p>';
	str += '<h2>Money</h2><p>Daily Profit: <b>&euro;'+formatNumber(moOverview[2])+'</b></p></td>';

	str += '<td colspan="2"><h2>Forums</h2><p>';
	var	i,j,k,a=new Array();
	for	(i=0;i<genForums.length;i++)
	{with(genForums[i]){
		k = 0;
		for	(j=0;j<forums.length;j++)
			k += parseInt(forums[j].nUnread,10);
		j = (k==0? "no unread topics" : (k + ' unread topic' + (k>1 ? 's' : '')));
		a.push('<a href="forums?cmd=C%23G%23'+id+'" ' + ovett[4] + ' >'+name+'</a>: ' + j);
	}}

	if	(aForums.length)
	{
		k = 0;
		for	(j=0;j<aForums.length;j++)
			k += parseInt(aForums[j].nUnread,10);
		j = (k==0? "no unread topics" : (k + ' unread topic' + (k>1 ? 's' : '')));
		a.push('<a href="forums?cmd=C%23A%23'+allianceId+'" ' + ovett[5] + ' >Alliance forums</a>: ' + j);
	}
	str += a.join('<br/>') + '</p>';

	str += '<h2>Planets</h2><p><b>'+formatNumber(unOverview[0])+'</b> planets</p>';
	str += '<h2>Next ticks</h2><p id="ticks"> </p>';
	str += '<h2>Rankings</h2><p>General ranking: <b>#'+formatNumber(rankings[2])+'</b><br/>';
	str += 'Round ranking: <b>' + (rankings[10] == '' ? 'N/A' : ('#' + formatNumber(rankings[10]))) + '</b></p></td>';
	str += '</tr></table>';
	document.getElementById('overview').innerHTML = str;
}

function	drawCompleteOverview() {
	var str = '<table><tr><td class="pc50"><h1>Empire</h1></td><td class="pc30"><h1>Universe</h1></td><td class="flink"><a href="#" '
		+ ovett[10] + ' onClick="switchMode();return false">Short overview</a> - <a href="manual?p=overview_page">Help</a></td></tr>';

	str += '<tr><td><h2>Messages</h2><p>';
	var	i,dfld = ['Inbox','Internal Transmissions','Outbox'],dcmd=['I','T','O'];
	for	(i=0;i<3;i++)
	{
		str += '<a href="message?a=f&f='+dcmd[i]+'" ' + ovett[11] + ' >'+dfld[i]+'</a>: <b>' + formatNumber(dFolders[i].tMsg);
		str += '</b> message' + (dFolders[i].tMsg > 1 ? 's' : '');
		if	(dFolders[i].nMsg > 0)
			str += ' (<b>' + formatNumber(dFolders[i].nMsg) + '</b> unread)';
		str += '<br/>';
	}
	str += '<a href="message?a=c" ' + ovett[3] + ' >Compose</a> a message.</p>';

	str += '<h2>Planets</h2><p>'
		+ (protection == 0 ? '' : (
			'<b>Under protection</b> - <b>' + protection + '</b> day'
			+ (protection > 1 ? 's' : '') + ' left (<a href="#" onclick="breakProtection(); return false">'
			+ 'Break protection</a>)<br/>'))
		+ 'Planets owned: <b>'+plOverview[0]+'</b>';
	if	(plOverview[0] > 0)
	{
		str += '<br/>Average happiness: <b class="phap';
		if	(plOverview[1] >= 70)
			str += 'ok';
		else if	(plOverview[1] >= 40)
			str += 'med';
		else if	(plOverview[1] >= 20)
			str += 'dgr';
		else
			str += 'bad';
		str += '">' + plOverview[1] + '%</b><br/>';
		str += 'Average corruption: <b class="phap';
		if	(plOverview[9] >= 71)
			str += 'bad';
		else if	(plOverview[9] >= 41)
			str += 'dgr';
		else if	(plOverview[9] >= 11)
			str += 'med';
		else
			str += 'ok';
		str += '">'+formatNumber(plOverview[9])+'%</b><br/>';
		str += 'Total population: <b>'+formatNumber(plOverview[2])+'</b> (avg. <b>'
		str += formatNumber(plOverview[3]) + '</b>)<br/>';
		str += 'Total factories: <b>'+formatNumber(plOverview[4])+'</b> (avg. <b>'
		str += formatNumber(plOverview[5]) + '</b>)<br/>';
		str += 'Total turrets: <b>'+formatNumber(plOverview[6])+'</b> (avg. <b>'
		str += formatNumber(plOverview[7]) + '</b>)';
	}
	str += '<br/><a href="planets" ' + ovett[12] + ' >More details...</a></p>';

	str += '<h2>Fleets</h2><p>Total fleet power: <b>'+formatNumber(flOverview[0])+'</b>'
	if	(flOverview[1] > 0)
	{
		str += '<br/><b>' + formatNumber(flOverview[1]) + '</b> fleet' + (flOverview[1]>1?'s':'');
		if	(flOverview[2] > 0)
			str += ' (<b>' + formatNumber(flOverview[2]) + '</b> engaged in battle)';
	}
	str += '<br/><a href="fleets" ' + ovett[13] + ' >More details...</a></p>';

	str += '<h2>Research</h2><p>';
	if	(nResearch == 0)
		str += 'Sorry, no new technology has been discovered at this time.';
	else
		str += '<b>' + nResearch + '</b> new technolog' + (nResearch > 1 ? 'ies have' : 'y has') + ' been discovered.';
	str += '<br/><a href="research" ' + ovett[14] + ' >More details...</a></p>';

	str += '<h2>Money</h2><p>';
	str += 'Income: <b>&euro;'+formatNumber(moOverview[0])+'</b><br/>';
	str += 'Fleet Upkeep: <b>&euro;'+formatNumber(moOverview[1])+'</b><br/>';
	str += 'Daily Profit: <b>&euro;'+formatNumber(moOverview[2])+'</b><br/>';
	str += '<a href="money" ' + ovett[15] + ' >More details...</a></p></td>';

	str += '<td colspan="2"><h2>Forums</h2><p>';
	var	j,a=new Array(),s;
	for	(i=0;i<genForums.length;i++)
	{with(genForums[i]){
		s = '<b>' + name + '</b> (<a href="forums?cmd=C%23G%23'+id+'" ' + ovett[16] + '>view</a>)';
		for	(j=0;j<forums.length;j++)
		{
			s += '<br/>&nbsp;&nbsp;-&nbsp;<a href="forums?cmd=F%23' + type + '%23' + forums[j].id + '" ' + ovett[17] + '>' + forums[j].name + '</a>: ';
			s += makeTopicsText(forums[j].nTopics, forums[j].nUnread);
		}
		a.push(s);
	}}

	if	(aForums.length)
	{
		s = '<b>Alliance Forums</b> (<a href="forums?cmd=C%23A%23'+allianceId+'" ' + ovett[18] + ' >view</a>)';
		for	(j=0;j<aForums.length;j++)
		{
			s += '<br/>&nbsp;&nbsp;-&nbsp;<a href="forums?cmd=F%23A%23' + aForums[j].id + '" ' + ovett[17] + ' >' + aForums[j].name + '</a>: ';
			s += makeTopicsText(aForums[j].nTopics, aForums[j].nUnread);
		}
		a.push(s);
	}
	str += a.join('<br/><br/>') + '</p>';

	str += '<h2>Universe</h2><p><b>'+formatNumber(unOverview[0])+'</b> planets';// (<b>';
	str += /*formatNumber(unOverview[2]) + '</b> at the same prot. level)*/'<br/><b>';
	str += formatNumber(unOverview[1]) + '</b> systems occupied by nebulas<br/>';
	str += '<a href="map" ' + ovett[19] + ' >Maps</a> - <a href="universe" ' + ovett[20] + '>More details...</a></p>';

	str += '<h2>Next ticks</h2><p><span id="ticks"> </span><a href="ticks" ' + ovett[21] + ' >More details...</a></p>';
	str += '<h2>Rankings</h2><p>General ranking: <b>#'+formatNumber(rankings[2])+'</b> (<b>'+formatNumber(rankings[1])+'</b> points)<br/>';
	str += 'Civilisation ranking: <b>#'+formatNumber(rankings[4])+'</b> (<b>'+formatNumber(rankings[3])+'</b> points)<br/>';
	str += 'Military ranking: <b>#'+formatNumber(rankings[8])+'</b> (<b>'+formatNumber(rankings[7])+'</b> points)<br/>';
	str += 'Financial ranking: <b>#'+formatNumber(rankings[6])+'</b> (<b>'+formatNumber(rankings[5])+'</b> points)<br/>';
	str += 'Inflicted damage ranking: <b>#'+formatNumber(rankings[12])+'</b> (<b>'+formatNumber(rankings[11])+'</b> points)<br/>';
	if	(rankings[10] == '')
		str += 'You are too weak to be in the round rankings.';
	else
		str += 'Round ranking: <b>#' + formatNumber(rankings[10]) + '</b> (<b>'+formatNumber(rankings[9])+'</b> points)'
	str += '<br/><a href="rank" ' + ovett[22] + ' >More details...</a></p></td>';
	str += '</tr></table>';
	document.getElementById('overview').innerHTML = str;
}


function	getDaysText(p)
{
	return	"day" + (p?'s':'');
}


function confirmBreakProtection() {
	return confirm('You are about to break away from Peacekeeper protection.\n'
		+ 'Anyone will be able to attack your planets afterwards.\n'
		+ 'Please confirm.');
}

var	pageTitles = new Array('Research topics', 'Laws', 'Research budget', 'Diplomacy');
var	rTypes = new Array('Fundamental', 'Military', 'Civilian');

function	makeResearchTooltips()
{
	rett = new Array();
	if	(ttDelay == 0)
	{
		var	i;
		for	(i=0;i<82;i++)
			rett[i] = "";
		return;
	}
	rett[0] = tt_Dynamic("Click here to hide / show this research topic");
	rett[1] = tt_Dynamic("Click here to implement this technology");
	rett[10] = tt_Dynamic("Click here to hide / show this law");
	rett[11] = tt_Dynamic("Click here to revoke this law");
	rett[12] = tt_Dynamic("Click here to enact this law");
	rett[20] = tt_Dynamic("Click here to decrease the percentage of research points attributed to this category by 10 points");
	rett[21] = tt_Dynamic("Click here to decrease the percentage of research points attributed to this category by 1 point");
	rett[22] = tt_Dynamic("Click here to increase the percentage of research points attributed to this category by 1 point");
	rett[23] = tt_Dynamic("Click here to increase the percentage of research points attributed to this category by 10 points");
	rett[30] = tt_Dynamic("Click here to validate the new budget attributions");
	rett[31] = tt_Dynamic("Click here to reset the budget attributions");
	rett[40] = tt_Dynamic("Check this radio button to offer scientific assistance");
	rett[41] = tt_Dynamic("Type here the name of the player to send the offer to");
	rett[42] = tt_Dynamic("Type here the amount of cash you want to receive in exchange for the offer");
	rett[43] = tt_Dynamic("Click here to send the offer");
	rett[44] = tt_Dynamic("Check this radio button to offer the selected technology");
	rett[45] = tt_Dynamic("Choose here the technology you want to offer");
	rett[50] = tt_Dynamic("Select here the page of pending technology exchange offers you want to display");
	rett[51] = tt_Dynamic("Click here to accept this technology exchange");
	rett[52] = tt_Dynamic("Click here to decline this technology exchange");
	rett[60] = tt_Dynamic("Selected here the page of the history of technology exchanges to display");
	rett[70] = tt_Dynamic("Click here to implement new technologies, view foreseen breakthroughs and already implemented technologies");
	rett[71] = tt_Dynamic("Click here to enact and revoke laws");
	rett[72] = tt_Dynamic("Click here to balance your research budget between fundamental, military and civilian research");
	rett[73] = tt_Dynamic("Click here to give or sell technologies and scientific knowledge with other empires, and examine offers made to you");
	rett[80] = tt_Dynamic("Click here to unlock the category");
	rett[81] = tt_Dynamic("Click here to prevent this category from being changed along with the other two");
}

function	getLoadingText()
{
	return	"Loading page data ...";
}




function	getTopicsPage()
{
	var	str = '<h1>New technologies</h1>';
	var	tl = '!' + rtExpanded.join('!!') + '!';
	var	i, d, id;

	if	(rtCompleted.length == 0)
		str += '<p>No new technology has been discovered.</p>';
	else
	{
		str += '<table cellspacing="0" cellpadding="0" class="list">';
		str += '<tr><td class="sdesc">&nbsp;</td><th class="tname">Technology</th><th class="itype">Type</th><th class="icost">Cost</th><td class="impl">&nbsp;</td></tr>';
		for	(i=0;i<rtCompleted.length;i++)
		{
			id = rtCompleted[i];
			d = (tl.indexOf('!' + id + '!') != -1);
			str += '<tr><td class="sdesc"><a href="#" ' + rett[0] + ' onClick="' + (d ? 'hide' : 'show') + 'Description('+id+'); return false;">';
			str += '[&nbsp;' + (d ? '-' : '+') + '&nbsp;]</a>&nbsp;</td>';
			if (d) {
				str += '<td rowspan="2"><span onClick="' + (d ? 'hide' : 'show') + 'Description('+id+')">' + rtTitles[id] + '</span><br/>';
				str += rtDescriptions[id] + '<br/><a href="manual?p=tech_' + id
					+ '" style="color:white;text-decoration:underline;font-weight:normal" target="_blank">More information ...</a></td>';
			}
			else
				str += '<td onClick="' + (d ? 'hide' : 'show') + 'Description('+id+')">' + rtTitles[id] + '</td>';
			str += '<td class="itype">' + rTypes[rtTypes[id]] + '</td>';
			str += '<td class="icost">&euro;' + formatNumber(rtCosts[id]) + '</td><td><a href="#" ' + rett[1] + ' onClick="';
			str += 'implementTechnology(' + id + '); return false;">Implement technology</a></td>';
			if	(d)
				str += '</tr><tr><td>&nbsp;</td><td colspan="3">&nbsp;</td>';
			str += '</tr>';
		}
		str += '</table><br/>';
	}

	str += '<h1>Foreseen breakthroughs</h1>';
	if	(rtForeseen.length == 0)
		str += '<p>No breakthrough is currently in sight.</p>';
	else
	{
		str += '<table cellspacing="0" cellpadding="0" class="list">';
		str += '<tr><td class="sdesc">&nbsp;</td><th class="tname">Technology</th><th class="itype">Type</th><th class="icost">Estimated cost</th><th class="compl">Completion</th></tr>';
		for	(i=0;i<rtForeseen.length;i++)
		{
			id = rtForeseen[i];
			d = (tl.indexOf('!' + id + '!') != -1);
			str += '<tr><td class="sdesc"><a href="#" ' + rett[0] + ' onClick="' + (d ? 'hide' : 'show') + 'Description('+id+'); return false;">';
			str += '[&nbsp;' + (d ? '-' : '+') + '&nbsp;]</a>&nbsp;</td>';
			if	(d)
			{
				str += '<td rowspan="2"><span onClick="' + (d ? 'hide' : 'show') + 'Description('+id+')">' + rtTitles[id] + '</span><br/>';
				str += rtDescriptions[id]  + '<br/><a href="manual?p=tech_' + id
					+ '" style="color:white;text-decoration:underline;font-weight:normal" target="_blank">More information ...</a></td>';
			}
			else
				str += '<td onClick="' + (d ? 'hide' : 'show') + 'Description('+id+')">' + rtTitles[id] + '</td>';
			str += '<td class="itype">' + rTypes[rtTypes[id]] + '</td>';
			str += '<td class="icost">&euro;' + formatNumber(rtCosts[id]) + '</td><td class="compl">' + rtForeseenP[id] + '%</td>';
			if	(d)
				str += '</tr><tr><td>&nbsp;</td><td colspan="2">&nbsp;</td>';
			str += '</tr>';
		}
		str += '</table><br/>';
	}

	str += '<h1>Implemented technologies</h1><table cellspacing="0" cellpadding="0"><tr>'
		+ drawImplementedList(tl, 0) + drawImplementedList(tl, 1) + drawImplementedList(tl, 2)
		+ '</tr></table>';

	return	str;
}


function	drawImplementedList(tl, type)
{
	var	str = '<td class="div3"><h3>' + rTypes[type] + ' research</h3>';
	if	(rtImplemented.length == 0)
		return	str + '<p>No such technology has been implemented at the moment.</p></td>';

	var	n = 0;
	for	(i=0;i<rtImplemented.length;i++)
	{
		id = rtImplemented[i];
		if	(rtTypes[id] != type)
			continue;
		if	(n == 0)
			str += '<table class="list">';
		n ++;
		d = (tl.indexOf('!' + id + '!') != -1);
		str += '<tr><td class="sdesc"><a href="#" ' + rett[0] + ' onClick="' + (d ? 'hide' : 'show') + 'Description('+id+'); return false;">';
		str += '[&nbsp;' + (d ? '-' : '+') + '&nbsp;]</a>&nbsp;</td>';
		if	(d)
		{
			str += '<td rowspan="2"><span onClick="' + (d ? 'hide' : 'show') + 'Description('+id+')">' + rtTitles[id] + '</span><br/>';
			str += rtDescriptions[id] + '<br/><a href="manual?p=tech_' + id
				+ '" style="color:white;text-decoration:underline;font-weight:normal" target="_blank">More information ...</a></td></tr><tr><td>&nbsp;</td></tr>';
		}
		else
			str += '<td onClick="' + (d ? 'hide' : 'show') + 'Description('+id+')">' + rtTitles[id] + '</td></tr>';
	}
	if	(n>0)
		str += '</table></td>';
	else
		str += '<p>No such technology has been implemented at the moment.</p></td>';
	return	str;
}


function	confirmTech(id)
{
	var	str;
	str = 'Implementing ' + rtTitles[id] + '\nThis will cost ' + formatNumber(rtCosts[id]) + ' euros.\n';
	str += 'Are you sure you want to do this?';
	return	confirm(str);
}


function implError(code) {
	var str = 'Error: ';
	switch (code) {
	case 0:
		str += 'you do not have enough cash to implement this technology';
		break;
	case 1:
		str += 'trying to cheat by hacking the server _is_ evil';
		break;
	case 200:
		str += 'you can\'t implement technologies while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}
	alert(str + '.');
}




function	getLawsPage()
{
	var	str = '<h1>Enacted laws</h1>';
	var	tl = '!' + rtExpanded.join('!!') + '!';
	var	i, d, id;

	if	(lEnacted.length == 0)
		str += '<p>No laws have been enacted yet.</p>';
	else
	{
		str += '<table cellspacing="0" cellpadding="0" class="list">';
		str += '<tr><td class="sdesc">&nbsp;</td><th class="tname">Law</th><th class="icost">Cost</th><td class="impl">&nbsp;</td></tr>';
		for	(i=0;i<lEnacted.length;i++)
		{
			id = lEnacted[i];
			d = (tl.indexOf('!' + id + '!') != -1);
			str += '<tr><td class="sdesc"><a href="#" ' + rett[10] + 'onClick="' + (d ? 'hide' : 'show') + 'Description('+id+'); return false;">';
			str += '[&nbsp;' + (d ? '-' : '+') + '&nbsp;]</a>&nbsp;</td>';
			if	(d)
				str += '<td rowspan="2">' + rtTitles[id] + '<br/>' + rtDescriptions[id] + '<br/><a href="manual?p=tech_' + id + '" style="color:white;text-decoration:underline;font-weight:normal" target="_blank">More information ...</a></td>';
			else
				str += '<td>' + rtTitles[id] + '</td>';
			if	(lTimeRevoke[id] == 0)
			{
				str += '<td class="icost">&euro;' + formatNumber(rtCosts[id]) + '</td><td><a href="#" ' + rett[11] + ' onClick="';
				str += 'revokeLaw(' + id + '); return false;">Revoke</a></td>';
			}
			else
			{
				str += '<td colspan="2">' + lTimeRevoke[id] + ' day' + (lTimeRevoke[id] > 1 ? 's' : '');
				str += ' left before the law can be revoked.</td>';
			}
			if	(d)
				str += '</tr><tr><td>&nbsp;</td><td colspan="2">&nbsp;</td>';
			str += '</tr>';
		}
		str += '</table><br/>';
	}

	str += '<h1>Available laws</h1>';
	if	(lAvailable.length == 0)
		str += '<p>No laws are available.</p>';
	else
	{
		str += '<table cellspacing="0" cellpadding="0" class="list">';
		str += '<tr><td class="sdesc">&nbsp;</td><th class="tname">Law</th><th class="icost">Cost</th><td class="impl">&nbsp;</td></tr>';
		for	(i=0;i<lAvailable.length;i++)
		{
			id = lAvailable[i];
			d = (tl.indexOf('!' + id + '!') != -1);
			str += '<tr><td class="sdesc"><a href="#" ' + rett[10] + ' onClick="' + (d ? 'hide' : 'show') + 'Description('+id+'); return false;">';
			str += '[&nbsp;' + (d ? '-' : '+') + '&nbsp;]</a>&nbsp;</td>';
			if	(d)
				str += '<td rowspan="2">' + rtTitles[id] + '<br/>' + rtDescriptions[id] + '<br/><a href="manual?p=tech_' + id + '" style="color:white;text-decoration:underline;font-weight:normal" target="_blank">More information ...</a></td>';
			else
				str += '<td>' + rtTitles[id] + '</td>';
			if	(lTimeEnact[id] == 0)
			{
				str += '<td class="icost">&euro;' + formatNumber(rtCosts[id]) + '</td><td><a href="#" ' + rett[12] + ' onClick="';
				str += 'enactLaw(' + id + '); return false;">Enact</a></td>';
			}
			else
			{
				str += '<td colspan="2">' + lTimeEnact[id] + ' day' + (lTimeEnact[id] > 1 ? 's' : '');
				str += ' left before the law can be enacted.</td>';
			}
			if	(d)
				str += '</tr><tr><td>&nbsp;</td><td colspan="2">&nbsp;</td>';
			str += '</tr>';
		}
		str += '</table><br/>';
	}

	return	str;
}


function	confirmEnact(id)
{
	var	str;
	str = 'Enacting ' + rtTitles[id] + ' will cost ' + formatNumber(rtCosts[id]) + ' euros.\n';
	str += 'You will not be able to revoke the law before 5 days have passed.\n';
	str += 'Are you sure you want to do this?';
	return	confirm(str);
}


function	confirmRevoke(id)
{
	var	str;
	str = 'Revoking ' + rtTitles[id] + ' will cost ' + formatNumber(rtCosts[id]) + ' euros.\n';
	str += 'You will not be able to enact the law again before 5 days have passed.\n';
	str += 'Are you sure you want to do this?';
	return	confirm(str);
}


function slawError(code) {
	var str = 'Error: ';
	switch (code) {
	case 0:
		str += 'you do not have enough cash to switch this law\'s status';
		break;
	case 1:
		str += 'trying to cheat by hacking the server _is_ evil';
		break;
	case 200:
		str += 'you can\'t enact or revoke laws while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}
	alert(str + '.');
}



function budgetError(code) {
	var str = 'Error: ';
	switch (code) {
	case 0:
		str += 'trying to cheat by hacking the server _is_ evil';
		break;
	case 200:
		str += 'you can\'t modify your research budget while in vacation mode';
		break;
	default:
		str += 'an unknown error has occured';
		break;
	}
	alert(str + '.');
}



function	drawAttributionsDisplay()
{
	var	i, str = '<h1>Research budget</h1>';
	str += '<p>Research points: <b id="totRPoints"></b> per day</p>';
	str += '<form action="?"><table class="rbudget" cellspacing="0" cellpadding="0">';

	for	(i=0;i<rTypes.length;i++)
	{
		str += '<tr>';
		if	(i==0)
			str += '<td class="rbico" rowspan="' + rTypes.length + '">&nbsp;</td>';
		str += '<td class="rbttl">' + rTypes[i] + ' research:</td>';
		if	(i==0)
			str += '<td class="rbico" rowspan="' + rTypes.length + '">&nbsp;</td>';
		str += '<td class="rbico" ' + rett[20] + ' onClick="changeAttr(' + i + ', -10);"><img src="'+staticurl+'/beta5/pics/dec2_'+color+'.gif" alt="<<" /></td>';
		str += '<td class="rbico" ' + rett[21] + '  onClick="changeAttr(' + i + ', -1);"><img src="'+staticurl+'/beta5/pics/dec1_'+color+'.gif" alt="<" /></td>';
		str += '<td class="rbval"><b id="rbval' + i + '"> </b>%</td>';
		str += '<td class="rbico" ' + rett[22] + ' onClick="changeAttr(' + i + ', 1);"><img src="'+staticurl+'/beta5/pics/add1_'+color+'.gif" alt=">" /></td>';
		str += '<td class="rbico" ' + rett[23] + ' onClick="changeAttr(' + i + ', 10);"><img src="'+staticurl+'/beta5/pics/add2_'+color+'.gif" alt=">>" /></td>';
		if	(i==0)
			str += '<td class="rbico" rowspan="' + rTypes.length + '">&nbsp;</td>';
		str += '<td class="rbico" id="rblock'+i+'">&nbsp;</td>';
		if	(i==0)
			str += '<td class="rbico" rowspan="' + rTypes.length + '">&nbsp;</td>';
		str += '<td><b id="rbpts'+i+'"> </b> point(s)</td></tr>';
	}
	str += '<tr><td colspan="12">&nbsp;</td></tr>';
	str += '<tr id="rbctrl"><td class="rbcl" colspan="5" id="rbcl">&nbsp;</td><td>&nbsp;</td>';
	str += '<td class="rbcr" colspan="6" id="rbcr">&nbsp;</td></tr>';
	str += '</table></form>';

	document.getElementById('respage').innerHTML = str;
}


function	drawBudgetControls()
{
	document.getElementById('rbcl').innerHTML = '<input ' + rett[30] + ' type="button" name="ok" value="Modify research budget" onClick="validateBudget(); return false;" />';
	document.getElementById('rbcr').innerHTML = '<input ' + rett[31] + ' type="button" name="cancel" value="Reset to previous values" onClick="resetBudget(); return false;" />';
}



function	drawDiplomacyRestrained(d)
{
	var	str = '<h1>Give / receive scientific assistance</h1>';
	str += '<p>Your account is too young to use this feature. You must wait for ' + d + ' more day' + (d > 1 ? 's' : '') + '.</p>';
	document.getElementById('respage').innerHTML = str;
}


function	drawRDSentAlready()
{
	var	str = '<h1>Send scientific assistance</h1>';
	str += '<p>We already sent an assistance offer in the last 22 hours (<b>' + formatDate(rdSentOffer.time) + '</b>).<br/>';
	str += 'We offered to give <b>' + rdSentOffer.player + '</b> ';
	if	(rdSentOffer.tech == "")
		str += 'research assistance';
	else
		str += 'the "<b>' + rtTitles[rdSentOffer.tech] + '</b>" technology';
	if	(rdSentOffer.price > 0)
		str += ' in exchange for <b>&euro;' + formatNumber(rdSentOffer.price) + '</b>';
	str += '; the offer ';
	if	(rdSentOffer.status == "P")
		str += 'is <b>still pending</b>.';
	else if	(rdSentOffer.status == "A")
		str += 'has been <b>accepted</b>.';
	else
		str += 'has been <b>declined</b>.';
	str += '</p>';
	document.getElementById('rdgive').innerHTML = str;
}


function	drawRDSendForm()
{
	var	i, str = '<h1>Send scientific assistance</h1>';

	str += '<table cellspacing="0" cellpadding="0"><tr><td class="div20">&nbsp;</td>';
	str += '<td>Offer&nbsp;</td><td';
	if	(rdAvailTech.length > 0)
	{
		str += ' onClick="rdfType=0;document.getElementById(\'rdstype0\').checked=true" >';
		str += '<input type="radio" ' + rett[40] + ' name="rdstype" id="rdstype0" value="0"';
		if	(rdfType == 0)
			str += ' checked="checked"';
		str += ' onClick="rdfType = 0;" /';
	}
	str += '> research assistance</td><td>to player <input ' + rett[41] + ' type="text" name="rdsto" id="rdsto" value="';
	str += '" onChange="rdfTarget=this.value" /> in exchange for &euro;<input  ' + rett[42] + ' type="text" name="rdsprice" id="rdsprice" value="';
	str += '" onChange="rdfPrice=this.value" /></td><td class="div10"><input  ' + rett[43] + ' type="button" value="Send offer" ';
	str += 'onClick="sendOffer();return false" /></td></tr>';
	if	(rdAvailTech.length > 0)
	{
		str += '<tr><td colspan="2">&nbsp;</td><td onClick="rdfType=1;document.getElementById(\'rdstype1\').checked=true"';
		str += '><input type="radio" ' + rett[44] + ' name="rdstype" id="rdstype1" value="1"';
		if	(rdfType == 1)
			str += ' checked="checked"';
		str += ' onClick="rdfType = 1;" /> the <select ' + rett[45] + ' name="rdstech" id="rdstech"';
		str += ' onChange="rdfTech=parseInt(this.options[this.selectedIndex].value, 10)">';
		str += '<option value="0">-- select --</option>';
		for	(i=0;i<rdAvailTech.length;i++)
		{
			str += '<option value="'+rdAvailTech[i]+'"';
			if	(rdAvailTech[i] == rdfTech)
				str += ' selected="selected"';
			str += '>' + rtTitles[rdAvailTech[i]] + '</option>';
		}
		str += '</select> technology</td><td colspan="2">&nbsp;</td></tr>';
	}
	str += '</table>';

	document.getElementById('rdgive').innerHTML = str;
	document.getElementById('rdsto').value = rdfTarget;
	document.getElementById('rdsprice').value = rdfPrice;
}


function	drawRDPending()
{
	var	str = '<h1>Received assistance offers</h1>';
	if	(rdRecOffers.length == 0)
		str += '<p>No assistance offers have been received.</p>';
	else
	{
		var	n, m, i;
		n = rdRecOffers.length;
		m = n % 5;
		n = (n-m)/5 + (m>0?1:0);
		if	(rdRecPage >= n)
			rdRecPage = n-1;

		str += '<table cellspacing="0" cellpadding="0"><tr><td class="div20" rowspan="3">&nbsp;</td><td>Page ';
		if	(n == 1)
			str += "1 / 1";
		else
		{
			str += '<select  ' + rett[50] + ' name="rdrpage" id="rdrpage" onChange="rdRecToPage(this.options[this.selectedIndex].value)">';
			for	(i=0;i<n;i++)
			{
				str += '<option value="' + i + '"';
				if	(rdRecPage == i)
					str += ' selected="selected"';
				str += '>' + (i+1) + '</option>';
			}
			str += '</select> / ' + n;
		}
		str += '</td></tr><tr><td>&nbsp;</td></tr><tr><td><table class="list" cellspacing="0" cellpadding="0">';
		var	r = new Array(
				'Unfortunately, we do not have enough cash to accept this offer.',
				'Since we have already accepted another offer in the past 22 hours, we cannot accept this offer.',
				'However, we have already discovered or are about to discover this technology.',
				'However, we do not have the scientific knowledge required to accept this offer.'
			);
		for	(i=5*rdRecPage;i<rdRecOffers.length&&i<5*(rdRecPage+1);i++)
		{
			str += '<tr><td>Player <b>' + rdRecOffers[i].player + '</b> has offered to give us ';
			if	(rdRecOffers[i].tech == "")
				str += '<b>scientific assistance</b>';
			else
				str += 'the "<b>' + rtTitles[rdRecOffers[i].tech] + '</b>" technology';
			if	(rdRecOffers[i].price > 0)
				str += ' in exchange for <b>&euro;' + formatNumber(rdRecOffers[i].price) + '</b>';
			str += '. The offer was received at ' + formatDate(rdRecOffers[i].time) + '.<br/>';

			if	(rdRecOffers[i].status2 == 0)
			{
				str += '<input ' + rett[51] + ' type="button" value="Accept" onClick="acceptOffer(' + rdRecOffers[i].id + ');return false" />';
				str += ' <input ' + rett[52] + ' type="button" value="Decline" onClick="rejectOffer(' + rdRecOffers[i].id + ');return false" />';
			}
			else
			{
				str += r[rdRecOffers[i].status2 - 1] + ' You may either decline the offer or let it expire.<br/>';
				str += '<input type="button" ' + rett[52] + ' value="Decline" onClick="rejectOffer(' + rdRecOffers[i].id + ');return false" />';
			}

			str += '<hr/></td></tr>';
		}
		str += '</table></td></tr></table>';
	}
	document.getElementById('rdrec').innerHTML = str;
}


function	drawRDHistory()
{
	var	str = '<h1>History</h1>';
	if	(rdHistory.length == 0)
		str += '<p>The history is empty.</p>';
	else
	{
		var	n, m, i;
		n = rdHistory.length;
		m = n % 5;
		n = (n-m)/5 + (m>0?1:0);
		if	(rdHistPage >= n)
			rdHistPage = n-1;

		str += '<table cellspacing="0" cellpadding="0"><tr><td class="div20" rowspan="3">&nbsp;</td><td>Page ';
		if	(n == 1)
			str += "1 / 1";
		else
		{
			str += '<select ' + rett[60] + ' name="rdhpage" id="rdhpage" onChange="rdHistToPage(this.options[this.selectedIndex].value)">';
			for	(i=0;i<n;i++)
			{
				str += '<option value="' + i + '"';
				if	(rdHistPage == i)
					str += ' selected="selected"';
				str += '>' + (i+1) + '</option>';
			}
			str += '</select> / ' + n;
		}
		str += '</td></tr><tr><td>&nbsp;</td></tr><tr><td><table class="list" cellspacing="0" cellpadding="0">';
		for	(i = 5 * rdHistPage; i < rdHistory.length && i < 5 * (rdHistPage + 1); i ++) {
			str += '<tr><td>';
			if	(rdHistory[i].type == "R")
				str +='Player <b>' + rdHistory[i].player + '</b> offered to give us ';
			else
				str +='We offered to give player <b>' + rdHistory[i].player + '</b> ';
			if	(rdHistory[i].tech == "")
				str += '<b>scientific assistance</b>';
			else
				str += 'the "<b>' + rtTitles[rdHistory[i].tech] + '</b>" technology';
			if	(rdHistory[i].price > 0)
				str += ' in exchange for <b>&euro;' + formatNumber(rdHistory[i].price) + '</b>';
			str += '. The offer was ' + (rdHistory[i].type == "R" ? 'received' : 'sent') + ' at ' + formatDate(rdHistory[i].time);
			str += ' and ';
			if	(rdHistory[i].status == "E")
				str += "<b>expired</b> one day later.";
			else if	(rdHistory[i].status == "A")
				str += "was <b>accepted</b>.";
			else
				str += "was <b>declined</b>.";

			str += '<hr/></td></tr>';
		}
		str += '</table></td></tr></table>';
	}
	document.getElementById('rdhist').innerHTML = str;
}


function	confirmSendOffer(tp, tc, pl, pr)
{
	var	str = "You are about to offer ";
	if	(tp == 0)
		str += "scientific assistance";
	else
		str += 'data on the "' + rtTitles[tc] + '" technology';
	str += '\nto ' + pl;
	if	(pr > 0)
		str += ' in exchange for ' + formatNumber(pr.toString()) + ' euros.';
	str += '\nYou will not be able to make another offer for 24 hours.';
	return	confirm(str);
}


function	rdOfferError(ec)
{
	var	str = "Error :\n";

	switch	(ec)
	{
		case 1: str += "You have not selected a technology to offer."; break
		case 2: str += "You have not specified a player to send the offer to."; break;
		case 3: str += "The price you specified is invalid."; break;
		case 4: str += "This player does not exist."; break;
		case 5: str += "You do not seem to have this technology."; break;
		case 6: str += "You can't send a diplomatic offer to yourself."; break;
		case 7: str += "This player is on vacation."; break;
		case 8: str += "Invalid data received."; break;
		case 9: str += "It seems you have already sent an offer in the past 24h."; break;
		case 10: str += "This player is under Peacekeeper protection."; break;
		case 200: str += "You can't send diplomatic offers while in vacation mode."; break;
		case 201: str += "You can't send diplomatic offers while under Peacekeeper protection."; break;
		default: str += "An unknown error has happened ("+ec+")."; break;
	}

	alert(str);
}


function rdAcceptError(ec) {
	var	str = "Error :\n";

	switch (ec) {
		case 1: str += "You do not have enough cash to accept this offer."; break
		case 2: str += "You have already accepted this offer."; break;
		case 3: str += "You have already declined this offer."; break;
		case 4: str += "Unknown offer identifier."; break;
		case 5: str += "You don't have the dependencies for this technology."; break;
		case 200: str += "You can't accept or reject diplomatic offers while in vacation mode."; break;
		case 201: str += "You can't accept diplomatic offers while under Peacekeeper protection."; break;
		default: str += "An unknown error has happened ("+ec+")."; break;
	}

	alert(str);
}


function	confirmAcceptOffer(i)
{
	var	str = "Are you sure you want to accept " + rdRecOffers[i].player + "'s offer to\ngive us ";
	if	(rdRecOffers[i].type == "R")
		str += "scientific assistance";
	else
		str += "the \"" + rtTitles[rdRecOffers[i].tech] + "\" technology";
	if	(rdRecOffers[i].price > 0)
		str += " in exchange for\n" + formatNumber(rdRecOffers[i].price) + " euros";
	str += "?";
	return	confirm(str);
	
}


function	confirmRejectOffer(i)
{
	var	str = "Are you sure you want to decline " + rdRecOffers[i].player + "'s offer to\ngive us ";
	if	(rdRecOffers[i].type == "R")
		str += "scientific assistance";
	else
		str += "the \"" + rtTitles[rdRecOffers[i].tech] + "\" technology";
	if	(rdRecOffers[i].price > 0)
		str += " in exchange for\n" + formatNumber(rdRecOffers[i].price) + " euros";
	str += "?";
	return	confirm(str);
	
}

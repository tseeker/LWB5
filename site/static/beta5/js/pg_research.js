var	pageMode;
var	pageModes = new Array('Topics', 'Laws', 'Attributions', 'Diplomacy');
var	puTimer;

var	onGetComplete;
var	rtGetData = new Array();	// Research topics for which data must be requested
var	rtTitles = new Array();		// Research topics titles
var	rtDescriptions = new Array();	// Research topics descriptions
var	rtCosts = new Array();		// Research topics implementation cost
var	rtTypes = new Array();		// Research topics types
var	rtExpanded = new Array();	// Topics for which the description is being displayed

var	rtAll;				// All research topics or laws

var	rtCompleted;			// Completed researches
var	rtImplemented;			// Implemted technologies
var	rtForeseen;			// Foreseen breakthroughs
var	rtForeseenP;			// Foreseen breakthroughs completion percentages

var	lEnacted;			// Enacted laws
var	lTimeRevoke;			// Time left before a law can be revoked
var	lAvailable;			// Available laws
var	lTimeEnact;			// Time left before a law can be enacted again

var	rbPoints;			// Research points / day
var	rbPercentage;			// Percentage going to each research type
var	rbOriPercentage;		// Percentage going to each research type before player modification
var	rbCatPoints;			// Points going to each category
var	rbLocks;			// Locked categories

var	rdDiplomacyDelay;
var	rdSentOffer;			// Offer sent by the player, false if none
var	rdRecOffers;			// List of received offers
var	rdHistory;			// List of past offers
var	rdAvailTech;			// List of available technologies
var	rdRecPage = 0;			// Current page for the received offers list
var	rdHistPage = 0;			// Current page for the history
var	rdfType = 0;
var	rdfTech = 0;
var	rdfTarget = "";
var	rdfPrice = "";

var	rett;

function	initResearchPage(data)
{
	setPage(data);
}


function	showDescription(id)
{
	rtExpanded.push(id);
	if	(pageMode == 'Topics')
		topicsDisplayOk();
	else if	(pageMode == 'Laws')
		lawsDisplayOk();
}


function	hideDescription(id)
{
	var	t = rtExpanded, i;
	rtExpanded = new Array();
	for	(i=0;i<t.length;i++)
		if	(t[i] != id)
			rtExpanded.push(t[i]);
	if	(pageMode == 'Topics')
		topicsDisplayOk();
	else if	(pageMode == 'Laws')
		lawsDisplayOk();
}


function	sortResearch(a,b)
{
	var ta=rtTitles[a],tb=rtTitles[b];
	return	(ta > tb ? 1 : -1);
}


function	descriptionsReceived(data)
{
	if	(data != "")
	{
		var	i, l, a = data.split("\n");
		l = a.length / 4;
		for	(i=0;i<l;i++)
		{
			var	tmp = a[i*4+2].split('#');
			rtTitles[a[i*4]] = a[i*4+1];
			rtCosts[a[i*4]] = tmp[0];
			rtTypes[a[i*4]] = tmp[1];
			rtDescriptions[a[i*4]] = a[i*4+3];
		}
	}

	rtGetData = new Array();
	eval(onGetComplete);
}


function	setPage(mode)
{
	pageMode = mode;
	if	(puTimer)
		clearTimeout(puTimer);
	writeLinks();
	document.getElementById('respage').innerHTML = getLoadingText();

	var	str = 'x_get'+mode+'Data(build'+mode+'Display);'
	eval(str);
}


function	writeLinks()
{
	var	str = '', i;
	for	(i=0;i<pageModes.length;i++)
	{
		if	(str != '')
			str += ' - ';
		if	(pageModes[i] != pageMode)
			str += '<a href="#" ' + rett[70+i] + '  onClick="x_setMode(\'' + pageModes[i] + '\', setPage); return false;">';
		else
			str += '<b><u>';
		str += pageTitles[i];
		if	(pageModes[i] != pageMode)
			str += '</a>';
		else
			str += '</u></b>';
	}
	document.getElementById('respsel').innerHTML = str;
}




function	buildTopicsDisplay(data)
{
	var	all = data.split('!');
	var	t, i;

	rtCompleted = (all[1] == "" ? new Array() : all[1].split('#'));
	rtAll = rtCompleted;
	rtImplemented = (all[0] == "" ? new Array() : all[0].split('#'));
	rtAll = rtAll.concat(rtImplemented);

	t = all[2].split('#');
	rtForeseen = new Array();
	rtForeseenP = new Array();
	for	(i=0;all[2] != "" && i<t.length/2;i++)
	{
		rtForeseen[i] = t[i*2];
		rtForeseenP[t[i*2]] = t[i*2+1];
	}
	rtAll = rtAll.concat(rtForeseen);

	for	(i=0;i<rtAll.length;i++)
	{
		if	(!rtTitles[rtAll[i]])
			rtGetData.push(rtAll[i]);
	}

	if	(rtGetData.length > 0)
	{
		onGetComplete = 'topicsDisplayOk();';
		x_getDescriptions(rtGetData.join('#'), descriptionsReceived);
	}
	else
		topicsDisplayOk();

	puTimer = setTimeout('x_getTopicsData(buildTopicsDisplay)', 900000);
}


function	topicsDisplayOk()
{
	rtCompleted.sort(sortResearch);
	rtImplemented.sort(sortResearch);
	rtForeseen.sort(sortResearch);
	document.getElementById('respage').innerHTML = getTopicsPage();
}


function implementResult(data) {
	if (data == "OK") {
		updateHeader();
		setPage('Topics');
	} else {
		var a = data.split('#');
		implError(parseInt(a[1], 10));
	}
}


function	implementTechnology(id)
{
	if	(!confirmTech(id))
		return;
	x_implementTechnology(id, implementResult);
}




function	buildLawsDisplay(data)
{
	var	t, i;

	t = data.split('#');
	lEnacted = new Array();
	lTimeRevoke = new Array();
	lAvailable = new Array();
	lTimeEnact = new Array();
	for	(i=0;data != "" && i<t.length/2;i++)
	{
		var j = parseInt(t[i*2+1], 10);
		if	(j <= 0)
		{
			lAvailable.push(t[i*2]);
			lTimeEnact[t[i*2]] = -j;
		}
		else
		{
			lEnacted.push(t[i*2]);
			lTimeRevoke[t[i*2]] = (j-1);
		}
	}
	rtAll = lEnacted.concat(lAvailable);

	for	(i=0;i<rtAll.length;i++)
	{
		if	(!rtTitles[rtAll[i]])
			rtGetData.push(rtAll[i]);
	}

	if	(rtGetData.length > 0)
	{
		onGetComplete = 'lawsDisplayOk();';
		x_getDescriptions(rtGetData.join('#'), descriptionsReceived);
	}
	else
		lawsDisplayOk();

	puTimer = setTimeout('x_getLawsData(buildLawsDisplay)', 900000);
}


function	lawsDisplayOk()
{
	lEnacted.sort(sortResearch);
	lAvailable.sort(sortResearch);
	document.getElementById('respage').innerHTML = getLawsPage();
}


function switchLawResult(data) {
	if (data == "OK") {
		updateHeader();
		setPage('Laws');
	} else {
		var a = data.split('#');
		slawError(parseInt(a[1], 10));
	}
}


function	revokeLaw(id)
{
	if	(!confirmRevoke(id))
		return;
	x_switchLaw(id, switchLawResult);
}


function	enactLaw(id)
{
	if	(!confirmEnact(id))
		return;
	x_switchLaw(id, switchLawResult);
}




function	buildAttributionsDisplay(data)
{
	var	a = data.split('#'), i;

	rbPoints = a[0];
	rbOriPercentage = new Array();
	for	(i=0;i<rTypes.length;i++)
		rbOriPercentage[i] = parseInt(a[i+1], 10);
	if	(!rbPercentage)
	{
		rbPercentage = (new Array()).concat(rbOriPercentage);
		rbLocks = new Array();
	}
	computeCatPoints();

	if	(!document.getElementById('totRPoints'))
		drawAttributionsDisplay();
	updateAttributionsValues();

	puTimer = setTimeout('x_getAttributionsData(buildAttributionsDisplay)', 900000);
}


function	computeCatPoints()
{
	var	i, s;

	s = 0;
	rbCatPoints = new Array();
	for	(i=0;i<rTypes.length;i++)
	{
		rbCatPoints[i] = Math.floor(rbPercentage[i] * rbPoints / 100);
		s += rbCatPoints[i];
	}

	for	(i=0;s<rbPoints;i=(i+1)%rTypes.length)
	{
		rbCatPoints[i] ++;
		s++;
	}
}


function	updateAttributionsValues()
{
	var	i, l, s, t, d;
	document.getElementById('totRPoints').innerHTML = formatNumber(rbPoints);
	l = (rbLocks.length == rTypes.length - 2);
	s = '!' + rbLocks.join('!') + '!';
	d = false;
	for	(i=0;i<rTypes.length;i++)
	{
		document.getElementById('rbval' + i).innerHTML = rbPercentage[i];
		document.getElementById('rbpts' + i).innerHTML = formatNumber(rbCatPoints[i].toString());
		if	(s.indexOf('!' + i + '!') != -1)
			t = '<img  ' + rett[80] + ' src="'+staticurl+'/beta5/pics/lock_'+color+'.gif" alt="[L]" onClick="switchLock('+i+');" />';
		else if	(l)
			t = '&nbsp;';
		else
			t = '<img ' + rett[81] + ' src="'+staticurl+'/beta5/pics/unlock_'+color+'.gif" alt="[U]" onClick="switchLock('+i+');" />';
		document.getElementById('rblock'+i).innerHTML = t;
		d = d || (rbPercentage[i] != rbOriPercentage[i]);
	}
	if	(d)
		drawBudgetControls();
	else
	{
		document.getElementById('rbcl').innerHTML = '&nbsp;';
		document.getElementById('rbcr').innerHTML = '&nbsp;';
	}
}


function	switchLock(l)
{
	var	i, a, s = '!' + rbLocks.join('!') + '!';
	if	(s.indexOf('!' + l + '!') == -1)
		rbLocks.push(l);
	else
	{
		a = new Array();
		for	(i=0;i<rbLocks.length;i++)
			if	(rbLocks[i] != l)
				a.push(rbLocks[i]);
		rbLocks = a;
	}
	updateAttributionsValues();
}


function	changeAttr(a, v)
{
	var	i, j, k;
	var	rv, orv;
	var	ls = '!' + rbLocks.join('!') + '!', lv;
	var	mv = new Array(), mi = new Array(), m;

	if	(rbPercentage[a] + v < 0)
		rv = -rbPercentage[a];
	else if	(rbPercentage[a] + v  > 100)
		rv = 100 - rbPercentage[a];
	else
		rv = v;
	if	(rv == 0)
		return;

	mv = mv.concat(rbPercentage);
	mv.sort(new Function('a', 'b', 'return a - b'));
	m = '';
	for	(i=0;i<mv.length;i++)
	{
		k = (rv < 0) ? i : (mv.length - (i+1));
		for	(j=0;j<mv.length;j++)
			if	(	mv[k] == rbPercentage[j]
				&&	m.indexOf('!' + j + '!') == -1
				&&	ls.indexOf('!' + j + '!') == -1
				&&	j != a
				)
			{
				mi.push(j);
				break;
			}
		m = '!' + mi.join('!') + '!';
	}

	rs = Math.abs(rv)/rv;
	orv = 0;
	for	(i=0;rv!=0&&orv!=mi.length;i=(i+1)%mi.length)
	{
		orv ++;
		j = mi[i];
		if	(rbPercentage[j] - rs < 0 || rbPercentage[j] - rs > 100)
			continue;

		rbPercentage[a] += rs;
		rbPercentage[j] -= rs;
		rv -= rs;
		orv = 0;
	}

	computeCatPoints();
	updateAttributionsValues();
}


function	resetBudget()
{
	rbPercentage = (new Array()).concat(rbOriPercentage);
	rbLocks = new Array();
	computeCatPoints();
	updateAttributionsValues();
}


function	validateBudget()
{
	var	s = rbPercentage.join('#');
	x_validateBudget(s, budgetValidated);
}


function budgetValidated(data) {
	if (data == "OK") {
		rbOriPercentage = (new Array()).concat(rbPercentage);
		computeCatPoints();
		updateAttributionsValues();
	} else {
		var a = data.split('#');
		budgetError(parseInt(a[1], 10));
	}
}



function	ResearchOffer(id,type,status,status2,price,tech,time,player)
{
	this.id = id;
	this.type = type;
	this.status = status;
	this.status2 = status2;
	this.price = price;
	this.tech = tech;
	this.time = parseInt(time, 10);
	this.player = player;

	var	d = new Date();
	var	t = d.getTime();
	t = (t - (t % 1000)) / 1000;
	this.today = (t - this.time <= rdDiplomacyDelay);
}


function	buildDiplomacyDisplay(data)
{
	if	(data.indexOf('\n') == -1)
		drawDiplomacyRestrained(data);
	else
	{
		var	i, o, a = data.split('\n'), tl;

		rdDiplomacyDelay = parseInt(a.shift(), 10);
		tl = a.shift().split('#');
		rdAvailTech = new Array();
		rtGetData = new Array();
		for	(i=0;i<tl.length;i++)
		{
			if	(tl[i] == "")
				continue;
			rdAvailTech.push(tl[i]);
			if	(!rtTitles[tl[i]])
				rtGetData.push(tl[i]);
		}
		tl = '!' + rtGetData.join('!!') + '!';

		rdSentOffer = false;
		rdRecOffers = new Array();
		rdHistory = new Array();
		if	(a.length > 1)
		{
			for	(i=0;i<a.length;i+=2)
			{
				var	l = a[i].split('#');
				o = new ResearchOffer(l[0],l[1],l[2],l[6],l[3],l[4],l[5],a[i+1]);
				if	(o.type == "S" && o.today)
					rdSentOffer = o;
				else if	(o.type == "R" && o.status == "P")
					rdRecOffers.push(o);
				else
					rdHistory.push(o);
				if	(o.tech != "" && !rtTitles[o.tech] && tl.indexOf('!' + o.tech + '!') == -1)
				{
					rtGetData.push(o.tech);
					tl += '!' + o.tech + '!';
				}
			}

		}

		drawDiplomacyLayout();
		if	(rtGetData.length > 0)
		{
			onGetComplete = 'drawDiplomacyPage();';
			x_getDescriptions(rtGetData.join('#'), descriptionsReceived);
		}
		else
			drawDiplomacyPage();
	}

	puTimer = setTimeout('x_getDiplomacyData(buildDiplomacyDisplay)', 900000);
}

function	drawDiplomacyLayout()
{
	var	str = '<form action="?"><table cellspacing="0" cellpadding="0"><tr><td colspan="2" id="rdgive">&nbsp;</td></tr>';
	str += '<tr><td colspan="2">&nbsp;</td></tr>';
	str += '<tr><td class="div2" id="rdrec">&nbsp;</td><td class="div2" id="rdhist">&nbsp;</td></tr></table></form>';
	document.getElementById('respage').innerHTML = str;
}

function	drawDiplomacyPage()
{
	if	(typeof rdSentOffer == "object")
		drawRDSentAlready();
	else
	{
		rdAvailTech.sort(sortResearch);
		drawRDSendForm();
	}
	drawRDPending();
	drawRDHistory();
}


function	sendOffer()
{
	if	(rdfType == 1 && rdfTech == 0)
	{
		rdOfferError(1);
		return;
	}

	if	(rdfTarget == "")
	{
		rdOfferError(2);
		return;
	}

	var	p;
	if	(rdfPrice != "")
	{
		p = parseInt(rdfPrice, 10);
		if	(rdfPrice != p.toString() || p < 0)
		{
			rdOfferError(3);
			return;
		}
	}
	else
		p = 0;

	if	(!confirmSendOffer(rdfType, rdfTech, rdfTarget, p))
		return;

	x_sendOffer(rdfType, rdfTech, rdfTarget, p, offerSent);
}


function	offerSent(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		var	a = data.split("#");
		a[1] = parseInt(a[1], 10);
		rdOfferError(a[1]);
	}
	else
		buildDiplomacyDisplay(data);
}


function	acceptOffer(oid)
{
	var	i;
	for	(i=0;i<rdRecOffers.length&&rdRecOffers[i].id!=oid;i++) ;
	if	(i == rdRecOffers.length)
		return;

	if	(!confirmAcceptOffer(i))
		return;

	x_acceptOffer(oid, offerAccepted);
}


function	rejectOffer(oid)
{
	var	i;
	for	(i=0;i<rdRecOffers.length&&rdRecOffers[i].id!=oid;i++) ;
	if	(i == rdRecOffers.length)
		return;

	if	(!confirmRejectOffer(i))
		return;

	x_declineOffer(oid, offerAccepted);
}


function	offerAccepted(data)
{
	if	(data.indexOf('ERR#') == 0)
	{
		var	a = data.split("#");
		a[1] = parseInt(a[1], 10);
		rdAcceptError(a[1]);
	}
	else
	{
		buildDiplomacyDisplay(data);
		updateHeader();
	}
}


function	rdHistToPage(p)
{
	rdHistPage = parseInt(p, 10);
	drawRDHistory();
}

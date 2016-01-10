var	colTypes = new Array('st', 'sub', 'date', 'from', 'to');
var	msgSortGetData = [
		'(x.status == "N") ? 2 : ((x.status == "r") ? 1 : 0)',
		'x.subject.toLowerCase()', 'x.moment',
		'x.from.toLowerCase()', 'x.to.toLowerCase()'
	];
var	folders = new Array();
var	fpBuilding = false;
var	flUpdate;

var	page;
var	vFolder;
var	pCompose;
var	udFolder;
var	udFList;

var	msgtt;


function	emptyCB(data) { }


function	Folder(t, i, n, nc, mc)
{
	this.type = t;
	this.id = i;
	this.name = n;
	this.nCount = nc;
	this.mCount = mc;
	this.messages = new Array();
	this.settings = new Array();
	this.threadOrder = new Array();
	this.selAll = false;
	this.selected = false;
	this.deleteMessage = Folder_deleteMessage;
	this.prepareThreading = Folder_prepareThreading;
}

function	Folder_deleteMessage(idm)
{with(this) {
	var	i;
	for	(i=0;i<messages.length&&messages[i].id!=idm;i++) ;
	if	(i==messages.length)
		return;
	if	(messages[i].status == 'N')
		nCount --;
	messages.splice(i, 1);
}}

function	Folder_prepareThreading()
{with(this){
	var	i, j, ir = new Array();
	for	(i=0;i<messages.length;i++)
	{
		messages[i].replies = new Array();
		for	(j=0;j<messages.length;j++)
			if	(j!=i && messages[j].replyTo == messages[i].id)
			{
				messages[i].replies.push(j);
				ir[j] = i;
			}
	}

	threadOrder = new Array();
	for	(i=0;i<messages.length;i++)
	{
		if	(typeof ir[i] != "undefined")
			continue;
		threadedMessage(this, i, 0);
	}
}}

function	threadedMessage(fld, m, d)
{with(fld){
	var	i;
	threadOrder.push(m);
	messages[m].depth = d;
	for	(i=0;i<messages[m].replies.length;i++)
		threadedMessage(fld, messages[m].replies[i], d+1);
}}


function	Message(id, moment, status, sl, rl, repl, from, to, sub, rtid)
{
	this.id		= id;
	this.moment	= parseInt(moment, 10);
	this.status	= status;
	this.slink	= sl;
	this.rlink	= rl;
	this.replink	= repl;
	this.from	= from;
	this.to		= to;
	this.subject	= sub;
	this.replyTo	= rtid;
	this.replies	= new Array();
	this.depth	= 0;
	this.message	= "";
	this.isopen	= false;
	this.selected	= false;
}


function	ComposeParameters(tt, tid, rid)
{
	this.tType = tt;	// Player/Planet/Alliance
	this.tId = tid;		// Target identifier
	this.tText = "";	// Target text
	this.sText = "";	// Subject
	this.cText = "";	// Contents
	this.rId = rid;		// Reply to
	this.rText = "";	// Reply text
	this.errors = new Array(0, 0, 0);
	this.sent = false;
	this.ready = false;
	this.check = checkComposeMessage;
}


function	checkComposeMessage()
{with(this){
	sent = false;
	if	(tText == "")
		errors[0] = 1;
	else
		errors[0] = 0;

	if	(sText == "")
		errors[1] = 1;
	else if	(sText.length > 64)
		errors[1] = 2;
	else
		errors[1] = 0;

	if	(cText == "")
		errors[2] = 1;
	else
		errors[2] = 0;

	return	(errors[0] == 0 && errors[1] == 0 && errors[2] == 0);
}}



function	makeSortFunction(type,dir)
{
	var	str = 'function(a,b){var x,s1,s2,i,rv=0,l=msgSortGetData.length;'
			+ 'for(i=0;i<l&&rv==0;i++){x=a;eval(\'' + (dir ? 's1' : 's2')
			+ '=(\'+msgSortGetData[(i+'+type+')%l]+\')\');x=b;eval(\''
			+ (dir ? 's2' : 's1')+'=(\'+msgSortGetData[(i+'+type+')%l]+\')\');'
			+ 'rv=((s1<s2)?-1:((s1>s2)?1:0));}return rv;}';
	return	str;
}


function	initMessages()
{
	// Initialise sort functions
	var	i,j;
	for	(i=0;i<colTypes.length;i++)
	{
		eval('ms_' + colTypes[i] + '_asc = ' + makeSortFunction(i,true));
		eval('ms_' + colTypes[i] + '_desc = ' + makeSortFunction(i,false));
	}

	var	data = document.getElementById('jsmsgflist').innerHTML;
	if	(data.split('\n').length == 1)
	{
		// IE really sucks.
		x_updateFolders(initializeFoldersIE);
		return;
	}
	parseFolderList(data);
	initializeLayout();

	data = document.getElementById('jsmsginit').innerHTML.split('#');
	page = data.shift();
	eval('setParameters' + page + '(data)');
	drawMainMenu();
}


function	initializeFoldersIE(d1)
{
	parseFolderList(d1);
	initializeLayout();

	var	data = document.getElementById('jsmsginit').innerHTML.split('#');
	page = data.shift();
	eval('setParameters' + page + '(data)');
	drawMainMenu();
}


function	parseFolderList(data)
{
	var	lst = data.split('\n');
	var	l = lst.length, i, j;
	var	nFolders = new Array();
	var	init = (folders.length == 0);
	var	cft, cfi;
	var	afc = new Array();

	if	(!init && page == 'Browse')
	{
		cft = folders[vFolder].type;
		cfi = folders[vFolder].id;
	}

	for	(i=j=0;i<l;i+=2,j++)
	{
		var	of, a;
		a = lst[i].split('#');
		nFolders[j] = new Folder(a[0], a[1], lst[i+1], a[2], a[3]);
		of = findFolder(a[0], a[1]);
		if	(of == -1)
			continue;
		afc[j] = folders[of].nCount;
		nFolders[j].settings = folders[of].settings;
		nFolders[j].messages = folders[of].messages;
		nFolders[j].selAll = folders[of].selAll;
		nFolders[j].selected = folders[of].selected;
		nFolders[j].threadOrder = folders[of].threadOrder;
	}

	folders = nFolders;
	if	(!init)
	{
		if	(page == 'Browse')
		{
			idx = findFolder(cft, cfi);
			if	(idx == -1)
			{
				page = 'NotFound';
				drawPageNotFound();
			}
			else
			{
				i = vFolder;
				vFolder = idx;
				if	(afc[i] < folders[idx].nCount)
					browseFolder(idx);
			}
		}
		else if	(page == "Folders")
			buildFoldersList();
		drawMainMenu();
	}

	flUpdate = (new Date()).getTime();
	udFList = setTimeout('x_updateFolders(parseFolderList)', 15000);
}


function	findFolder(t, id)
{
	var	i;
	for	(i=0;i<folders.length;i++)
		if	(folders[i].type == t && (t != 'CUS' || (t == 'CUS' && folders[i].id == id)))
			return	i;
	return	-1;
}


function initializeLayout() {
	document.getElementById('jsmsg').innerHTML = '<table cellspacing="0" cellpadding="0"><tr><td class="mmenu" id="mmenu">&nbsp;</td><td id="mbody">&nbsp;</td>'
		+ '<td style="width:5%"><a href="manual?p=messages">Help</a></td></tr></table>';
}



//--------------------
// COMPOSE MESSAGE
//--------------------

function	composeNew()
{
	page = "Compose";
	setParametersCompose(new Array("","",""));
	drawMainMenu();
}

function	messageTo(t,n)
{
	page = "Compose";
	setParametersCompose(new Array(t.toString(),n.toString(),""));
	drawMainMenu();
}

function	composeReply(t,n,r)
{
	page = "Compose";
	setParametersCompose(new Array(t.toString(),n.toString(),r));
	drawMainMenu();
}

function	setParametersCompose(p)
{
	var	tt, tid, trp;
	tt = p[0] ? p[0] : "p";
	tid = p[1] ? p[1] : -1;
	trp = p[2] ? p[2] : -1;
	pCompose = new ComposeParameters(tt, tid, trp);
	if	(tid != -1)
	{
		x_getTargetName(tt, tid, targetNameReceived);
		setTimeout('checkComposeReady()', 500);
	}
	else
	{
		pCompose.ready = true;
		buildComposePage();
	}
}

function	targetNameReceived(data)
{
	pCompose.tText = data;
	if	(pCompose.rId != -1)
		x_getMessageText(pCompose.rId, gotOriginalMessage);
	else
		pCompose.ready = true;
}

function	gotOriginalMessage(data)
{
	var	a = data.split('\n');
	a.shift();
	if	(a[0].indexOf('Re:') == 0)
		pCompose.sText = a.shift();
	else
		pCompose.sText = 'Re: ' + a.shift();
	if	(a.length > 0)
		pCompose.rText = a.join('\n');
	pCompose.ready = true;
}

function	checkComposeReady()
{
	if	(pCompose.ready)
		buildComposePage();
	else
		setTimeout('checkComposeReady()', 500);
}

function	buildComposePage()
{
	drawComposePage();
	document.getElementById('mcmsg').value = pCompose.cText;
}

function	composePageStatus(s)
{
	var	allids = new Array('mcmtype', 'mcmto', 'mcmsub', 'mcmsg', 'mcmsend');
	var	i;
	for	(i=0;i<allids.length;i++)
		document.getElementById(allids[i]).disabled = !s;
}

function	sendMessage()
{
	composePageStatus(false);
	pCompose.tType = document.getElementById('mcmtype').value;
	pCompose.tText = document.getElementById('mcmto').value;
	pCompose.sText = document.getElementById('mcmsub').value;
	pCompose.cText = document.getElementById('mcmsg').value;
	if	(!pCompose.check())
	{
		buildComposePage();
		composePageStatus(true);
	}
	else
		x_sendMessage(pCompose.tType, pCompose.tText, pCompose.sText, pCompose.cText, pCompose.rId, messageSent);
}

function	messageSent(data)
{
	if	(data == "OK")
	{
		pCompose.tType = 0;
		pCompose.tText = pCompose.sText = pCompose.cText = "";
		pCompose.tId = pCompose.rId = -1;
		pCompose.sent = true;
	}
	else
	{
		var	i, d = data.split('#');
		for	(i=0;i<d.length;i++)
			pCompose.errors[i] = parseInt(d[i], 10);
	}
	buildComposePage();
	composePageStatus(true);
}



//--------------------
// BROWSE FOLDER
//--------------------

function	browseFolder(i)
{
	clearTimeout(udFolder);
	page = 'Browse';
	vFolder = i;
	buildFolderPage();
	drawMainMenu();
}

function	setParametersBrowse(p)
{
	var	idx;
	if	(p.length == 1 && p[0] != "-1")
		idx = findFolder(p[0], 0);
	else if	(p.length == 1)
		idx = -1;
	else
		idx = findFolder(p[0], p[1]);

	if	(idx == -1)
		page = 'NotFound';
	else
		vFolder = idx;

	buildFolderPage();
}

function	setParametersNotFound(p)
{
	drawPageNotFound();
}

function	buildFolderPage()
{
	fpBuilding = true;
	if	(page == 'NotFound')
	{
		drawPageNotFound();
		return;
	}
	drawFolderLayout();
	if	(folders[vFolder].settings.length == 0)
	{
		folders[vFolder].settings[4] = 0;
		x_getFolderSettings(folders[vFolder].type, folders[vFolder].id, settingsReceived);
	}
	else
	{
		drawFolderControls(folders[vFolder].settings);
		getMessageList();
	}
}

function	settingsReceived(data)
{
	if	(data != "")
	{
		var	a = data.split('\n');
		var	b = a[0].split('#');
		folders[vFolder].settings[0] = parseInt(b[0], 10);
		folders[vFolder].settings[1] = b[1];
		folders[vFolder].settings[2] = (b[2] == "1");
		folders[vFolder].settings[3] = (b[3] == "1");
		drawFolderControls(folders[vFolder].settings);
		getMessageList();
	}
	else
	{
		page = "NotFound";
		drawPageNotFound();
	}
}

function	getMessageList()
{
	if	(page != 'Browse')
		return;
	with(folders[vFolder]){
		var	a = new Array(), i, l = messages.length;
		for	(i=0;i<l;i++)
			a.push(messages[i].id);
		x_getMessageList(type, id, a.join('#'), messageListReceived);
	}
}

function	messageListReceived(data)
{
	var	ml = data.split('\n');
	var	i = 0, l = ml.length;

	while	(data != "" && i<l)
	{
		var	dl = ml[i].split('#');
		if	(dl[0] == '-')
			folders[vFolder].deleteMessage(dl[1]);
		else
		{
			dl.shift();
			var m = new Message(dl[0], dl[1], dl[2], dl[3], dl[4], dl[5], ml[i+1], ml[i+2], ml[i+3], dl[6]);
			m.selected = folders[vFolder].selAll;
			folders[vFolder].messages.push(m);
			i+=3;
		}
		i++;
	}
	if	(i>0)
	{
		if	(udFList)
			clearTimeout(udFList);
		x_updateFolders(parseFolderList);
	}
	if	(i>0||fpBuilding)
	{
		fpBuilding = false;
		buildMessageDisplay();
	}
}

function	buildMessageDisplay()
{with(folders[vFolder]){
	var	mcl = messages.length, mcm = mcl % settings[0];
	if	(mcl == 0)
	{
		drawEmptyFolder();
		return;
	}

	var	smet = 'messages.sort(ms_' + settings[1] + '_' + (settings[2] ? "asc" : "desc") + ')';
	eval(smet);
	if	(settings[3])
		prepareThreading();

	var	npg = (mcl - mcm) / settings[0] + (mcm > 0 ? 1 : 0);
	if	(settings[4] >= npg)
		settings[4] = npg - 1;
	drawPageSelector(npg, settings[4]);
	drawListHeader();
	drawList();
	updateControls();
	udFolder = setTimeout('getMessageList()', 30000);
}}


function	drawListHeader()
{with(folders[vFolder]){
	var	i, str = '<table class="msghdr" cellspacing="0" cellpadding="0">';
	str += '<tr><td class="msgview">&nbsp;</td>';
	str += '<td class="msgview"><input type="checkbox"' + msgtt[80] + 'name="selall" id="mlsa" onClick="switchSelAll();return true"';
	str += (selAll ? ' checked="checked"':'') + '/></td>';
	for	(i=0;i<colTypes.length;i++)
	{
		var	t = colTypes[i];
		str += '<th class="mct'+t+'" onClick="switchSort(\''+t+'\');"' + msgtt[81] + '>';
		if	(settings[1] == t)
			str += '<b>';
		str += colTitles[i];
		if	(settings[1] == t)
		{
			str += '</b><img src="'+staticurl+'/beta5/pics/';
			str += settings[2] ? "down" : "up";
			str += '_' + color + '.gif" alt="' + (settings[2] ? "down" : "up");
			str += '" />';
		}
		str += '</th>';
	}
	str += '<td class="msgview">&nbsp;</td></tr><tr><td id="msglist2" colspan="' + (colTypes.length+3);
	str += '">&nbsp;</td></tr></table>';
	document.getElementById('msglist').innerHTML = str;
}}


function	drawList()
{with(folders[vFolder]){
	var	i, j, m, str = '<table class="list" cellspacing="0" cellpadding="0">';
	var	crc = false;

	m = settings[4]*settings[0];
	for	(j=m;j<m+settings[0]&&j<messages.length;j++)
	{
		i = settings[3] ? threadOrder[j] : j;

		var	oct = ' onClick="switchOpen('+i+')"';
		str += '<tr><td><table cellspacing="0" cellpadding="0">';
		str += '<tr><td class="msgview">&nbsp;</td>';
		str += '<td class="msgview"><input type="checkbox" name="msg" ' + msgtt[82] + 'id="msgck'+i+'" value="';
		str += messages[i].id + '" onClick="switchSelection('+i+');"';
		str += (messages[i].selected ? ' checked="checked"' : '');
		str += '/></td><td class="mctst"'+oct+'><img src="'+staticurl+'/beta5/pics/msg';
		str += (messages[i].status == 'N' ? 'u' : (messages[i].status == 'r' ? 'r' : 'rep'));
		str += '.gif" alt="' + messages[i].status + '" /></td>';

		str += '<td class="mctsub"'+oct+'>';
		if	(settings[3])
		{
			var	k;
			for	(k=0;k<messages[i].depth;k++)
				str += '-&nbsp;';
		}
		if	(messages[i].status == 'N')
			str += '<b>';
		str += '<u' + msgtt[83] + '>'+messages[i].subject+'</u>';
		if	(messages[i].status == 'N')
			str += '</b>';
		str += '</td>';
		str += '<td class="mctdate"'+oct+'>' + formatDate(messages[i].moment) + '</td>';

		str += '<td class="mctfrom">';
		if	(messages[i].slink != "")
			str += '<a href="#"' + msgtt[84] + ' onClick="messageTo('+messages[i].slink+');return false">';
		str += messages[i].from;
		if	(messages[i].slink != "")
			str += '</a>';
		str += '</td>';

		str += '<td class="mctto">';
		if	(messages[i].rlink != "")
			str += '<a href="#"' + msgtt[84] + 'onClick="messageTo(' + messages[i].rlink + ');return false">';
		str += messages[i].to;
		if	(messages[i].rlink != "")
			str += '</a>';
		str += '</td>';

		str += '<td class="msgview">&nbsp;</td></tr>';
		if	(messages[i].isopen && messages[i].message != "")
		{
			if	(messages[i].status == 'N')
			{
				messages[i].status = 'r';
				nCount--;
				crc = true;
			}
			str += '<tr><td colspan="3">&nbsp;</td><td class="mbody" colspan="4"><p>';
			str += messages[i].message + '</p>';
			if	(messages[i].replink)
			{
				str += '<a class="rlink" href="#"' + msgtt[85] + 'onClick="composeReply(' + messages[i].replink + ');return false">[ ';
				str += replyText + ' ]</a>';
			}
			str += '</td><td>&nbsp;</td></tr>';
		}
		str += '</table></td></tr>';
	}

	str += '</table>';
	document.getElementById('msglist2').innerHTML = str;

	if	(crc)
	{
		drawMainMenu();
		updateHeader();
	}
}}

function	switchSort(t)
{with(folders[vFolder]){
	if	(settings[1] != t)
		settings[1] = t;
	settings[2] = !settings[2];
	x_setSortParameters(type, id, settings[1], settings[2] ? "1" : "0", emptyCB);
	buildMessageDisplay();
}}

function	setMsgPerPage()
{with(folders[vFolder]){
	var	sel = document.getElementById('fcmpp'), si = sel.selectedIndex;
	settings[0] = parseInt(sel.options[si].value, 10) * 10;
	x_setMessagesPerPage(type, id, sel.options[si].value, emptyCB);
	buildMessageDisplay();
}}

function	switchThreaded()
{with(folders[vFolder]){
	settings[3] = !settings[3];
	x_switchThreaded(type, id, emptyCB);
	buildMessageDisplay();
}}

function	changePage()
{with(folders[vFolder]){
	var	sel = document.getElementById('pgs'), si = sel.selectedIndex;
	settings[4] = parseInt(sel.options[si].value, 10);
	buildMessageDisplay();
}}

function	switchOpen(i)
{with(folders[vFolder]){
	messages[i].isopen = !messages[i].isopen;
	if	(messages[i].isopen && messages[i].message == "")
		x_getMessageText(messages[i].id, gotMessageText);
	else
		drawList();
}}

function	gotMessageText(data)
{
	var	i, a = data.split('\n');
	var	mId = a[0].split('#');
	if	(mId.length == 1)
		return;
	idx = findFolder(mId[0], mId[1]);
	if	(idx == -1)
		return;
	with (folders[idx])
	{
		for	(i=0;i<messages.length&&messages[i].id!=mId[2];i++)
			;
		if	(i == messages.length)
			return;
		if	(a.length > 2)
		{
			a.shift();
			a.shift();
			messages[i].message = a.join('\n');
		}
		else
			messages[i].message = msgNotFound;
	}
	drawList();
}

function	switchSelection(idx)
{with(folders[vFolder]){
	messages[idx].selected = !messages[idx].selected;
	if	(!messages[idx].selected && selAll)
	{
		selAll = false;
		document.getElementById('mlsa').checked = false;
	}
	updateControls();
}}

function	switchSelAll()
{with(folders[vFolder]){
	var	i,j=settings[0]*settings[4];
	selAll = !selAll;
	for	(i=0;i<messages.length;i++)
		messages[i].selected = selAll;
	for	(i=j;i<j+settings[0];i++)
	{
		var	idx = settings[3] ? threadOrder[i] : i;
		var	e = document.getElementById('msgck' + idx);
		if	(!e)
			break;
		e.checked = selAll;
	}
	updateControls();
}}

function	getPageSelection()
{with(folders[vFolder]){
	var	a = new Array();
	var	i, j = settings[0], k = j*settings[4];
	for	(i=k;i<j+k&&i<messages.length;i++)
		if	(messages[i].selected)
			a.push(messages[i].id);
	return	a;
}}

function	updateControls()
{
	var	str;
	var	s = getPageSelection();
	if	(s.length == 0)
		str = '&nbsp;';
	else
	{
		var	i = document.getElementById('msgmvt');
		if	(i)
			str = genMsgControls(i.options[i.selectedIndex].value);
		else
			str = genMsgControls('');
	}
	document.getElementById('msgctrl').innerHTML = str;
}

function	deleteSelected()
{with(folders[vFolder]){
	var	a = getPageSelection(), b = new Array();
	var	i;
	if	(!confirmDelete(a.length))
		return;
	for	(i=0;i<messages.length;i++)
		b.push(messages[i].id);
	clearTimeout(udFolder);
	x_deleteMessages(type, id, a.join('#'), b.join('#'), messageListReceived);
}}

function	moveSelected()
{with(folders[vFolder]){
	var	a = getPageSelection(), b = new Array();
	var	i, j, k;

	i = document.getElementById('msgmvt');
	j = i.selectedIndex;
	k = i.options[j].value;
	if	(k == "")
	{
		moveError();
		return;
	}
	c = k.split('#');
	if	(!c[1])
		c[1] = 0;

	for	(i=0;i<messages.length;i++)
		b.push(messages[i].id);

	clearTimeout(udFolder);
	x_moveMessages(type, id, c[0], c[1], a.join('#'), b.join('#'), messageListReceived);
}}




//--------------------
// FOLDER MANAGER
//--------------------

function	manageFolders()
{
	page = "Folders";
	buildFoldersList();
	drawMainMenu();
}

function	setParametersFolders(p)
{
	buildFoldersList();
}

function	buildFoldersList()
{
	drawFoldersList();
	updateFControls();
	updateHeader();
}

function	switchFSel(fid)
{
	var	a = fid.split('#');
	if	(a[0] != "CUS")
		a[1] = 0;
	var	i = findFolder(a[0], a[1]);
	if	(i == -1)
		return;
	folders[i].selected = !folders[i].selected;
	updateFControls();
}

function	updateFControls()
{
	var	str, fs, cd, i;
	if	(folders.length < 18)
		drawAddFolder();
	else
		document.getElementById('addfld').innerHTML = '&nbsp;';
	fs = 0;
	cd = true;
	for	(i=0;i<folders.length;i++)
	{
		if	(!folders[i].selected)
			continue;
		fs++;
		cd = cd && (folders[i].type == 'CUS');
	}
	if	(fs > 0)
		drawSFControls(fs, cd);
	else
		document.getElementById('chgfld').innerHTML = '&nbsp;';
}

function	renameFolder(fid)
{
	var	i = findFolder("CUS", fid);
	if	(i == -1)
		return;
	var	nn = "";
	while	(nn == "")
	{
		nn = promptRename(folders[i].name);
		if	(typeof nn == 'object' && !nn)
			return;
		if	(nn == "")
			alertFolderName(0);
		else if	(nn.length > 32)
		{
			alertFolderName(1);
			nn = "";
		}
	}
	if	(udFList)
		clearTimeout(udFList);
	x_renameFolder(fid, nn, parseFolderList);
}

function	addFolder()
{
	var	n = document.getElementById('adfldn').value;
	if	(n == "")
	{
		alertFolderName(0);
		return;
	}
	else if	(n.length > 32)
	{
		alertFolderName(1);
		return;
	}
	if	(udFList)
		clearTimeout(udFList);
	x_addFolder(n, parseFolderList);
}

function	deleteFolders()
{
	var	i, a = new Array(), tm = 0;
	for	(i=0;i<folders.length;i++)
	{
		if	(folders[i].selected)
		{
			a.push(folders[i].id);
			tm += parseInt(folders[i].mCount, 10);
		}
	}
	if	(tm > 0 && !confirmFDelete(a.length, tm))
		return;
	x_deleteFolders(a.join('#'), parseFolderList);
}

function	flushFolders()
{
	var	i, a = new Array(), tm = 0;
	for	(i=0;i<folders.length;i++)
	{
		if	(folders[i].selected)
		{
			a.push(folders[i].type + '!' + folders[i].id);
			tm += parseInt(folders[i].mCount, 10);
		}
	}
	if	(tm == 0 || !confirmFFlush(a.length, tm))
		return;
	x_flushFolders(a.join('#'), parseFolderList);
}

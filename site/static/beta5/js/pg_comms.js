var	dFolders, cFolders;
var	genForums, aForums;

var	comtt;


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
	commsDataReceived(document.getElementById('init-data').value);
}


function	commsDataReceived(data)
{
	var	i, l = data.split('\n');
	var	a = l.shift().split('#');
	var	nCustom = parseInt(a[0],10), nGenCats = parseInt(a[1],10), nAForums = parseInt(a[2],10);

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

	drawCommsPage();
	setTimeout('x_getCommsData(commsDataReceived)', 30000);
}


function	drawCommsPage()
{
	var	i, j, a;

	// Default folders
	for	(i=0;i<3;i++)
		document.getElementById('msg' + i).innerHTML = makeMessagesText(dFolders[i].tMsg, dFolders[i].nMsg);

	// Custom folders
	if	(cFolders.length == 0)
		document.getElementById('cflist').innerHTML = noCustomFolders;
	else
	{
		a = new Array();
		for	(i=0;i<cFolders.length;i++)
		{
			var	s = '<a href="message?a=f&f=C&cf=' + cFolders[i].id;
			s += '" ' + comtt[0] + ' >' + cFolders[i].name + '</a>: ' + makeMessagesText(cFolders[i].tMsg, cFolders[i].nMsg);
			a.push(s);
		}
		document.getElementById('cflist').innerHTML = a.join('<br/>') + '<br/>';
	}

	// General forums
	a = new Array();
	for	(i=0;i<genForums.length;i++)
	{with(genForums[i]){
		var	s = '<a href="forums?cmd=C%23G%23' + id + '" ' + comtt[1] + ' >' + name + '</a>';
		for	(j=0;j<forums.length;j++)
		{
			s += '<br/>&nbsp;&nbsp;-&nbsp;<a href="forums?cmd=F%23' + type + '%23' + forums[j].id + '" ' + comtt[2] + ' >' + forums[j].name + '</a>: ';
			s += makeTopicsText(forums[j].nTopics, forums[j].nUnread);
		}
		a.push(s);
	}}
	document.getElementById('gforums').innerHTML = a.join('<br/><br/>');

	// Alliance forums
	if	(aForums.length == 0)
		document.getElementById('aforums').innerHTML = '&nbsp;';
	else
	{
		a = new Array();
		for	(j=0;j<aForums.length;j++)
		{
			s  = '&nbsp;&nbsp;-&nbsp;<a href="forums?cmd=F%23A%23' + aForums[j].id + '" ' + comtt[3] + ' >' + aForums[j].name + '</a>: ';
			s += makeTopicsText(aForums[j].nTopics, aForums[j].nUnread);
			a.push(s);
		}
		document.getElementById('aforums').innerHTML = '<h2>' + allianceForums + '</h2><p>' + a.join('<br/>') + '</p>';
	}
}

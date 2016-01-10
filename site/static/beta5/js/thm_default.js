var	thmdef_hdTimer;
var	thmdef_stTimer;
var	thmdef_plTimer;
var	thmdef_msgIcon = null, thmdef_milock = false, thmdef_dIcon;
var	thmdef_mFolders, thmdef_fdTimer;
var	thm_sTime;


function	thmdef_activateMsgIcon()
{
	if	(thmdef_milock)
	{
		setTimeout('thmdef_activateMsgIcon()', 500);
		return;
	}

	thmdef_milock = true;
	if	(!thmdef_msgIcon)
	{
		thmdef_dIcon = false;
		thmdef_msgIcon = setTimeout('thmdef_blinkMsgIcon()', 200);
	}
	thmdef_milock = false;
}

function	thmdef_disableMsgIcon()
{
	if	(thmdef_milock)
	{
		setTimeout('thmdef_disableMsgIcon()', 500);
		return;
	}

	thmdef_milock = true;
	clearTimeout(thmdef_msgIcon);
	thmdef_msgIcon = null;
	document.getElementById('msgicon').innerHTML = '&nbsp;';
	document.getElementById('msgicon').onclick = null;
	thmdef_milock = false;
}

function thmdef_viewMessage() {
	document.location.href = 'message.redirect';
}

function	thmdef_blinkMsgIcon()
{
	if	(thmdef_milock)
	{
		setTimeout('thmdef_blinkMsgIcon()', 50);
		return;
	}
	thmdef_milock = true;

	var	e = document.getElementById('msgicon');
	if	(typeof e.onClick != 'function')
		e.onclick = thmdef_viewMessage;
	if	(thmdef_dIcon)
		e.innerHTML = '&nbsp;';
	else
		e.innerHTML = '<img src="'+staticurl+'/beta5/pics/icons/message.gif" alt="New message" />';
	thmdef_dIcon = !thmdef_dIcon;
	thmdef_msgIcon = setTimeout('thmdef_blinkMsgIcon()', 1000);
	thmdef_milock = false;
}


function	thmdef_updateTime()
{
	thm_sTime ++;
	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	thmdef_stTimer = setTimeout('thmdef_updateTime()', 1000);
}


function	thmdef_writeHeader(data)
{
	if	(thmdef_stTimer)
		clearTimeout(thmdef_stTimer);

	var	a = data.split("#");
	thm_sTime = parseInt(a.shift(), 10);

	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	document.getElementById('jspname').innerHTML = a.shift();
	document.getElementById('jscash').innerHTML = "&euro;" + formatNumber(a.shift());

	if	(a[0] == "1")
		thmdef_activateMsgIcon();
	else
		thmdef_disableMsgIcon();
	a.shift();

	if	(a[0] != "" || a.length > 1)
		document.getElementById('jsalliance').innerHTML = " [<b>" + a.join('#') + "</b>]";
	else
		document.getElementById('jsalliance').innerHTML = "";
	thmdef_stTimer = setTimeout('thmdef_updateTime()', 1000);
	thmdef_hdTimer = setTimeout('x_getHeaderData(thmdef_writeHeader)', 15000);
}

function	thmdef_writePlanets(data)
{
	if	(!document.getElementById('jspmenu'))
		return;

	var	ms = "";
	if	(data != '')
	{
		var	i, a = data.split("\n");
		for	(i=0;i<a.length;i++)
		{
			p = a[i].split('#');
			ms += "<li class='tmenu'><a class='tmenu' href='planet?id=" + p[0] + "'>";
			ms += p[1].replace(' ', '&nbsp;') + "</a></li>";
		}
	}
	else
		ms = '<li class="tmenu"><a class="tmenu" href="nplanet">' + thmdef_getPlanet + '</a></li>';
	document.getElementById('jspmenu').innerHTML = ms;
	thmdef_plTimer = setTimeout('x_getHeaderPList(thmdef_writePlanets)', 180000);
}

function	thmdef_ieDisplay(mid)
{
	document.getElementById(mid).style.display = 'block';
}

function	thmdef_ieHide(mid)
{
	document.getElementById(mid).style.display = 'none';
}

function	thmdef_writeFolders(data)
{
	if	(!document.getElementById('jsfmenu'))
		return;

	var	ms = thmdef_mFolders;
	if	(data != '')
	{
		var	i, a = data.split("\n");
		for	(i=0;i<a.length;i++)
		{
			var p = a[i].split('#');
			ms += "<li class='tmenu'><a class='tmenu' href='message?a=f&f=C&cf=" + p.shift() + "'>";
			ms += p.join('#').replace(' ', '&nbsp;') + "</a></li>";
		}
	}
	document.getElementById('jsfmenu').innerHTML = ms;
	thmdef_fdTimer = setTimeout('x_getHeaderFolders(thmdef_writeFolders)', 180000);
}

function	thmdef_initFolders()
{
	var	e = document.getElementById('jsfmenu');
	if	(!e)
		return;
	thmdef_mFolders = e.innerHTML;
	x_getHeaderFolders(thmdef_writeFolders);
}


function	updateHeader()
{
	if	(thmdef_hdTimer)
		clearTimeout(thmdef_hdTimer);
	if	(thmdef_stTimer)
		clearTimeout(thmdef_stTimer);
	if	(thmdef_plTimer)
		clearTimeout(thmdef_plTimer);
	if	(thmdef_fdTimer)
		clearTimeout(thmdef_fdTimer);

	x_getHeaderData(thmdef_writeHeader);
	x_getHeaderPList(thmdef_writePlanets);
	x_getHeaderFolders(thmdef_writeFolders);
}

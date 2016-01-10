var	thminv_hdTimer;
var	thminv_stTimer;
var	thminv_plTimer;
var	thminv_msgIcon = null, thminv_milock = false, thminv_dIcon;
var	thminv_mFolders, thminv_fdTimer;
var	thm_sTime;


function	thminv_activateMsgIcon()
{
	if	(thminv_milock)
	{
		setTimeout('thminv_activateMsgIcon()', 500);
		return;
	}

	thminv_milock = true;
	if	(!thminv_msgIcon)
	{
		thminv_dIcon = false;
		thminv_msgIcon = setTimeout('thminv_blinkMsgIcon()', 200);
	}
	thminv_milock = false;
}

function	thminv_disableMsgIcon()
{
	if	(thminv_milock)
	{
		setTimeout('thminv_disableMsgIcon()', 500);
		return;
	}

	thminv_milock = true;
	clearTimeout(thminv_msgIcon);
	thminv_msgIcon = null;
	document.getElementById('msgicon').innerHTML = '&nbsp;';
	document.getElementById('msgicon').onclick = null;
	thminv_milock = false;
}

function	thminv_viewMessage()
{
	document.location.href = 'message.redirect';
}

function	thminv_blinkMsgIcon()
{
	if	(thminv_milock)
	{
		setTimeout('thminv_blinkMsgIcon()', 50);
		return;
	}
	thminv_milock = true;

	var	e = document.getElementById('msgicon');
	if	(typeof e.onClick != 'function')
		e.onclick = thminv_viewMessage;
	if	(thminv_dIcon)
		e.innerHTML = '&nbsp;';
	else
		e.innerHTML = '<img src="'+staticurl+'/beta5/pics/icons/message.gif" alt="New message" />';
	thminv_dIcon = !thminv_dIcon;
	thminv_msgIcon = setTimeout('thminv_blinkMsgIcon()', 1000);
	thminv_milock = false;
}


function	thminv_updateTime()
{
	thm_sTime ++;
	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	thminv_stTimer = setTimeout('thminv_updateTime()', 1000);
}


function	thminv_writeHeader(data)
{
	if	(thminv_stTimer)
		clearTimeout(thminv_stTimer);

	var	a = data.split("#");
	thm_sTime = parseInt(a.shift(), 10);

	document.getElementById('jsservtm').innerHTML = formatDate(thm_sTime);
	document.getElementById('jspname').innerHTML = a.shift();
	document.getElementById('jscash').innerHTML = "&euro;" + formatNumber(a.shift());

	if	(a[0] == "1")
		thminv_activateMsgIcon();
	else
		thminv_disableMsgIcon();
	a.shift();

	if	(a[0] != "" || a.length > 1)
		document.getElementById('jsalliance').innerHTML = " [<b>" + a.join('#') + "</b>]";
	else
		document.getElementById('jsalliance').innerHTML = "";
	thminv_stTimer = setTimeout('thminv_updateTime()', 1000);
	thminv_hdTimer = setTimeout('x_getHeaderData(thminv_writeHeader)', 15000);
}

function	thminv_writePlanets(data)
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
		ms = '<li class="tmenu"><a class="tmenu" href="nplanet">' + thminv_getPlanet + '</a></li>';
	document.getElementById('jspmenu').innerHTML = ms;
	thminv_plTimer = setTimeout('x_getHeaderPList(thminv_writePlanets)', 180000);
}

function	thminv_ieDisplay(mid)
{
	document.getElementById(mid).style.display = 'block';
}

function	thminv_ieHide(mid)
{
	document.getElementById(mid).style.display = 'none';
}

function	thminv_writeFolders(data)
{
	if	(!document.getElementById('jsfmenu'))
		return;

	var	ms = thminv_mFolders;
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
	thminv_fdTimer = setTimeout('x_getHeaderFolders(thminv_writeFolders)', 180000);
}

function	thminv_initFolders()
{
	var	e = document.getElementById('jsfmenu');
	if	(!e)
		return;
	thminv_mFolders = e.innerHTML;
	x_getHeaderFolders(thminv_writeFolders);
}


function	updateHeader()
{
	if	(thminv_hdTimer)
		clearTimeout(thminv_hdTimer);
	if	(thminv_stTimer)
		clearTimeout(thminv_stTimer);
	if	(thminv_plTimer)
		clearTimeout(thminv_plTimer);
	if	(thminv_fdTimer)
		clearTimeout(thminv_fdTimer);

	x_getHeaderData(thminv_writeHeader);
	x_getHeaderPList(thminv_writePlanets);
	x_getHeaderFolders(thminv_writeFolders);
}

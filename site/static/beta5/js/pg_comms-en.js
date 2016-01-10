var	noCustomFolders = 'You do not have custom folders.';
var	allianceForums = 'Alliance Forums';

function        makeCommsTooltips()
{
        comtt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<5;i++)
			comtt[i] = "";
		return;
	}
	comtt[0] = tt_Dynamic("Click here to go to the page of this custom folder");
	comtt[1] = tt_Dynamic("Click here to go to the page of this general forums category");
	comtt[2] = tt_Dynamic("Click here to go to the page of this general forum");
	comtt[3] = tt_Dynamic("Click here to go to the page of this alliance forum");
}													

function	makeMessagesText(tot, n)
{
	if	(tot == 0)
		return	"no messages";
	var	str = '<b>' + formatNumber(tot) + '</b> message' + (tot > 1 ? 's' : '');
	if	(n == 0)
		return	str;
	str += ' (<b>' + formatNumber(n) + '</b> unread message'+(n>1?'s':'')+')';
	return	str;
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

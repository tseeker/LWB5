function        makeEmpireTooltips()
{
        emptt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<2;i++)
			emptt[i] = "";
		return;
	}
	emptt[0] = tt_Dynamic("Click here to go to this planet's individual page");
	emptt[1] = tt_Dynamic("Click here to get a new planet when you've lost all of yours");
}


function	empire_planets(d)
{
	var str;

	if	(d[3] != 'N/A')
		str = '<b>' + formatNumber(d[3]) + '</b> per planet';
	else
		str = 'N/A';
	document.getElementById('plapop').innerHTML = str;

	if	(d[5] != 'N/A')
		str = '<b>' + formatNumber(d[5]) + '</b> per planet';
	else
		str = 'N/A';
	document.getElementById('plafct').innerHTML = str;

	if	(d[7] != 'N/A')
		str = '<b>' + formatNumber(d[7]) + '</b> per planet';
	else
		str = 'N/A';
	document.getElementById('platrt').innerHTML = str;

	if	(d[8] != '0')
	{
		str = '<b class="phapbad">' + d[8] + ' planet';
		if	(d[8] > 1)
			str += 's';
		str += ' under attack!</b><br/>';
	}
	else
		str = '';
	document.getElementById('platt').innerHTML = str;
}


function	displayResearchStatus(na, nb)
{
	var	str;
	if	(na == 0 && nb == 0)
		return;
	str = '<h2>Status</h2><p>';
	if	(na > 0)
		str += 'Available technologies: <b>' + na + '</b>';
	if	(nb > 0)
		str += (na > 0 ? '<br/>' : '') + 'Foreseen breakthroughs: <b>' + nb + '</b>';
	document.getElementById('rsst').innerHTML = str;
}

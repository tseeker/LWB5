function	makeNextText(name)
{
	return	"Next " + name + ": ";
}

function	drawRoundRankings(points,rank)
{
	var	str;
	if	(points == "")
		str = 'You are too weak to be in the overall round rankings.';
	else
		str = 'Your overall round ranking is <b>#' + formatNumber(rank) + '</b> (<b>' + formatNumber(points) + '</b> points)';
	document.getElementById('rndrank').innerHTML = str;
}


function	getDaysText(p)
{
	return	"day" + (p?'s':'');
}

Rankings.pointsText		= 'points';
Rankings.noPlayersFound		= 'No players were found.';
Rankings.noAlliancesFound	= 'No alliances were found.';
Rankings.pageText		= 'Page __CP__ / __MP__';
Rankings.playersPerPageText	= 'Display __PS__ players / page';
Rankings.alliancesPerPageText	= 'Display __PS__ alliances / page';
Rankings.searchPlayerText	= 'Search for player : __TF__';
Rankings.searchAllianceText	= 'Search for alliance : __TF__';

Rankings.Summary.loadingText	= 'Please wait; loading ...';

Rankings.Headers.player		= 'Player name';
Rankings.Headers.alliance	= 'Alliance tag';
Rankings.Headers.ranking	= 'Ranking / Points';
Rankings.Headers.civRanking	= 'Civilization<br/>Ranking / Points';
Rankings.Headers.milRanking	= 'Military<br/>Ranking / Points';
Rankings.Headers.finRanking	= 'Financial<br/>Ranking / Points';


var	menuText = ['Summary', 'General Rankings', 'Detailed Rankings', 'Alliance Rankings', 'Overall Round Rankings', 'Inflicted Damage Ranking'];


function        makeRanksTooltips()
{
        rantt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=30;i<31;i++)
			rantt[i] = "";
		return;
	}
//	rantt[0] = tt_Dynamic("Use this drop down list to select the number of items to display on each page");
//	rantt[1] = tt_Dynamic("Use this text field to search a particular string in this ranking listing");
//	rantt[10] = tt_Dynamic("Use this drop down list to go to the selected page of this rankings");
//	rantt[20] = tt_Dynamic("Click here to sort the list according to this field and switch between ascending / descending order");
	rantt[30] = tt_Dynamic("Click here to go to this particular rankings listing");
}


Rankings.Summary.prototype.getText = function() {
	var	str = '<table cellspacing="0" cellpadding="0"><tr>';

	str += '<td class="div2"><h1>General ranking</h1><p>';
	str += 'This ranking corresponds to a combination of your civilisation, military and financial rankings. It represents your current overall advancement and strength in the game.</b>';
	str += '</p><p>Your general ranking: <b>#' + formatNumber(this.rkGen) + '</b> (<b>' + formatNumber(this.ptGen) + '</b> points)';
	str += '</p></td>';
	str += '<td><h1>Civilization Ranking</h1><p>';
	str += 'This ranking represents the advancement level of the society in your empire. It takes into account technology level, population and happiness.';
	str += '</p><p>Your civilization ranking: <b>#' + formatNumber(this.rkCiv) + '</b> (<b>' + formatNumber(this.ptCiv) + '</b> points)';
	str += '</p></td>';

	str += '</tr><tr></tr><tr>';

	str += '<td><h1>Overall Round Ranking</h1><p>';
	str += 'This ranking is calculated based on your previous general rankings. It allows for a long term estimate of your strength and advancement.';
	str += '</p><p>';
	if	(this.rkRnd == "")
		str += 'You are too weak to have an overall round ranking.';
	else
		str += 'Your overall ranking: <b>#' + formatNumber(this.rkRnd) + '</b> (<b>' + formatNumber(this.ptRnd) + '</b> points)';
	str += '</p></td>';
	str += '<td><h1>Military Ranking</h1><p>';
	str += 'This ranking allows to assess the military strength of your empire. Its calculation is based on the number of turrets and military factories you own along with your fleet fire power.';
	str += '</p><p>Your military ranking: <b>#' + formatNumber(this.rkMil) + '</b> (<b>' + formatNumber(this.ptMil) + '</b> points)';
	str += '</p></td>';

	str += '</tr><tr>';

	str += '<td><h1>Inflicted Damage Ranking</h1><p>';
	str += 'This ranking represents the amount of damage you have inflicted on other players\' fleets since you started playing.';
	str += '</p><p>Your inflicted damage ranking: <b>#' + formatNumber(this.rkDam) + '</b> (<b>' + formatNumber(this.ptDam) + '</b> points)';
	str += '</p></td>';
	str += '<td><h1>Financial Ranking</h1><p>';
	str += 'This ranking corresponds to the economic health of your empire. It takes into account your banked cash, your income and the number of industrial factories you own.';
	str += '</p><p>Your financial ranking: <b>#' + formatNumber(this.rkFin) + '</b> (<b>' + formatNumber(this.ptFin) + '</b> points)';
	str += '</p></td>';

	str += '</tr></table>';
	return	str;
}

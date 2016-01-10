var	pgTitles = ['Policy', 'Beacons'];
var	empPolTitle = "General Policy";
var	plaPolTitle = "Planet-specific Policy";
var	polTexts = ['probes from trusted allies', 'probes from alliance members', 'probes from other players', 'probes from enemies'];
var	polValue = ['Destroy', 'Jam', 'Allow'];
var	noPlanets = 'No planets were found.';
var	planetTxt = 'Planet';
var	polUse = 'Use a planet-specific policy instead of the general policy';
var	polUseEmpire = 'This planet follows the empire\'s general policy';
var	bcnCurrent = ['No beacon', 'Hyperspace Beacon', 'Probing Beacon', 'Advanced Warning Beacon'];
var	bcnNoUpgrade = 'There is no available upgrade for this planet\'s beacon.';
var	bcnAlreadyBuilt = 'A beacon or probe has been built on this planet recently; you must wait for the next Day Tick.';
var	bcnUpgrade = "You can upgrade this planet\'s beacon to the level of <b>__B__</b> (cost: <b>&euro;__C__</b>)";
var	bcnUpgradeBtn = "Upgrade Beacon";
var	bcnInfoLevel = ['Minimal', 'Very rough estimate', 'Rough estimate', 'Fleet size', 'Positive identification'];
var	bcnUnknown = 'Unknown';


function	PolicyPage_error(a)
{
	var	ec = parseInt(a.shift(), 10), str = 'Probes policy manager: error\n\n';
	switch	(ec)
	{
		case 0: str += 'Database error.'; break
		case 1: str += 'Invalid parameters.'; break;
		case 2: str += 'You no longer have control over this planet.'; break;
		default: str += 'Something weird has happened (unknown error).'; break;
	}
	alert(str);
}


function	BeaconsPage_error(a)
{
	var	ec = parseInt(a.shift(), 10), str = 'Beacons manager: error\n\n';
	switch	(ec)
	{
		case 0: str += 'Invalid parameters.'; break;
		case 1: str += 'You no longer have control over this planet.'; break;
		case 2: str += 'You don\'t have enough cash to pay for this upgrade.'; break;
		case 200: str += 'You can\'t upgrade beacons while in vacation mode'; break;
		default: str += 'Something weird has happened (unknown error).'; break;
	}
	alert(str);
}


function	BeaconsPage_confirmUpgrade(p)
{
	var	str = 'Are you sure you want to upgrade the beacon on planet\n' + p.name
			+ ' to the level of ' + bcnCurrent[p.cLevel + 1] + '\nfor ' + p.upgrade + ' euros?';
	return	confirm(str);
}

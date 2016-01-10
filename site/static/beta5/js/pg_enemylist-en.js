var	emptyListText = 'This list is empty.';
var	removeText = 'Remove selected enemies';

function        makeEnemyListTooltips()
{
        enltt = new Array();
	if      (ttDelay == 0)
	{
		var     i;
		for     (i=0;i<11;i++)
			enltt[i] = "";
		return;
	}
	enltt[0] = tt_Dynamic("Use this checkbox to select / unselect this enemy");
	enltt[10] = tt_Dynamic("Click here to remove the selected enemies from your enemy list");
}

function addAlert(ei) {
	var	str = 'Error\n';

	switch(ei) {
		case 0: str += 'Possible cheating detected.'; break;
		case 1: str += 'You must specify the name of the enemy to add.'; break;
		case 2: str += 'This player does not exist.'; break;
		case 3: str += 'This alliance does not exist.'; break;
		case 4: str += 'This player is already in your enemy list.'; break;
		case 5: str += 'This alliance is already in your enemy list.'; break;
		case 6: str += 'You can\'t add yourself to your enemy list.'; break;
		case 7: str += 'You can\'t add your own alliance to your enemy list.'; break;
		case 8: str += 'This player is in your trusted allies list.'; break;
		case 9: str += 'You can\'t add the Peacekeepers to your enemy list.'; break;
		case 200: str += 'You are not allowed to edit the enemy list\nwhile in vacation mode.'; break;
		default: str += 'An unknown error has occured.'; break;
	}
	alert(str);
}

function alertRemoveVacation () {
	alert('Error\nYou are not allowed to edit the enemy list\nwhile in vacation mode.');
}

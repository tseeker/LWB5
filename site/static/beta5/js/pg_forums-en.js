function	confirmDelete()
{
	var	i = countSelected();
	if	(i == 0)
	{
		alert('Please select the topic(s) you want to delete.');
		return	false;
	}
	return	confirm('Please confirm you want to delete ' + (i > 1 ? ('these ' + i + ' topics') : 'this topic') + '.');
}

function	confirmSticky()
{
	var	i = countSelected();
	if	(i == 0)
	{
		alert('Please select the topic(s) you want to switch to/from sticky.');
		return	false;
	}
	return	confirm('Please confirm you want to switch ' + (i > 1 ? ('these ' + i + ' topics') : 'this topic') + ' to/from sticky.');
}

function	confirmMove()
{
	var	i = countSelected();
	if	(i == 0)
	{
		alert('Please select the topic(s) you want to move.');
		return	false;
	}

	var	e = document.getElementById('mdest');
	if	(e.options[e.selectedIndex].value == '')
	{
		alert('Please select the forum to which the topic'+(i>1?'s':'')+' must be moved.');
		return	false;
	}

	return	confirm(
			'Please confirm you want to move the selected topic' + (i>1?'s':'') + '\nto the "'
			+ e.options[e.selectedIndex].text + '" forum.'
		);
}

function	confirmDTopic()
{
	return	confirm('Deleting this post will delete the whole topic. Please confirm.');
}

function	confirmDPost()
{
	return	confirm('Please confirm you want to delete this post.');
}

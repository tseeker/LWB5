function	rpc_alertSupport()
{
	alert("LegacyWorlds fatal error\nIt seems your browser has no support for asynchronous JavaScript.\nYou must upgrade your browser.");
}

function	rpc_alertFunction(name)
{
	alert("LegacyWorlds bug\nAn undefined RPC function has been called ('" + name + "')\nThis is a bug, please report it to webmaster@legacyworlds.com.");
}

function	rpc_alertCallback(name, aType, nArgs)
{
	var	str = 'Dumping args[]:\n' + nArgs.join('\n');
	alert("LegacyWorlds bug\nThe '"+name+"' RPC function was called without a callback function.\nThe last argument's type was: '"+aType
		+ "'.\nThis is a bug, please report it to webmaster@legacyworlds.com.\n\n" + str);
}

function	rpc_alertCallID(id)
{
	alert("LegacyWorlds bug\nThe server returned data for call ID '"+id+"' which can't be found in the queue.\nThis is a bug, please report it to webmaster@legacyworlds.com.");
}

function rpc_showErrorPage() {
	var str = '<h1>LegacyWorlds</h1><p>A connection error occured when the page tried to contact the server. This means that you were disconnected from the internet '
			+ 'or that the server is currently unavailable.<br/><br/>You may try <a href="#" onclick="window.location.href=window.location.href">reloading the '
			+ 'page</a> shortly if you wish to do so. Alternatively you can hang around and enjoy this cute error page.</p>';
	var t = document.getElementsByTagName("body");
	if (!t || !t[0])
		return;	// FIXME
	t[0].innerHTML = str;
}

function rpc_alertFatalError(code) {
	var codes = [
		"Could not open configuration file", "Could not connect to database", "Failed to set up tracking data",
		"Failed to set up tracking data", "Failed to set up tracking data", "Failed to set up tracking data",
		'Invalid request', 'Invalid request', 'Page not found', 'Page not found', 'Internal error', 'Internal error',
		"Failed to set up session data", "Failed to set up session data", "Failed to set up session data",
		"Failed to set up session data", 'Internal error', 'Internal error', "Internal error", 'Internal error',
		'Internal error', 'Internal error', 'Internal error', 'Internal error', 'Internal error', 'Internal error',
		'Invalid extension', 'Unhandled extension', 'Internal error', 'Internal error', 'Resource not found',
	];
	alert("A fatal error occured on the server.\nError " + code + ": " + codes[code] + "\nSorry for the inconvenience.");
}

function rpc_alertKicked(reason) {
	alert("You have been kicked from the game!\n" + (reason != "" ? ("Reason: " + reason) : "No reason was given"));
}

function rpc_alertUnkownError(data) {
	alert("LegacyWorlds bug\nThe server sent an unknown error code: " + data + "\nThis is a bug, please report it to webmaster@legacyworlds.com.");
}

function rpc_alertUnknownStatus(text) {
	alert("LegacyWorlds bug\nThe server sent an invalid RPC reply.\nThis is a bug, please report it to webmaster@legacyworlds.com."
		+ ("\n" + text));
}

var	rpc_objCode = '';
var	rpc_defaultMethod = "GET";
var	rpc_queueLock = false;
var	rpc_lockLock = false;
var	rpc_currentCalls = new Array();
var	rpc_functions = new Array();
var	rpc_failed = false;


// Calls a remote function on the server
function	rpc_doCall(name, pArguments)
{
	if	(rpc_failed)
		return;

	var	func = rpc_functions[name];
	if	(!func)
	{
		rpc_alertFunction(name);
		return;
	}

	var	args = new Array();
	var	callback, i;
	for	(i=0;i<pArguments.length-1;i++)
		args[i] = encodeURIComponent(pArguments[i]);
	callback = pArguments[i];
	if	(typeof callback != 'function')
	{
		rpc_alertCallback(name, typeof callback, args);
		return;
	}

	rpc_setLock();
	var	nCall = new rpc_Call(func, args, callback);
	rpc_currentCalls.push(nCall);
	rpc_resetLock();

	nCall.call();
}


// Locks the main RPC lock
function	rpc_setLock()
{
	while	(rpc_queueLock)
	{
		while	(rpc_lockLock) ;
		rpc_lockLock = true;
		if	(!rpc_queueLock)
		{
			rpc_queueLock = true;
			rpc_lockLock = false;
			break;
		}
		rpc_lockLock = false;
	}
}


// Unlocks the main RPC lock
function	rpc_resetLock()
{
	while	(rpc_lockLock) ;
	rpc_lockLock = true;
	rpc_queueLock = false;
	rpc_lockLock = false;
}


// Checks the queue of current calls for a specific identifier
function	rpc_isCalling(id)
{
	var	i = rpc_currentCalls.length;
	while	(i>0)
	{
		i--;
		if	(rpc_currentCalls[i].id == id)
			return	true;
	}
	return	false;
}


// Reads the queue and returns the RPC call identified by the ID passed by the
// caller. Returns null if the RPC call cannot be found.
function	rpc_getCall(id)
{
	for	(i=0;i<rpc_currentCalls.length;i++)
		if	(rpc_currentCalls[i].id == id)
			return	rpc_currentCalls[i];
	return	null;
}


// Removes a call object from the list of calls in progress.
function	rpc_removeCall(id)
{
	var	n = new Array();
	for	(i=0;i<rpc_currentCalls.length;i++)
		if	(rpc_currentCalls[i].id != id)
			n.push(rpc_currentCalls[i]);
	rpc_currentCalls = n;
	return;
}


// Initialises an object that will be able to handle RPC calls
function	rpc_initObject()
{
	var	A;
	if	(rpc_objCode == "E")
		return	null;
	else if	(rpc_objCode != "")
		eval('A='+rpc_objCode);
	else
	{
		try
		{
			A = new ActiveXObject("Msxml2.XMLHTTP");
			rpc_objCode = 'new ActiveXObject("Msxml2.XMLHTTP")';
		}
		catch (e)
		{
			try
			{
				A=new ActiveXObject("Microsoft.XMLHTTP");
				rpc_objCode = 'new ActiveXObject("Microsoft.XMLHTTP")';
			}
			catch (oc)
			{
				A = null;
			}
		}
		if	(!A && typeof XMLHttpRequest != "undefined")
		{
			A = new XMLHttpRequest();
			rpc_objCode = "new XMLHttpRequest()";
		}
	}
	if	(!A && typeof A == "object" && rpc_objCode != "E")
	{
		rpc_objCode = "E";
		rpc_alertSupport();
	}
	return A;
}


//-----------------------------------------------------------------
// rpc_Function object
//-----------------------------------------------------------------

// Initialises the object by specifying the name of the function,
// and its calling method. Adds the object to the function list, and
// generate the stub.
function	rpc_Function(name, method)
{
	this.name	= name;
	this.method	= method ? method : rpc_defaultMethod;

	rpc_functions[name] = this;
	eval('x_'+name+'=function(){rpc_doCall("'+name+'",arguments)}');
}


//-----------------------------------------------------------------
// rpc_Call object
//-----------------------------------------------------------------

// Initialises the object using the function object, arguments and
// callback passed by the main function.
function	rpc_Call(func, args, callback)
{
	this.funcObj	= func;
	this.arguments	= args;
	this.callback	= callback;

	var	rts = new Date().getTime();
	while	(rpc_isCalling(rts) || rts == "")
		rts++;
	this.id = rts;

	this.inCall	= false;
	this.retries	= 0;
	this.timeout	= null;
	this.timestamp	= 0;
	this.object	= null;

	this.call	= rpc_Call_call;
	this.retry	= rpc_Call_retry;
	this.send	= rpc_Call_send;
}


// Prepares a RPC call using the data stored within the object
function	rpc_Call_call()
{
	if	(this.inCall)
		return;
	this.inCall = true;
	this.send();
}


// Tries re-executing a RPC call
function	rpc_Call_retry()
{
	if	(!this.inCall)
		return;

	delete this.object;
	clearTimeout(this.timeout);

	if	(this.retries == 3)
	{
		rpc_failed = true;
		rpc_showErrorPage();
		rpc_resetLock();
		return;
	}
	this.retries ++;
	rpc_resetLock();
	this.send();
}


// Sends a request to the server
function	rpc_Call_send()
{
	var	uri = rpc_pageURI;
	var	postData, mks;
	var	i, obj;

	mks = 'rs=' + escape(this.funcObj.name);
	if	(this.arguments.length)
		mks += '&rsargs[]=' + this.arguments.join('&rsargs[]=');
	mks += '&rsId=' + this.id;

	if	(this.funcObj.method == "GET")
	{
		postData = null;
		uri += '?' + mks;
	}
	else
		postData = mks;

	obj = rpc_initObject();
	if	(!obj)
	{
		rpc_setLock();
		rpc_failed = true;
		rpc_resetLock();
		return;
	}

	obj.open(this.funcObj.method, uri, true);
	if	(this.funcObj.method == "POST")
	{
		obj.setRequestHeader("Method", "POST " + uri + " HTTP/1.1");
		obj.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	}

	eval('obj.onreadystatechange=function(){rpc_Call_returned('+this.id+')}');
	this.timeout = setTimeout('rpc_Call_timeout('+this.id+')', 25000);
	this.object = obj;
	obj.send(postData);
	delete obj;
}


// This function is used as the callback for all RPC functions
function	rpc_Call_returned(id)
{
	rpc_setLock();

	if	(rpc_failed)
	{
		rpc_resetLock();
		return;
	}

	var	c = rpc_getCall(id);
	if	(!c || !c.inCall)
	{
		rpc_failed = true;
		rpc_resetLock();
		rpc_alertCallID(id);
		return;
	}

	var	obj = c.object;
	if	(obj.readyState != 4)
	{
		rpc_resetLock();
		return;
	}

	try
	{
		if	(obj.status != 200)
		{
			c.retry();
			return;
		}
	}
	catch	(e)
	{
		c.retry();
		return;
	}

	var stat = obj.responseText.charAt(0);
	var data = obj.responseText.substring(2);

	if (stat == "-") {
		rpc_failed = true;
		if (data == "m" || data == "a") {
			window.location.href=window.location.href;
		} else if (data.indexOf('f:') == 0) {
			rpc_alertFatalError(data.substring(2));
		} else if (data.indexOf('k:') == 0) {
			rpc_alertKicked(data.substring(2));
			window.location.href=window.location.href;
		} else {
			rpc_alertUnknownError(data);
			rpc_showErrorPage();
		}
		return;
	} else if (stat != '+') {
		rpc_failed = true;
		rpc_alertUnknownStatus(stat.charCodeAt(0) + " " + stat + "\n" + obj.responseText);
		rpc_showErrorPage();
		return;
	}

	c.inCall = false;
	clearTimeout(c.timeout);
	c.timestamp = new Date().getTime();
	rpc_removeCall(c.id);
	rpc_resetLock();

	c.callback(data);
}


// Timeout function
function	rpc_Call_timeout(id)
{
	rpc_setLock();

	if	(rpc_failed)
	{
		rpc_resetLock();
		return;
	}

	var	c = rpc_getCall(id);
	if	(!c || !c.inCall || (new Date().getTime()) - c.timestamp < 23000)
	{
		rpc_resetLock();
		return;
	}

	c.object.abort();
	c.object = null;

	c.retry();
}

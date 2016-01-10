/** This class allows the user to load data files from the server using the browser's
 * native XML loader class.
 */
Base.XMLLoader = Base.Comp.inherits({

	/** This property indicates whether the instance is currently loading data
	 * from the server.
	 */
	isLoading:	false,

	/** This property contains the native XML loader object when data is being
	 * loaded or null when the loader isn't active.
	 */
	xmlObject:	null,

	/** This property contains the loader's timer when data is being retrieved.
	 * The timer will be used to force the loader object to timeout. When no
	 * data is being loaded, the property contains the null value.
	 */
	timer:		null,


	/** The constructor initialises the component itself and sets default values
	 * for the URI, method and timeout to use.
	 * @param	uri	a string containing the URI from which the data is
	 *				to be fetched
	 * @param	usePost	a boolean indicating whether the loader should use
	 *				the HTTP POST method (true) or GET method
	 * @param	timeout	an optionnal integer containing the amount of seconds
	 *				before the loader times out; this defaults to
	 *				20 seconds if the parameter is missing
	 */
	constructor: function (uri, usePost, timeout) {
		this.base();

		this.addEvent('Aborted');
		this.addEvent('Loaded');
		this.addEvent('NetworkError');
		this.addEvent('ServerError');
		this.addEvent('Timeout');
		this.addEvent('Unsupported');

		this.addSlot('load');
		this.addSlot('abort');
		this.addSlot('timedOut');

		this.uri	= uri;
		this.usePost	= usePost;
		this.timeout	= timeout ? timeout : 20;
	},

	/** The destructor starts by aborting the loader's operation, then calls
	 * the standard component destructor.
	 */
	destroy: function () {
		this.abort();
		this.base();
	},


	/** This method allows the user to change the parameters such as the URI, the
	 * method to be used and the timeout value. It will only be able to do so if
	 * the loader isn't operating.
	 * @param	uri	the new data URI (ignored if null)
	 * @param	usePost	the new method to use (ignored if null)
	 * @param	timeout	the new value for the loader's timeout (ignored if
	 *				null)
	 * @returns		true if the values were set, false if they weren't
	 */
	changeParameters: function (uri, usePost, timeout) {
		if (this.isLoading) {
			return false;
		}

		this.uri	= uri ? uri : this.uri;
		this.usePost	= (typeof usePost == 'boolean') ? usePost : this.usePost;
		this.timeout	= timeout ? timeout : 20;

		return true;
	},


	/** This method loads the data using the object's URI and method using the
	 * specified parameters.
	 * @param parameters	a hashtable associating to each of the parameters'
	 *				name a value that is either a string or an
	 *				array
	 * @returns		true if the loader was successfully started
	 */
	load: function (parameters) {
		// Make sure we're not already loading
		if (this.isLoading) {
			return false;
		}
		this.isLoading = true;
		Base.Log.write('Base.XMLLoader.load(): ' + (this.usePost ? 'POST' : 'GET') + ' ' + this.uri);

		// Initialise the native loader object
		var obj = Base.XMLLoader.makeNativeInstance();
		if (!obj) {
			this.isLoading = false;
			Base.Log.write('Base.XMLLoader.load(): XMLHttpRequest unsupported');
			this.onUnsupported();
			return false;
		}

		// Generate the parameters string and the URI
		var uri = this.uri, ps;
		if (parameters) {
			ps = Base.XMLLoader.encodeParameters(parameters); 
			if (!this.usePost) {
				uri += '?' + ps;
			}
		} else {
			ps = "";
		}

/*
		// Safari and IE are stupid
		if (Base.Browser.get().safari || Base.Browser.get().IE) {
			if (this.usePost || ps == '') {
				uri += '?stoopid_browsa=' + (new Date()).getTime();
			} else {
				uri += '&stoopid_browsa=' + (new Date()).getTime();
			}
		}
*/

		// Get ready to send the request
		try {
			obj.open(this.usePost ? "POST" : "GET", uri, true);
		} catch (e) {
			// We got a permission problem
			this.isLoading = false;
			Base.Log.write('Base.XMLLoader.load(): XMLHttpRequest.open() failed, possible permission problem');
			Base.Log.write('Base.XMLLoader.load(): exception was ' + (e.toString ? e.toString() : e));
			this.onUnsupported();
			return false;
		}
		if (this.usePost) {
			obj.setRequestHeader("Method", "POST " + uri + " HTTP/1.1");
			obj.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		}
		// Safari is still stupid
		if (Base.Browser.get().safari) {
			obj.setRequestHeader('If-Modified-Since', 'Wed, 15 Nov 1995 00:00:00 GMT');
			obj.setRequestHeader("Cache-Control", "no-cache");
		}

		// Initialise the timer
		this.timer = new Base.Timer(this.timeout * 1000);
		this.timer.bindEvent('Tick', 'timedOut', this);

		// Prepare the callback
		var _id = this._cid;
		obj.onreadystatechange = function () {
			if (typeof Base != 'undefined') {
				Base.Comp.get(_id).loaderEvent();
			}
		};

		// Start the timer and send the request
		this.xmlObject = obj;
		this.timer.start();
		obj.send((parameters && this.usePost) ? ps : null);
		return true;
	},

	/** This method cancels the current operation if there is one.
	 */
	abort: function () {
		if (!this.isLoading) {
			return;
		}

		this.doAbort();
		this.onAborted();
	},


	/** This method is used as a callback for the native loader object.
	 */
	loaderEvent: function () {
		if (!this.isLoading) {
			return;
		}

		var obj = this.xmlObject;
		try {
			if (obj.readyState != 4) {
				return;
			}

			if (obj.status != 200) {
				this.doAbort();
				Base.Log.write('Base.XMLLoader.load(): server returned ' + obj.status);
				this.onServerError(obj.status);
				return;
			}
		} catch	(e) {
			this.doAbort();
			Base.Log.write('Base.XMLLoader.load(): exception ' + (e.toString ? e.toString() : e) + ' while receiving');
			this.onNetworkError(e);
			return;
		}

		var _r = obj.responseText;
		this.doAbort();
		this.onLoaded(_r);
	},

	/** This method is responsible for deleting existing objects such as
	 * the native loader and the timer component.
	 */
	doAbort: function () {
		this.isLoading = false;

		if (this.xmlObject) {
			this.xmlObject.abort();
			delete this.xmlObject;
			this.xmlObject = null;
		}

		if (this.timer) {
			this.timer.destroy();
			this.timer = null;
		}
	},

	/** This method is called by the timer when the time imparted for loading the
	 * data expires.
	 */
	timedOut: function () {
		if (!this.isLoading) {
			return;
		}
		Base.Log.write('Base.XMLLoader: timeout :(');
		this.doAbort();
		this.onTimeout();
	}

}, {

	/** This class property stores the code to be used to generate native
	 * loader objects. If it is empty, the code hasn't been determined yet,
	 * and the "E" value indicates an error.
	 */
	objCode:	"",

	/** This class method generates native loader objects. If the objCode
	 * class property has been initialised, it uses its content to generate
	 * the new object. Otherwise it explores available options to try and
	 * generate a native loader object.
	 * @returns	the native loader instance on success, null on failure
	 */
	makeNativeInstance: function () {
		var obj;

		if (this.objCode == "E") {
			return null;
		}

		if (this.objCode != "") {
			eval('obj=' + this.objCode);
		} else {
			try {
				obj = new ActiveXObject("Msxml2.XMLHTTP");
				this.objCode = 'new ActiveXObject("Msxml2.XMLHTTP")';
			} catch (e) {
				try {
					obj = new ActiveXObject("Microsoft.XMLHTTP");
					this.objCode = 'new ActiveXObject("Microsoft.XMLHTTP")';
				} catch (oc) {
					obj = null;
				}
			}
			if (!obj && typeof XMLHttpRequest != "undefined") {
				obj = new XMLHttpRequest();
				this.objCode = "new XMLHttpRequest()";
			}
		}

		if (!obj && typeof obj == "object" && this.objCode != "E") {
			this.objCode = "E";
			obj = null;
		}

		return obj;
	},

	/** This class method encodes a hashtable containing request parameters into
	 * a string usable when loading the data.
	 * @param parameters	the hashtable containing the parameters to be encoded.
	 * @returns		the encoded string
	 */
	encodeParameters: function (parameters) {
		var mk = new Array(), pl = parameters.keys();
		for (var i in pl) {
			var _k = pl[i], _pn = encodeURIComponent(_k), _v = parameters.get(_k);

			if (_v instanceof Array) {
				for (var j in _v) {
					mk.push(_pn + '[]=' + encodeURIComponent(_v[j]));
				}
			} else {
				mk.push(_pn + '=' + encodeURIComponent(_v));
			}
		}

		return mk.join('&');
	}

});

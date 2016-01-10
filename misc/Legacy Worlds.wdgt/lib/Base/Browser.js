/** The Base.Browser class contains the code that detects the user's browser and keeps
 * track of the current window's size.
 */
Base.Browser = Base.Comp.inherits({

	/** The constructor detects the browser type, then initialises the window size
	 * watching code.
	 */
	constructor: function () {
		if (Base.Browser.singleton) {
			throw "SingletonException";
		}

		this.base();

		// Get the document body object
		this.docBody = (document.compatMode && document.compatMode != 'BackCompat') ? document.documentElement : (document.body ? document.body : null);

		// Detect the browser's type
		var nav = navigator.userAgent.toLowerCase(), ver = navigator.appVersion;
		this.opera	= window.opera && document.getElementById;
		this.ie		= nav.indexOf("msie") != -1 && document.all && this.docBody && !this.opera;
		this.opera6	= this.opera && !document.defaultView;
		this.operaOther	= this.opera && !this.opera6;
		this.ie7	= this.ie && parseFloat(ver.substring(ver.indexOf("MSIE")+5)) >= 7;
		this.ie6	= this.ie && !this.ie7 && parseFloat(ver.substring(ver.indexOf("MSIE")+5)) >= 5.5;
		var ns		= !this.opera && document.defaultView && (typeof document.defaultView.getComputedStyle != "undefined");
		this.konqueror	= ns && nav.indexOf('konqueror') != -1;
		this.safari	= ns && nav.indexOf('applewebkit') != -1;
		this.ns6	= ns && !this.konqueror && !this.safari;
		this.supported	= this.docBody && (this.ie6 || this.ie7 || this.opera || this.ns6 || this.konqueror || this.safari);

		// Prepare the window size watching code
		this.addEvent('SizeChanged');
		var _cid = this._cid;
		window.onresize = function () {
			Base.Comp.get(_cid).handleResize();
		};
		if (this.konqueror) {
			this.addSlot('resizeTimer');
			this.timer = new Base.Timer(250, false);
			this.timer.bindEvent('Tick', 'resizeTimer', this);
		}
		this.readSize();
	},

	/** This method reads the current window's size.
	 */
	readSize: function () {
		this.width = (this.ie ? this.docBody.offsetWidth : window.innerWidth);
		this.height = (this.ie ? this.docBody.offsetHeight : window.innerHeight);
	},

	/** This method is called by the resize handler or, if the user is running Konqueror,
	 * by the timer that keeps track of resizing. It reads the current size, and if that
	 * size is different from the old size, it sends a SizeChanged(width,height) event.
	 */
	resizeTimer: function () {
		var ow = this.width, oh = this.height;
		this.readSize();
		if (ow != this.width || oh != this.height) {
			this.onSizeChanged(this.width, this.height);
		}
	},

	/** This method either forces the object to re-check the window's size or, if the
	 * user is running Konqueror, starts or restarts the update timer to prevent
	 * receiving multiple window update codes.
	 */
	handleResize: function () {
		if (this.konqueror) {
			this.timer.restart();
			return;
		}
		this.resizeTimer();
	}

}, {

	/** This class property contains the single Base.Browser instance.
	 */
	singleton: null,

	/** This class method returns the Base.Browser instance after creating it
	 * if it didn't exist.
	 */
	get: function () {
		if (!Base.Browser.singleton) {
			Base.Browser.singleton = new Base.Browser();
		}
		return Base.Browser.singleton;
	}

});

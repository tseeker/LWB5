LWWidget.VersionCheck = Base.Comp.inherits({

	constructor: function () {
		this.base();

		this.addEvent('CommError');
		this.addEvent('NewVersion');
		this.addEvent('RequiredVersion');
		this.addEvent('VersionOk');

		this.addSlot('dataReceived');
		this.addSlot('loaderDestroyed');
		this.addSlot('timerDestroyed');

		this.loader = new Base.XMLLoader(LWWidget.base + '/index.php/main/macwidget.xml', false);
		this.loader.bindEvent('Destroy', 'loaderDestroyed', this);
		this.loader.bindEvent('NetworkError', 'onCommError', this);
		this.loader.bindEvent('ServerError', 'onCommError', this);
		this.loader.bindEvent('Timeout', 'onCommError', this);
		this.loader.bindEvent('Unsupported', 'onCommError', this);
		this.loader.bindEvent('Aborted', 'onCommError', this);
		this.loader.bindEvent('Loaded', 'dataReceived', this);

		this.timer = new Base.Timer(300000, true);
		this.timer.bindEvent('Tick', 'load', this.loader);
		this.timer.bindEvent('Destroy', 'timerDestroyed', this);

		this.knownVersion = LWWidget.version;
	},


	destroy: function () {
		if (this.loader) {
			this.loader.destroy();
		}
		if (this.timer) {
			this.timer.destroy();
		}
		this.base();
	},


	timerDestroyed: function () {
		this.timer = null;
	},

	loaderDestroyed: function () {
		this.loader = null;
	},


	start: function () {
		if (typeof this.requiredVersion != 'undefined' && LWWidget.version < this.requiredVersion) {
			return;
		}
		this.timer.start();
		this.loader.load();
	},

	stop: function () {
		this.timer.stop();
		this.knownVersion = LWWidget.version;
		delete this.latestVersion;
		delete this.requiredVersion;
	},


	dataReceived: function (data) {
		var _xml = Base.Util.parseXML(data);
		if (!_xml) {
			this.onCommError();
			return;
		}

		var _doc = _xml.documentElement;
		this.latestVersion	= parseInt(_doc.getAttribute('latest'), 10);
		this.requiredVersion	= parseInt(_doc.getAttribute('oldestOk'), 10);
		this.url		= _doc.childNodes[0].nodeValue;

		if (this.knownVersion == this.latestVersion && this.requiredVersion <= LWWidget.version) {
			this.onVersionOk();
			return;
		}

		Base.Log.write('Version check: current version = ' + LWWidget.version);
		Base.Log.write('Version check: latest received version = ' + this.knownVersion);
		Base.Log.write('Version check: latest version = ' + this.latestVersion);
		Base.Log.write('Version check: oldest accepted version = ' + this.requiredVersion);

		this.knownVersion = this.latestVersion;
		if (LWWidget.version < this.requiredVersion) {
			this.onRequiredVersion(this.url);
			this.timer.stop();
		} else if (LWWidget.version < this.latestVersion) {
			this.onNewVersion(this.url);
		} else {
			this.onVersionOk();
		}
	}

});
